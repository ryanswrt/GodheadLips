require "system/model"

Object = Class()
Object.class_name = "Object"

--- Finds objects.
-- @param clss Object class.
-- @param args Arguments.<ul>
--   <li>id: Object ID for ID search.</li>
--   <li>point: Center point for radius search.</li>
--   <li>radius: Search radius for radius search.</li>
--   <li>sector: Return all object in this sector.</li></ul>
-- @return Table of matching objects.
Object.find = function(self, args)
	if args.id then
		-- Search by ID.
		local obj = Object.objects[args.id]
		if not obj then return end
		-- Optional distance check.
		if args.point and args.radius then
			if not obj.realized then return end
			if (obj.position - args.point).length > args.radius then return end
		end
		return obj
	else
		-- Search by position or sector.
		local list = Los.object_find{
			point = args.point and args.point.handle,
			radius = args.radius,
			sector = args.sector}
		for k,v in pairs(list) do list[k] = __userdata_lookup[v] end
		-- Create an easily searchable dictionary.
		local dict = {}
		for k,v in pairs(list) do
			dict[v.id] = v
		end
		return dict
	end
end

--- Creates a new object.
-- @param clss Object class.
-- @param args Arguments.
-- @return New object.
Object.new = function(clss, args)
	local self = Class.new(clss)
	self.handle = Los.object_new()
	__userdata_lookup[self.handle] = self
	if args then
		for k,v in pairs(args) do
			if k ~= "realized" then self[k] = v end
		end
		if args.realized then self.realized = true end
	end
	return self
end

--- Recalculates the bounding box of the model of the object.
-- @param self Object.
Object.calculate_bounds = function(self)
	Los.object_calculate_bounds(self.handle)
end

--- Prevents map sectors around the object from being unloaded.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>radius: Refresh radius.</li></ul>
Object.refresh = function(self, args)
	Los.object_refresh(self.handle, args)
end

--- The local bounding box of the object.
-- @name Object.bounding_box
-- @class table

--- The local center offset of the bounding box of the object.
-- @name Object.center_offset
-- @class table

--- The model of the object.
-- @name Object.model
-- @class table

--- The position of the object.
-- @name Object.position
-- @class table

--- The realization status of the object.
-- @name Object.realized
-- @class table

--- The rotation of the object.
-- @name Object.rotation
-- @class table

--- The map sector of the object (read-only).
-- @name Object.sector
-- @class table

Object:add_getters{
	bounding_box = function(self)
		local m = rawget(self, "__model")
		if not m then return Aabb{point = Vector(-0.1,-0.1,-0.1), size = Vector(0.2,0.2,0.2)} end
		return m.bounding_box
	end,
	center_offset = function(self)
		local m = rawget(self, "__model")
		if not m then return Vector() end
		return m.center_offset
	end,
	model = function(s) return rawget(s, "__model") end,
	model_name = function(s)
		local m = rawget(s, "__model")
		return m and m.name or ""
	end,
	position = function(s) return Class.new(Vector, {handle = Los.object_get_position(s.handle)}) end,
	rotation = function(s) return Class.new(Quaternion, {handle = Los.object_get_rotation(s.handle)}) end,
	realized = function(s) return Los.object_get_realized(s.handle) end,
	sector = function(s) return Los.object_get_sector(s.handle) end}

Object:add_setters{
	model = function(s, v)
		local m = v
		if type(v) == "string" then m = Model:find_or_load{file = v, mesh = Object.load_meshes} end
		rawset(s, "__model", m)
		Los.object_set_model(s.handle, m and m.handle)
	end,
	model_name = function(s, v) s.model = Model:find_or_load{file = v, mesh = Object.load_meshes} end,
	position = function(s, v) Los.object_set_position(s.handle, v.handle) end,
	rotation = function(s, v) Los.object_set_rotation(s.handle, v.handle) end,
	realized = function(s, v)
		if v then
			if __objects_realized[s] then return end
			__objects_realized[s] = true
			Los.object_set_realized(s.handle, true)
			Program:push_event{type = "object-visibility", object = s, visible = true}
		else
			if not __objects_realized[s] then return end
			__objects_realized[s] = nil
			Los.object_set_realized(s.handle, false)
			Program:push_event{type = "object-visibility", object = s, visible = false}
		end
	end}

Object.unittest = function()
	-- Getters and setters.
	do
		local obj = Object{position = Vector(1,2,3), realized = true}
		assert(obj.position.class == Vector)
		assert(obj.position.x == 1 and obj.position.y == 2 and obj.position.z == 3)
		assert(obj.rotation.class == Quaternion)
		assert(obj.realized)
		assert(obj.sector == 0)
		obj.realized = false
		assert(obj.sector == nil)
		collectgarbage()
	end
	-- Name field.
	do
		local obj = Object()
		assert(obj.name == nil)
		assert(obj.class_name == "Object")
		obj.name = "Yui"
		assert(obj.name == "Yui")
		obj.name = nil
		assert(obj.name == nil)
		collectgarbage()
	end
	-- Keeping realized objects.
	for i = 1,100 do Object{model = Model(), position = Vector(10*i,50,50), realized = true} end
	collectgarbage()
	local num = 0
	for k,v in pairs(__objects_realized) do num = num + 1 end
	assert(num == 100)
	-- Physics simulation.
	for i = 1,1000 do
		local o = Object{model = Model(), position = Vector(50,50,50), realized = true}
		Program:update()
		o.realized = false
	end
	-- Unloading objects.
	Program:unload_world()
	for k,v in pairs(__objects_realized) do assert(false) end
	collectgarbage()
end

__objects_realized = {}

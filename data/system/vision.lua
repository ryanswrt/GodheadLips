require "system/class"

if not Los.program_load_extension("vision") then
	error("loading extension `vision' failed")
end

------------------------------------------------------------------------------

Vision = Class()
Vision.class_name = "Vision"
Vision.dict = {}

--- Creates a vision manager.
-- @param clss Vision class.
-- @param args Arguments.
-- @return New tile.
Vision.new = function(clss, args)
	local self = Class.new(clss)
	self.handle = Los.vision_new()
	self.objects = setmetatable({}, {__mode = "k"})
	__userdata_lookup[self.handle] = self
	if args then
		for k,v in pairs(args) do self[k] = v end
	end
	return self
end

--- Emits a custom vision event.
-- @param clss Vision class.
-- @param args Arguments.
Vision.event = function(clss, args)
	if args.id then args.object = Object:find{id = args.id} end
	if not args.point and not args.object then return end
	if args.object then
		-- Object events.
		for k in pairs(clss.dict) do
			if k.callback and k.objects[args.object] then
				k.callback(args)
			end
		end
	else
		-- Point events.
		for k in pairs(clss.dict) do
			if k.callback and (k.position - args.point).length <= k.radius then
				k.callback(args)
			end
		end
	end
end

--- Clears the vision.
-- @param self Vision.
Vision.clear = function(self)
	Los.vision_clear(self.handle)
	self.objects = {}
end

--- Updates the vision.
-- @param self Vision.
-- @return Table of events.
Vision.update = function(self)
	local e = Los.vision_update(self.handle)
	for k,v in ipairs(e) do
		-- Translate the object handle.
		if v.object then
			v.object = __userdata_lookup[v.object]
			self.objects[v.object] = (v.type == "object-shown") and true or nil
		end
		-- Call the callback.
		if self.callback then self.callback(v) end
	end
	return e
end

Vision:add_getters{
	cone_angle = function(self) return Los.vision_get_cone_angle(self.handle) end,
	cone_factor = function(self) return Los.vision_get_cone_factor(self.handle) end,
	direction = function(self)
		return Class.new(Vector, {handle = Los.vision_get_direction(self.handle)})
	end,
	enabled = function(self) return Vision.dict[self] end,
	position = function(self)
		return Class.new(Vector, {handle = Los.vision_get_position(self.handle)})
	end,
	radius = function(self) return Los.vision_get_radius(self.handle) end,
	threshold = function(self) return Los.vision_get_threshold(self.handle) end}

Vision:add_setters{
	cone_angle = function(self, v) Los.vision_set_cone_angle(self.handle, v) end,
	cone_factor = function(self, v) Los.vision_set_cone_factor(self.handle, v) end,
	direction = function(self, v) Los.vision_set_direction(self.handle, v.handle) end,
	enabled = function(self, v) Vision.dict[self] = v and true or nil end,
	position = function(self, v) Los.vision_set_position(self.handle, v.handle) end,
	radius = function(self, v) Los.vision_set_radius(self.handle, v) end,
	threshold = function(self, v) Los.vision_set_threshold(self.handle, v) end}

Vision.unittest = function()
	local evt
	-- Initialization.
	local vis = Vision{position = Vector(100,200,300), radius = 2, threshold = 1}
	assert(vis.radius == 2)
	assert(vis.threshold == 1)
	-- Creating unrealized objects.
	local obj = Object{position = Vector(100,200,300)}
	evt = vis:update()
	assert(#evt == 0)
	-- Realizing within the sphere.
	obj.realized = true
	evt = vis:update()
	assert(#evt == 1)
	assert(evt[1].type == "object-shown")
	assert(evt[1].object == obj)
	assert(vis.objects[obj])
	-- Moving within the sphere.
	obj.position = obj.position + Vector(1.8)
	evt = vis:update()
	assert(#evt == 0)
	-- Moving within the keep threshold.
	obj.position = obj.position + Vector(1)
	evt = vis:update()
	assert(#evt == 0)
	-- Leaving the sphere.
	obj.position = obj.position + Vector(1.2)
	evt = vis:update()
	assert(#evt == 1)
	assert(evt[1].type == "object-hidden")
	assert(evt[1].object == obj)
	assert(not vis.objects[obj])
	-- Moving outside the sphere.
	obj.position = obj.position - Vector(1.5)
	evt = vis:update()
	assert(#evt == 0)
	-- Moving into the sphere.
	obj.position = obj.position - Vector(1)
	evt = vis:update()
	assert(#evt == 1)
	assert(evt[1].type == "object-shown")
	assert(evt[1].object == obj)
	-- Unrealizing within the sphere.
	obj.realized = false
	evt = vis:update()
	assert(#evt == 1)
	assert(evt[1].type == "object-hidden")
	assert(evt[1].object == obj)
	-- Multiple events.
	obj.realized = true
	evt = vis:update()
	local obj1 = Object{position = Vector(100,200,300), realized = true}
	obj.realized = false
	evt = vis:update()
	assert(#evt == 2)
	if evt[2].type == "object-hidden" then
		assert(evt[1].type == "object-shown")
		assert(evt[1].object == obj1)
		assert(evt[2].type == "object-hidden")
		assert(evt[2].object == obj)
	else
		assert(evt[1].type == "object-hidden")
		assert(evt[1].object == obj)
		assert(evt[2].type == "object-shown")
		assert(evt[2].object == obj1)
	end
	-- Garbage collection.
	obj1.realized = nil
	vis:update()
	obj = nil
	obj1 = nil
	collectgarbage()
	for k,v in pairs(vis.objects) do assert(false) end
end

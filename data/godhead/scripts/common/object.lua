local oldinst = Object.new
Object.objects = {}
Object.load_meshes = true
setmetatable(Object.objects, {__mode = "v"})

--- Creates a new object.
-- @param clss Object class.
-- @param args Arguments.<ul>
--   <li>id: Unique object ID.</li></ul>
-- @return New object.
Object.new = function(clss, args)
	-- Create an object with the desired ID.
	local self = oldinst(clss, args)
	self.id = args and args.id or clss:get_free_id()
	-- If there was an existing object with the same ID, hide it. This can
	-- happen when object was hidden and displayed again before it being GC'd.
	local old = Object.objects[self.id]
	if old then old.realized = false end
	Object.objects[self.id] = self
	-- Repeat particle animations by default.
	if self.particle_animation then
		self:particle_animation{loop = true}
	end
	return self
end

--- Gets a free object ID.
-- @param clss Object class.
-- @return Free object ID.
Object.get_free_id = function(clss)
	while true do
		local id = math.random(0x1000000, 0x7FFFFFF)
		if not Object:find{id = id} then
			return id
		end
	end
end

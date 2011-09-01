Target = Class()

--- Cancels targeting.
-- @param self Target class.
Target.cancel = function(self)
	if not self.active then return end
	if Gui then Gui:set_action_text() end
	self.func = nil
	self.active = false
end

--- Picks an object or a tile from the scene based on a ray.
-- @param clss Target class.
-- @param args Arguments.<ul>
--   <li>camera: Camera whose picking ray to use.</li>
--   <li>ignore: Table of objects to ignore, or nil.</li>
--   <li>ray1: Start point of the ray.</li>
--   <li>ray2: End point of the ray.</li></ul>
-- @return Position vector, object, tile vector.
Target.pick_ray = function(clss, args)
	local best_object = nil
	local best_point = nil
	local best_dist = nil
	-- Get the picking ray.
	local r1 = args.ray1
	local r2 = args.ray2
	if args.camera then
		r1,r2 = args.camera:picking_ray{cursor = Program.cursor_position}
	end
	-- Pick from the scene.
	local ret = Physics:cast_ray{collision_mask = Physics.MASK_PICK, src = r1, dst = r2, ignore = args.ignore}
	if not ret then return end
	return ret.point, ret.object, ret.tile
end

--- Finishes targeting by selecting an item from a container.
-- @param self Target class.
-- @param inv Container number.
-- @param slot Slot number.
Target.select_container = function(self, inv, slot)
	if self.active then
		local func = self.func
		self:cancel()
		func("inv", inv, slot)
	end
end

--- Finishes targeting by selecting an equipment slot.
-- @param self Target class.
-- @param slot Slot name.
Target.select_equipment = function(self, slot)
	if self.active then
		local func = self.func
		self:cancel()
		func("equ", 0, slot)
	end
end

--- Finishes targeting by selecting an object from under the cursor.
-- @param self Target class.
Target.select_scene = function(self)
	if self.active then
		local func = self.func
		self:cancel()
		local pos,obj = self:pick_ray{camera = Client.camera}
		if obj then
			func("obj", obj.id)
		else
			func("map", nil, pos)
		end
	end
end

--- Initiates targeting.
-- @param clss Target class.
-- @param func Targeting callback.
Target.start = function(clss, msg, func)
	if Program.cursor_grabbed then
		if clss.target_object then
			func("obj", clss.target_object.id)
		end
	else
		if Gui then Gui:set_action_text(msg) end
		clss.func = func
		clss.active = true
	end
end

require "system/class"

if not Los.program_load_extension("physics") then
	error("loading extension `physics' failed")
end

------------------------------------------------------------------------------

Physics = Class()
Physics.class_name = "Physics"

--- Performs a ray cast test.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>collision_group: Collision group.</li>
--   <li>collision_mask: Collision mask.</li>
--   <li>ignore: Object or table of objects to ignore.</li>
--   <li>src: Start point vector in world space.</li>
--   <li>dst: End point vector in world space.</li></ul>
-- @return Table with point, normal, object and tile. Nil if no collision occurred.
Physics.cast_ray = function(self, args)
	local r = Los.physics_cast_ray{collision_group = args.collision_group,
		collision_mask = args.collision_mask, ignore = ignore,
		src = args.src.handle, dst = args.dst.handle}
	if not r then return end
	r.point = Class.new(Vector, {handle = r.point})
	r.normal = Class.new(Vector, {handle = r.normal})
	if r.object then r.object = __userdata_lookup[r.object] end
	if r.tile then r.tile = Class.new(Vector, {handle = r.tile}) end
	return r
end

--- Performs a sphere cast test.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>collision_group: Collision group.</li>
--   <li>collision_mask: Collision mask.</li>
--   <li>ignore: Object or table of objects to ignore.</li>
--   <li>radius: Radius in world units.</li>
--   <li>src: Start point vector in world space.</li>
--   <li>dst: End point vector in world space.</li></ul>
-- @return Table with point, normal, object and tile. Nil if no collision occurred.
Physics.cast_sphere = function(self, args)
	local r = Los.physics_cast_sphere{collision_group = args.collision_group,
		collision_mask = args.collision_mask, ignore = ignore, radius = args.radius,
		src = args.src.handle, dst = args.dst.handle}
	if not r then return end
	r.point = Class.new(Vector, {handle = r.point})
	r.normal = Class.new(Vector, {handle = r.normal})
	if r.object then r.object = __userdata_lookup[r.object] end
	if r.tile then r.tile = Class.new(Vector, {handle = r.tile}) end
	return r
end

--- Physics simulation toggle.
-- @name Physics.enable_simulation
-- @class table

Physics.class_getters = {
	enable_simulation = function(s) return Los.physics_get_enable_simulation() end}

Physics.class_setters = {
	enable_simulation = function(s, v) Los.physics_set_enable_simulation(v) end}

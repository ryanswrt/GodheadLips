require "system/object"
require "system/physics"

if not Los.program_load_extension("object-physics") then
	error("loading extension `object-physics' failed")
end

--- Makes the object approach a point.
-- @param self Object.
-- @param args Arguments.
--   <li>dist: Distance how close to target to get.</li>
--   <li>point: Point vector in world space. (required)</li>
--   <li>speed: Movement speed multiplier.</li></ul>
Object.approach = function(self, args)
	Los.object_approach(self.handle, {dist = args.dist, point = args.point.handle, speed = args.speec})
end

--- Lets an impulse force affect the object.
-- @param self Object.
-- @param ... Arguments.<ul>
--   <li>1,impulse: Force of impulse.</li>
--   <li>2,point: Point of impulse or nil.</li></ul>
Object.impulse = function(self, ...)
	local a,b = ...
	if a.class then
		Los.object_impulse(self.handle, a.handle, b and b.handle)
	else
		Los.object_impulse(self.handle, a.impulse.handle, a.point and a.point.handle)
	end
end

--- Creates a hinge constraint.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>position: Position vector.</li>
--   <li>axis: Axis of rotation.</li></ul>
Object.insert_hinge_constraint = function(self, args)
	Los.object_insert_hinge_constraint(self.handle, args.position.handle, args.axis and args.axis.handle)
end

--- Causes the object to jump.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>1,impulse: Force of impulse.</li></ul>
Object.jump = function(self, args)
	if args.class then
		Los.object_jump(self.handle, args.handle)
	else
		Los.object_jump(self.handle, args.impulse.handle)
	end
end

--- Activation state of the object.
-- @name Object.activated
-- @class table

--- Angular velocity.<br/>
-- Angular velocity specifies how the object rotates. The direction of the
-- vector points towards the axis of rotation and the length of the vector
-- specifies how fast the object rotates around its center point.<br/>
-- Only supported by rigid bodies. Other kind of objects always return
-- a zero vector.
-- @name Object.angular
-- @class table

--- Collision group bitmask.
-- @name Object.collision_group
-- @class table

--- Collision bitmask.
-- @name Object.collision_mask
-- @class table

--- Contact event generation toggle.
-- @name Object.contact_events
-- @class table

--- Gravity vector.
-- @name Object.gravity
-- @class table

--- Ground contact flag.<br/>
-- Only supported by kinematic objects. Other kind of objects always return false.
-- @name Object.ground
-- @class table

--- Liquid friction coefficient.
-- @name Object.friction_liquid
-- @class table

--- Liquid gravity vector.
-- @name Object.gravity_liquid
-- @class table

--- Mass.
-- @name Object.mass
-- @class table

--- Movement direction.<br/>
-- Only used by kinematic objects. The value of -1 means that the creature is
-- moving forward at walking speed. The value of 1 means backward, and the
-- value of 0 means no strafing.
-- @name Object.movement
-- @class table

--- Physics simulation mode.<br/>
-- Specifies the physics simulation mode of the object. The recognized values are:<ul>
-- <li>"kinematic": Kinematic character.</li>
-- <li>"none": Not included to the simulation.</li>
-- <li>"rigid": Rigid body simulation.</li>
-- <li>"static": Static obstacle.</li>
-- <li>"vehicle": Vehicle physics.</li></ul>

--- Physics shape.<br/>
-- A model can contain multiple physics shapes. By setting the field,
-- you can switch to one of the alternative shapes. This can be used
-- to, for example, set a smaller collision shape when the creature
-- is crouching.
-- @name Object.shape
-- @class table

--- Movement speed.<br/>
-- Only used by kinematic objects.
-- @name Object.speed
-- @class table

--- Strafing direction.<br/>
-- Only used by kinematic objects. The value of -1 means that the creature is
-- strafing to the left at walking speed. The value of 1 means right, and the
-- value of 0 means no strafing.
-- @name Object.strafing
-- @class table

--- Linear velocity.
-- @name Object.velocity
-- @class table

Object:add_getters{
	activated = function(self) return Los.object_get_activated(self.handle) end,
	angular = function(s) return Class.new(Vector, {handle = Los.object_get_angular(s.handle)}) end,
	bounding_box_physics = function(self)
		local h1,h2 = Los.object_get_bounding_box_physics(self.handle)
		local min = Class.new(Vector, {handle = h1})
		local max = Class.new(Vector, {handle = h2})
		return Aabb{point = min, size = max - min}
	end,
	center_offset_physics = function(self)
		local h = Los.object_get_center_offset_physics(self.handle)
		return Class.new(Vector, {handle = h})
	end,
	collision_group = function(s) return Los.object_get_collision_group(s.handle) end,
	collision_mask = function(s) return Los.object_get_collision_mask(s.handle) end,
	contact_events = function(s) return Los.object_get_contact_events(s.handle) end,
	friction_liquid = function(s) Los.object_get_friction_liquid(s.handle) end,
	gravity = function(s) return Class.new(Vector, {handle = Los.object_get_gravity(s.handle)}) end,
	gravity_liquid = function(s) return Class.new(Vector, {handle = Los.object_get_gravity_liquid(s.handle)}) end,
	ground = function(s) return Los.object_get_ground(s.handle) end,
	mass = function(s) return Los.object_get_mass(s.handle) end,
	movement = function(s) return Los.object_get_movement(s.handle) end,
	physics = function(s) return Los.object_get_physics(s.handle) end,
	shape = function(s) return Los.object_get_shape(s.handle) end,
	speed = function(s) return Los.object_get_speed(s.handle) end,
	strafing = function(s) return Los.object_get_strafing(s.handle) end,
	velocity = function(s) return Class.new(Vector, {handle = Los.object_get_velocity(s.handle)}) end}

Object:add_setters{
	activated = function(self, v) return Los.object_set_activated(self.handle, v) end,
	angular = function(s, v) Los.object_set_angular(s.handle, v.handle) end,
	collision_group = function(s, v) Los.object_set_collision_group(s.handle, v) end,
	collision_mask = function(s, v) Los.object_set_collision_mask(s.handle, v) end,
	contact_events = function(s, v) Los.object_set_contact_events(s.handle, v) end,
	friction_liquid = function(s, v) Los.object_set_friction_liquid(s.handle, v) end,
	gravity = function(s, v)
		if not v then return end -- FIXME: Shouldn't be called with nil
		Los.object_set_gravity(s.handle, v.handle)
	end,
	gravity_liquid = function(s, v)
		if not v then return end -- FIXME: Shouldn't be called with nil
		Los.object_set_gravity_liquid(s.handle, v.handle)
	end,
	ground = function(s, v) Los.object_set_ground(s.handle, v) end,
	mass = function(s, v) Los.object_set_mass(s.handle, v) end,
	movement = function(s, v) Los.object_set_movement(s.handle, v) end,
	physics = function(s, v) Los.object_set_physics(s.handle, v) end,
	shape = function(s, v) Los.object_set_shape(s.handle, v) end,
	speed = function(s, v) Los.object_set_speed(s.handle, v) end,
	strafing = function(s, v) Los.object_set_strafing(s.handle, v) end,
	velocity = function(s, v) Los.object_set_velocity(s.handle, v.handle) end}

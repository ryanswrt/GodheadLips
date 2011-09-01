local radian_wrap = function(x)
	if x < -math.pi then return x + 2 * math.pi
	elseif x > math.pi then return x - 2 * math.pi
	else return x end
end

EditorCamera = Class(Camera)

--- Creates a new first person camera.
-- @param clss First person camera class.
-- @param args Arguments.
-- @return First person camera.
EditorCamera.new = function(clss, args)
	local self = Camera.new(clss, args)
	self.collision_mask = Physics.MASK_CAMERA
	self.far = 40.0
	self.fov = 1.1
	self.mode = "first-person"
	self.near = 0.01
	self.node_pos = Vector()
	self.node_rot = Quaternion()
	self.position_smoothing = 0.15
	self.rotation_smoothing = 0.15
	self.own_pos = Vector()
	self.own_ray = Vector()
	self.own_rot = Quaternion()
	self.own_tilt = 0
	self.own_turn = 0
	self.own_vel = Vector()
	return self
end

--- Gets the picking ray of the camera.
-- @param self First person camera.
-- @return Picking ray.
EditorCamera.get_picking_ray = function(self)
	-- Calculate the rotation.
	local euler = self.own_rot.euler
	euler[3] = euler[3] - self.own_tilt
	local rot = Quaternion{euler = euler}
	-- Project the ray.
	local ctr = self.own_pos + self.own_rot * self.own_ray
	return ctr,ctr + rot * Vector(0, 0, -5)
end

EditorCamera.get_velocity = function(self)
	return self.own_vel
end

EditorCamera.rotate = function(self, turn, tilt)
	self.own_turn = radian_wrap(self.own_turn + turn)
	self.own_tilt = radian_wrap(self.own_tilt - tilt)
	self.own_rot = Quaternion{euler = {self.own_turn, 0, self.own_tilt}}
end

EditorCamera.set_node_transform = function(self, pos, rot)
	self.node_pos = pos
	self.node_rot = rot
end

EditorCamera.set_velocity = function(self, v)
	self.own_vel = v
end

EditorCamera.translate = function(self, rel)
	self.own_pos = self.own_pos + self.own_rot * rel
end

--- Updates the transformation of the camera.
-- @param self First person camera.
-- @param secs Seconds since the last update.
EditorCamera.update = function(self, secs)
	-- Update position.
	local vel = self.own_vel
	if self.lifting then vel = vel + Vector(0, self.lifting, 0) end
	if self.movement then vel = vel + self.own_rot * Vector(0, 0, -self.movement) end
	if self.strafing then vel = vel + self.own_rot * Vector(self.strafing, 0, 0) end
	self.own_pos = self.own_pos + vel * secs
	-- Update turning and tilting.
	-- If ctrl is pressed, add mouse movement to rotation.
	-- If not, interpolate towards the default rotation.
	if false then--not Action or Action.dict_press[Keysym.LCTRL] then
		self.own_turn = self.own_turn * math.max(1 - 3 * secs, 0)
		self.own_tilt = self.own_tilt * math.max(1 - 3 * secs, 0)
	end
	-- Calculate the target transformation.
	local pos = self.own_pos + self.own_rot * self.node_pos
	local rot = self.own_rot * self.node_rot
	-- Apply the quake effect.
	if self.quake then
		local rnd = Vector(2*math.random()-1, 2*math.random()-1, 2*math.random()-1)
		pos = pos + rnd * 6 * math.min(1, self.quake)
		self.quake = self.quake - secs
		if self.quake < 0 then
			self.quake = 0
		end
	end
	-- Update the transformation.
	self.target_position = pos
	self.target_rotation = rot
	Camera.update(self, secs)
end

EditorCamera.warp = function(self, pos, rot)
	self.own_pos = pos
	self.own_rot = rot
	Camera.warp(self)
end

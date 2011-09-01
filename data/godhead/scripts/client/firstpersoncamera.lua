local radian_wrap = function(x)
	if x < -math.pi then return x + 2 * math.pi
	elseif x > math.pi then return x - 2 * math.pi
	else return x end
end

FirstPersonCamera = Class(Camera)

--- Creates a new first person camera.
-- @param clss First person camera class.
-- @param args Arguments.
-- @return First person camera.
FirstPersonCamera.new = function(clss, args)
	local self = Camera.new(clss, args)
	self.collision_mask = Physics.MASK_CAMERA
	self.mode = "first-person"
	self.tilt_speed = 0
	self.tilt_state = 0
	self.turn_speed = 0
	self.turn_state = 0
	return self
end

FirstPersonCamera.get_picking_ray = function(self)
	if not self.object then return end
	local spec = self.object.spec
	-- Calculate the rotation.
	local euler = self.object.rotation.euler
	euler[3] = self.object.tilt - self.tilt_state
	local rot = Quaternion{euler = euler}
	-- Project the ray.
	local ctr = self.object.position + self.object.rotation * spec.aim_ray_center
	return ctr,ctr + rot * Vector(0, 0, -5)
end

FirstPersonCamera.get_transform = function(self)
	-- Calculate the rotation.
	local turn = self.turn_state + self.object.rotation.euler[1]
	local tilt = self.tilt_state + (self.object.tilt or 0)
	local rot = Quaternion{euler = {turn, 0, tilt}}
	-- Find the camera offset.
	local spec = self.object.spec
	local rel = spec.camera_center or Vector(0, 2, 0)
	local pos = self.object.position + self.object.rotation * rel
	local npos,nrot = self.object:find_node{name = "#camera"}
	if npos then
		-- The position of the camera node is always used but the rotation is
		-- ignored most of the time since the rotation component of the node
		-- is highly annoying in many animations. However, the rotation is used
		-- when the player dies to avoid the camera facing inside the corpse.
		pos = self.object.position + self.object.rotation * npos
		if self.object.dead then
			rot = rot * nrot
		end
	end
	return pos,rot
end

FirstPersonCamera.update = function(self, secs)
	if Action.dict_press[Keysym.LCTRL] then
		-- Update turning.
		self.turn_state = self.turn_state + self.turn_speed * secs
		self.turn_state = radian_wrap(self.turn_state)
		self.turn_speed = 0
		-- Update tilting.
		self.tilt_state = self.tilt_state - self.tilt_speed * secs
		self.tilt_state = radian_wrap(self.tilt_state)
		self.tilt_speed = 0
	else
		-- Reset mouse look.
		self.turn_state = self.turn_state * math.max(1 - 3 * secs, 0)
		self.tilt_state = self.tilt_state * math.max(1 - 3 * secs, 0)
	end
	-- Set the target transformation.
	local pos,rot = self:get_transform()
	if self.quake then
		local rnd = Vector(2*math.random()-1, 2*math.random()-1, 2*math.random()-1)
		pos = pos + rnd * 6 * math.min(1, self.quake)
		self.quake = self.quake - secs
		if self.quake < 0 then
			self.quake = 0
		end
	end
	self.target_position = pos
	self.target_rotation = rot
	-- Interpolate.
	self.viewport = {Gui.scene.x, Gui.scene.y, Gui.scene.width, Gui.scene.height}
	Camera.update(self, secs)
end

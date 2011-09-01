require "client/objects/speedline"

Object.ipol_correction = 0.8
Object.ipol_max_error = 20
Object.dict_active = setmetatable({}, {__mode = "k"})
Object.physics_position_correction = Vector(0, 0, 0)

Object.animate_spec = function(self, name)
	return self:set_anim(name)
end

Object.detach = function(self)
	-- Hide special effects.
	if self.special_effects then
		for k,v in pairs(self.special_effects) do
			if v.enabled then
				v.enabled = false
			else
				v.realized = false
			end
		end
		self.special_effects = nil
	end
	-- Hide speed lines.
	if self.speedline then
		self.speedline.realized = false
		self.speedline = nil
	end
	-- Hide self.
	self.realized = false
	Object.dict_active[self] = nil
end

Object.set_anim = function(self, name, time)
	-- Play the animation.
	local args = {animation = name, fade_in = 0.3, fade_out = 0.3, time = t}
	local anim = self.spec and self.spec.animations[name]
	if anim then
		for k,v in pairs(anim) do args[k] = v end
	end
	self:animate(args)
	-- Mark as active.
	self.animated = true
	Object.dict_active[self] = 5.0
	return args
end

Object.set_dialog = function(self, type, args)
	-- Update the dialog state.
	if type == "choice" then
		self.dialog = {type = type, choices = args}
	elseif type == "message" then
		self.dialog = {type = type, character = args.character, message = args.message}
	else
		self.dialog = nil
	end
	-- Update the dialog UI.
	if Gui.active_dialog == self.id then
		Gui:set_dialog(self.id)
	end
end

Object.set_skill = function(self, s, v, m)
end

Object.set_slot = function(self, slot, spec, count)
end

Object.update = function(self, secs)
	-- Update special effects.
	if self.realized then
		if self.special_effects then
			for k,v in pairs(self.special_effects) do
				local p = self.position
				local r = self.rotation
				if v.node then
					local np,nr = self:find_node{name = v.node}
					if np then
						p = p + r * np
						r = r * nr
					end
				end
				if v.offset then p = p + v.offset end
				if v.rotate then v.rotation = r end
				v.position = p
			end
		end
	end
	-- Update speed lines.
	if self.speedline then
		self.speedline:update(secs)
	end
end

--- Sets the model and special effects of the object from its spec.
-- @param self Object.
-- @param skip True to only set the special effects.
Object.set_model = function(self, skip)
	if not self.spec then return end
	-- Set the new model.
	if not skip then
		local shape = self.shape
		self.model = Model:find_or_load{file = self.spec.model}
		self.shape = shape
	end
	-- Detach old special effects.
	if self.special_effects then
		for k,v in pairs(self.special_effects) do v:detach() end
		self.special_effects = nil
	end
	self.special_effects = nil
	-- Create new special effects.
	if self.spec.special_effects and #self.spec.special_effects then
		self.special_effects = {}
		for k,v in pairs(self.spec.special_effects) do
			if v.type == "light" then
				local fx = Light{ambient = v.ambient, diffuse = v.diffuse, node = v.node,
					equation = v.equation, position = self.position, enabled = true}
				table.insert(self.special_effects, fx)
			else
				local fx = Object{model = v.model, node = v.node, position = self.position, realized = true}
				table.insert(self.special_effects, fx)
			end
		end
	end
end

--- Rebuilds the character model of the object.
-- @param self Object.
Object.update_model = function(self)
end

--- Plays footstep sounds for creatures.
-- @param self Object.
-- @param secs Seconds since the last update.
Object.update_sound = function(self, secs)
end

--- Updates the rotation and tilt of the object.
-- @param self Object.
-- @param quat Rotation quaternion.
-- @param tilt Tilt angle in radians.
Object.update_rotation = function(self, quat, tilt)
	self.rotation = quat
	self.tilt = tilt
end

Object.update_motion_state = function(self, tick)
	if not self.interpolation then return end
	if self.interpolation_linear then
		-- Linear interpolation when the object is stopping.
		local t = 10 * tick
		if self.interpolation < 1 - t then
			self.position = self.position + self.correction * t
			self.interpolation = self.interpolation + t
		elseif self.interpolation < 1 then
			self.position = self.position + self.correction * (1 - self.interpolation)
			self.correction = Vector()
			self.interpolation = 1
		end
	else
		-- Damp velocity to reduce overshoots.
		self.interpolation = self.interpolation + tick
		if self.interpolation > 0.3 then
			self.velocity = self.velocity * 0.93
		end
		-- Apply position change predicted by the velocity.
		self.position = self.position + self.velocity * tick
		-- Correct prediction errors over time.
		self.position = self.position + self.correction * (1 - Object.ipol_correction)
		self.correction = self.correction * Object.ipol_correction
	end
end

Object.set_motion_state = function(self, pos, rot, vel, tilt)
	-- Store the prediction error so that it can be corrected over time.
	if (pos - self.position).length < Object.ipol_max_error then
		self.correction = pos - self.position
	else
		self.position = pos
		self.correction = Vector()
	end
	-- Store the current velocity so that we can predict future movements.
	self.velocity = vel
	self.interpolation = 0
	self.interpolation_linear = (vel.length < 0.5)
	-- Set rotation unless controlled by the local player.
	if self ~= Player.object then
		self:update_rotation(rot, tilt)
	end
	-- Mark as active.
	Object.dict_active[self] = 5.0
end

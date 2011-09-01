Creature = Class(Object)

Creature.new = function(clss, args)
	local self = Object.new(clss, args)
	Object.dict_active[self] = 1.0
	return self
end

Creature.detach = function(self)
	-- Hide equipment.
	if self.slots then
		for k,v in pairs(self.slots.slots) do
			v:detach()
		end
		self.slots = nil
	end
	-- Call base.
	Object.detach(self)
end

Creature.set_model = function(self, model)
	if model or not self.spec.models then
		-- Use the already built model.
		local m = model or Model:find_or_load{file = self.spec.model}
		if not m then return end
		if m and m.name and Program.opengl_version < 3.2 then
			m = m:copy()
			m:changed()
		end
		self.model = m
		Object.set_model(self, true)
		-- Apply body scale.
		if self.spec.body_scale then
			local factor = self.body_scale or 0.5
			local scale = self.spec.body_scale[1] * (1 - factor) + self.spec.body_scale[2] * factor
			self:animate{animation = "empty", channel = Animation.CHANNEL_CUSTOMIZE,
				weight = 0, weight_scale = 1000, fade_in = 0, fade_out = 0, permanent = true}
			self:edit_pose{channel = Animation.CHANNEL_CUSTOMIZE, node = "mover", scale = scale}
		end
		-- Create the tilting channel.
		if self.spec.models or self.spec.tilt_bone then
			self:animate{animation = "empty", channel = Animation.CHANNEL_TILT,
				additive = true, weight = 1, permanent = true}
		end
		-- Initialize the pose.
		self.animated = true
		self:update_animations{secs = 0}
	else
		-- Order a new model to be built.
		-- The model is instantly replaced only if there's no existing model.
		-- The base model only contains the armature and animations so we'd
		-- rather keep the existing build result until the new model has been built.
		Object.set_model(self, self.model)
		self:update_model()
	end
end

Creature.set_skill = function(self, s, v, m)
	-- Update player skills.
	if self == Player.object then
		Client.views.skills:update(s, v, m)
	end
	-- Display health changes.
	if s == "health" then
		if self.health then
			-- Show a health change text.
			local diff = v - self.health
			if math.abs(diff) > 2 then
				local code = (diff > 0 and 0x01 or 0x00) + (self == Player.object and 0x10 or 0x00)
				local colors = {
					[0x00] = {1,1,0,1},
					[0x01] = {0,1,1,1},
					[0x10] = {1,0,0,1},
					[0x11] = {0,1,0,1}}
				EffectObject{
					life = 3,
					object = self,
					position = Vector(0,2,0),
					realized = true,
					text = tostring(diff),
					text_color = colors[code],
					text_fade_time = 1,
					text_font = "medium",
					velocity = Vector(0,0.5,0)}
			end
			-- Quake the camera if the player was hurt.
			if self == Player.object and diff < -5 then
				Client:apply_quake(self.position, 0.01 * (5 - diff))
			end
		end
		self.health = v
		-- Set the correct collision shape.
		-- Dead creatures have a different collision shape. We switch between
		-- the two when the health changes between zero and non-zero.
		if self.health == 0 and self.animated then
			self.shape = "dead"
		else
			self.shape = "default"
		end
	end
end

Creature.set_slot = function(self, slot, spec, count)
	if not self.slots then self.slots = Slots() end
	local slots = self.slots
	spec = Itemspec:find{name = spec}
	-- Update the model.
	if not spec then
		-- Missing spec.
		slots:set_object{slot = slot}
		if self.equipment and self.equipment[slot] then
			self.equipment[slot] = nil
			if self.realized then self:update_model() end
		end
	elseif spec.equipment_models then
		-- Replacer equipment.
		slots:set_object{slot = slot}
		self.equipment = self.equipment or {}
		self.equipment[slot] = spec.name
		if self.realized then self:update_model() end
	else
		-- Add-on equipment.
		slots:set_object{slot = slot, model = spec.model, spec = spec}
		self.equipment = self.equipment or {}
		self.equipment[slot] = spec.name
		if self.realized then self:update_model() end
	end
	-- Equip animations.
	local a
	if spec then
		a = self:animate_spec(spec.animation_hold)
	end
	if self.equipment_animations then
		if not a and self.equipment_animations[slot] then
			self:animate{channel = self.equipment_animations[slot].channel}
		end
		self.equipment_animations[slot] = a
	else
		self.equipment_animations = {[slot] = a}
	end
end

Creature.update = function(self, secs)
	-- Update slots.
	if self.slots then
		local species = self.spec
		for name,object in pairs(self.slots.slots) do
			local slot = species and species.equipment_slots[name]
			if slot and slot.node and self.realized then
				-- Show slot.
				local p,r = self:find_node{name = slot.node, space = "world"}
				if p then
					local h = object:find_node{name = "#handle"}
					if h then p = p - r * h end
					object.position = p
					object.rotation = r
					object.realized = true
					object:update(secs)
				else
					object:detach()
				end
			else
				-- Hide slot.
				object:detach()
			end
		end
	end
	-- Call the base class.
	Object.update(self, secs)
end

Creature.update_model = function(self)
	if not self.spec then return end
	if not self.spec.models then return end
	if not Model.morph then return end
	-- Build the character model in a separate thread.
	-- The result is handled in the tick handler in event.lua.
	Client.threads.model_builder:push_message(tostring(self.id), serialize{
		beheaded = Bitwise:bchk(self.flags or 0, Protocol.object_flags.BEHEADED),
		body_scale = self.body_scale,
		body_style = self.body_style,
		equipment = self.equipment,
		eye_color = self.eye_color,
		eye_style = self.eye_style,
		face_style = self.face_style,
		hair_color = self.hair_color,
		hair_style = self.hair_style,
		nudity = Client.views.options.nudity_enabled,
		species = self.spec.name,
		skin_color = self.skin_color,
		skin_style = self.skin_style})
end

--- Updates the rotation and tilt of the object.
-- @param self Object.
-- @param quat Rotation quaternion.
-- @param tilt Tilt angle in radians.
Creature.update_rotation = function(self, quat, tilt)
	Object.update_rotation(self, quat, tilt)
	local spec = self.spec
	if spec and spec.tilt_bone then
		local nodes = spec.tilt_bone
		if type(nodes) ~= "table" then nodes = {nodes} end
		local angle = self.dead and 0 or -tilt
		local rot = Quaternion{axis = Vector(1,0,0), angle = angle / #nodes}
		for k,v in pairs(nodes) do
			self:edit_pose{channel = Animation.CHANNEL_TILT, node = v, rotation = rot}
		end
	end
end

--- Plays footstep sounds for creatures.
-- @param self Object.
-- @param secs Seconds since the last update.
Creature.update_sound = function(self, secs)
	-- Check for an applicable species.
	local spec = self.spec
	if not spec or not spec.footstep_sound then return end
	-- Check for an applicable animation.
	local anim = self:get_animation{channel = 1}
	if not anim then return end
	if anim.animation == "dead" or anim.animation == "idle" then return end
	-- Find the foot anchors.
	-- These are needed for foot position tracking so that we know when and
	-- where to play the positional sound.
	local lnode = self:find_node{name = "#foot.L", space = "world"}
	if not lnode then return end
	local rnode = self:find_node{name = "#foot.R", space = "world"}
	if not rnode then return end
	-- Ground check.
	-- We don't want to play footsteps if the character is flying.
	if not Physics:cast_ray{src = lnode, dst = lnode - Vector(0,spec.footstep_height)} and
	   not Physics:cast_ray{src = rnode, dst = rnode - Vector(0,spec.footstep_height)} then
		self.lfoot_prev = nil
		self.rfoot_prev = nil
		return
	end
	-- Left foot.
	-- We play the sound when the node crosses from the local positive Z axis
	-- to the negative. Using the Y distance to the ground is too error prone
	-- so this approximation is the best approach I have found so far.
	self.lfoot_prev = self.lfoot_curr
	self.lfoot_curr = self:find_node{name = "#foot.L"}
	if self.lfoot_prev and self.lfoot_curr.z < 0 and self.lfoot_prev.z >= 0 then
		Effect:play_world(spec.footstep_sound, lnode)
	end
	-- Right foot.
	-- Works the same way with the left foot.
	self.rfoot_prev = self.rfoot_curr
	self.rfoot_curr = self:find_node{name = "#foot.R"}
	if self.rfoot_prev and self.rfoot_curr.z < 0 and self.rfoot_prev.z >= 0 then
		Effect:play_world(spec.footstep_sound, rnode)
	end
	return true
end

--- Writes the appearance preset of the object to a string.
-- @param self Object.
-- @return String.
Creature.write_preset = function(self)
	return serialize{
		body_scale = self.body_scale,
		body_style = self.body_style,
		eye_color = self.eye_color,
		eye_style = self.eye_style,
		face_style = self.face_style,
		hair_color = self.hair_color,
		hair_style = self.hair_style,
		skin_color = self.skin_color,
		skin_style = self.skin_style}
end

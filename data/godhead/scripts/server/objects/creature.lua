require "server/ai"
require "server/objects/object"

Creature = Class(Object)
Creature.class_name = "Creature"
Creature.dict_id = setmetatable({}, {__mode = "kv"})

Creature:add_getters{
	armor_class = function(s)
		local value = 0
		for k,v in pairs(s.spec.equipment_slots) do
			local i = s:get_item{slot = v.name}
			value = value + (i and i.spec.armor_class or 0)
		end
		return value
	end,
	beheaded = function(s)
		return Bitwise:band(s.flags or 0, Protocol.object_flags.BEHEADED) ~= 0
	end}

Creature:add_setters{
	beheaded = function(s, v)
		s.flags = Bitwise:bor(s.flags or 0, Protocol.object_flags.BEHEADED)
		Vision:event{type = "object-beheaded", object = s}
		local hat = s:get_item{slot = "head"}
		if hat then
			local p = s.position
			hat:detach()
			hat.position = p + s.rotation * (s.dead and Vector(0,0.5,2) or Vector(0,2,0))
			hat.velocity = Vector(math.random(), math.random(), math.random())
			hat.realized = true
		end
	end,
	spec = function(s, v)
		local spec = type(v) == "string" and Species:find{name = v} or v
		if s.spec == spec then return end
		rawset(s, "__spec", spec)
		s.model = spec.model
		s.mass = spec.mass
		s.friction_liquid = spec.water_friction
		s.gravity = spec.gravity
		s.gravity_liquid = spec.water_gravity
		-- Set appearance.
		-- Only set once when spawned by the map generator or an admin.
		if s.random and spec.eye_style then
			if spec.eye_style == "random" then
				local s = spec.hair_styles[math.random(1, #spec.eye_styles)]
				local rgb = Color:hsv_to_rgb{math.random(), 0.2 + 0.8 * math.random(), math.random()}
				rgb[1] = math.floor(255 * rgb[1] + 0.5)
				rgb[2] = math.floor(255 * rgb[1] + 0.5)
				rgb[3] = math.floor(255 * rgb[1] + 0.5)
				table.insert(rgb, 1, s[2])
				s.eye_style = rgb
			else
				s.eye_style = spec.eye_style
			end
		end
		if s.random and spec.hair_style then
			if spec.hair_style == "random" then
				local h = spec.hair_styles[math.random(1, #spec.hair_styles)]
				local r = math.random(0, 255)
				local g = math.random(0, 255)
				local b = math.random(0, 255)
				s.hair_style = {h[2], r, g, b}
			else
				s.hair_style = spec.hair_style
			end
		end
		-- Create skills.
		s.skills = s.skills or Skills{id = s.id}
		s.skills:clear()
		for k,v in pairs(spec.skills) do
			local prot = v.name == "health" and "public" or "private"
			s.skills:register{
				prot = prot,
				name = v.name,
				maximum = v.val,
				value = v.val,
				regen = spec.skill_regen}
		end
		-- Create inventory.
		-- When the map generator or an admin command creates an object, the
		-- random field is set to indicate that items should be generated.
		-- The field isn't saved so items are only created once as expected.
		s.inventory = s.inventory or Inventory{id = s.id, size = spec.inventory_size} -- FIXME: Resizing not supported.
		if s.random then
			for k,v in pairs(spec.inventory_items) do
				local itemspec = Itemspec:find{name = k}
				if itemspec then
					if itemspec.stacking then
						s:add_item{object = Item{spec = itemspec, count = v}}
					else
						for i = 1,v do s:add_item{object = Item{spec = itemspec}} end
					end
				else
					print(string.format("WARNING: Creature '%s' uses an invalid inventory item name '%s'", s.spec.name, k))
				end
			end
			s:equip_best_items()
		end
		-- Create random loot.
		-- The same about random objects applies as above.
		if s.random and spec.loot_categories then
			local num_cat = #spec.loot_categories
			local num_item
			if spec.loot_count then
				num_item = math.random(spec.loot_count[1], spec.loot_count[2])
			else
				num_item = math.random(0, s.inventory.size)
			end
			for i = 1,num_item do
				local cat = spec.loot_categories[math.random(1, num_cat)]
				local itemspec = Itemspec:random{category = cat}
				if itemspec then
					s:add_item{object = Item{spec = itemspec}}
				else
					print(string.format("WARNING: Creature '%s' uses an invalid inventory item category '%s'", s.spec.name, v))
				end
			end
		end
		-- Create the map marker.
		-- Usually the marker exists already but we support creating new markers on
		-- the fly in case that'll be needed in the future.
		if spec.marker then
			s.marker = Marker:find{name = spec.marker}
			if s.marker then
				s.marker.position = s.position
				s.marker.target = s.id
			else
				s.marker = Marker{name = spec.marker, position = s.position, target = s.id}
			end
		end
		-- Kill dead quest characters.
		if spec.dead then
			s.dead = true
		end
		-- Create the AI.
		if not spec.dead and spec.ai_enabled then
			s.ai = Ai(s)
		end
	end}

--- Creates a new creature.
-- @param clss Creature class.
-- @param args Arguments.<ul>
--   <li>angular: Angular velocity.</li>
--   <li>beheaded: True to spawn without a head.</li>
--   <li>body_scale: Scale factor of the body.</li>
--   <li>body_style: Body style defined by an array of scalars.</li>
--   <li>dead: True for a dead creature.</li>
--   <li>eye_style: Eye style defined by an array of {style, red, green, blue}.</li>
--   <li>hair_style: Hair style defined by an array of {style, red, green, blue}.</li>
--   <li>id: Unique object ID or nil for a random free one.</li>
--   <li>jumped: Jump timer.</li>
--   <li>name: Name of the creature.</li>
--   <li>physics: Physics mode.</li>
--   <li>position: Position vector of the creature.</li>
--   <li>rotation: Rotation quaternion of the creature.</li>
--   <li>skin_style: Skin style defined by an array of {style, red, green, blue}.</li>
--   <li>spec: Species of the creature.</li>
--   <li>realized: True to add the object to the simulation.</li></ul>
Creature.new = function(clss, args)
	local self = Object.new(clss, {id = args.id})
	local copy = function(n, d)
		if args[n] ~= nil or d then
			self[n] = (args[n] ~= nil) and args[n] or d
		end
	end
	copy("angular")
	copy("beheaded")
	copy("body_scale")
	copy("body_style")
	copy("eye_style")
	copy("face_style")
	copy("hair_style")
	copy("jumped", 0)
	copy("name")
	copy("physics", "kinematic")
	copy("random")
	copy("rotation")
	copy("position")
	copy("skin_style")
	copy("carried_weight", 0)
	copy("spec")
	copy("variables")
	clss.dict_id[self.id] = self
	self.update_timer = 0.1 * math.random()
	self:calculate_speed()
	if args and args.dead then self:set_dead_state() end
	copy("realized")
	return self
end

--- Adds an object to the list of known enemies.<br/>
-- This function skips faction checks and adds the object directly to the
-- list. Hence, calling this temporarily makes the creature angry at the
-- passed object.
-- @param self Object.
-- @param object Object to add to the list of enemies.
Creature.add_enemy = function(self, object)
	if not self.ai then return end
	if object.god then return end
	local enemy = self.ai.enemies[object]
	if enemy then
		enemy[2] = Program.time + 30
	else
		self.ai.enemies[object] = {object, Program.time + 30}
	end
end

--- Calculates the animation state based on the active controls.
-- @param self Object.
Creature.calculate_animation = function(self)
	local anim
	local back = self.movement < 0
	local front = self.movement > 0
	local left = self.strafing < 0
	local right = self.strafing > 0
	local run = self.running
	-- Select the animation.
	if back then self:animate("walk back")
	elseif front and right then self:animate("run right")
	elseif front and left then self:animate("run left")
	elseif front and run and not self.blocking then self:animate("run")
	elseif front then self:animate("walk")
	elseif left then self:animate("strafe left")
	elseif right then self:animate("strafe right")
	else self:animate("idle") end
end

--- Calculates the movement speed of the creature.
-- @param self Object.
Creature.calculate_speed = function(self)
	-- Base speed.
	local s = (self.running and not self.blocking) and self.spec.speed_run or self.spec.speed_walk
	-- Skill bonuses.
	local str = self.skills:get_value{skill = "strength"} or 20
	local dex = self.skills:get_value{skill = "dexterity"} or 20
	s = s * (0.5 + dex / 100 + str / 200)
	-- Burdening penalty.
	if self:get_burdened() then
		s = math.max(1, s * 0.3)
	end
	-- Update speed.
	if s ~= self.speed then
		self.speed = s
		self:calculate_animation()
	end
end

--- Checks if the creature could climb over a low wall.
-- @param self Creature.
-- @return True if could climb.
Creature.can_climb_low = function(self)
	if self.movement < 0 then return end
	local ctr = self.position * Voxel.tile_scale + Vector(0,0.5,0)
	local dir = self.rotation * Vector(0,0,-1)
	local dst = (ctr + dir):floor()
	local f1 = Voxel:get_tile(dst)
	local f2 = Voxel:get_tile(dst + Vector(0,1,0))
	local f3 = Voxel:get_tile(dst + Vector(0,2,0))
	return f1 ~= 0 and f2 == 0 and f3 == 0
end

--- Checks if the creature could climb over a high wall.
-- @param self Creature.
-- @return True if could climb.
Creature.can_climb_high = function(self)
	if self.movement < 0 then return end
	local ctr = self.position * Voxel.tile_scale + Vector(0,0.5,0)
	local dir = self.rotation * Vector(0,0,-1)
	local dst = (ctr + dir):floor()
	local f1 = Voxel:get_tile(dst + Vector(0,1,0))
	local f2 = Voxel:get_tile(dst + Vector(0,2,0))
	local f3 = Voxel:get_tile(dst + Vector(0,3,0))
	return f1 ~= 0 and f2 == 0 and f3 == 0
end

--- Checks line of sight to the target point or object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>object: Object being looked for.</li>
--   <li>point: Point being looked for.</li></ul>
-- @return True if seen.
Creature.check_line_of_sight = function(self, args)
	-- TODO: Take stealth into account.
	local src
	local dst
	-- Get the vision ray.
	-- TODO: Take bounding box into account.
	if args.point then
		src = self.position + Vector(0,1,0)
		dst = args.point
	elseif args.object then
		src = self.position + Vector(0,1,0)
		dst = args.object.position + Vector(0,1,0)
	end
	-- Check for view cone.
	local ray = (src - dst):normalize()
	local look = self.rotation * Vector(0,0,-1)
	if math.acos(ray:dot(look)) > self.spec.view_cone then
		return
	end
	-- Check for ray cast success.
	-- TODO: Shoot multiple rays?
	if args.point then
		local ret = Physics:cast_ray{src = src, dst = dst, ignore = self}
		return not ret or (ret.point - dst).length < 0.5
	elseif args.object then
		local ret = Physics:cast_ray{src = src, dst = dst, ignore = self}
		if not ret or ret.object == args.object then return true end
	end
end

--- Checks if the given object is an enemy of the creature.
-- @param self Object.
-- @param object Object.
-- @return True if the object is an enemy.
Creature.check_enemy = function(self, object)
	if object == self then return end
	if object.dead then return end
	if object.god then return end
	return self.spec:check_enemy(object)
end

--- Causes the object to take damage.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>amount: Amount of damage.</li>
--   <li>point: Damage point.</li>
--   <li>type: Damage type.</li></ul>
Creature.damaged = function(self, args)
	-- Check for invulnerability.
	if not self.realized then return end
	if self.god then return end
	local health = self.skills:get_value{skill = "health"}
	if not health then return end
	-- Reduce health.
	if args.amount < 0 then
		local max = self.skills:get_maximum{skill = "health"}
		self.skills:set_value{skill = "health", value = math.min(health - args.amount, max)}
	elseif health - args.amount > 0 then
		self.skills:set_value{skill = "health", value = health - args.amount}
	else
		-- Behead if the blow was strong.
		-- Players can't be beheaded with first hit.
		if not self.beheaded then
			if self.dead then
				if args.amount > 15 then self.beheaded = true end
			elseif not self.client then
				if args.amount > 30 then self.beheaded = true end
			end
		end
		-- Kill if not killed already.
		self.skills:set_value{skill = "health", value = 0}
		self:die()
	end
	-- Play the damage effect.
	-- TODO: Should depend on the attack type.
	-- TODO: Should depend on the damage magnitude.
	if args.type == "physical" and self.spec.effect_physical_damage and args.amount > 0 then
		Effect:play{effect = self.spec.effect_physical_damage, object = not args.point and self, point = args.point}
	end
end

Creature.get_weapon = function(self)
	return self:get_item{slot = self.spec.weapon_slot}
end

Creature.set_weapon = function(self, value)
	return self:set_item{slot = self.spec.weapon_slot, object = value}
end

Creature.set_dead_state = function(self, drop)
	-- Playback animation.
	-- This needs to be done first because setting the 'dead' member will
	-- prevent any future animation changes.
	self:animate("death")
	-- Disable controls.
	self.dead = true
	self.physics = "rigid"
	self.shape = "dead"
	self:set_movement(0)
	self:set_strafing(0)
	-- Disable skills.
	self.skills.enabled = false
	self.skills:set_value{skill = "health", value = 0}
	-- Drop held items.
	if drop then
		local o = self:get_item{slot = "hand.L"}
		if o then
			o:detach()
			o.position = self.position
			o.realized = true
		end
		o = self:get_weapon()
		if o then
			o:detach()
			o.position = self.position
			o.realized = true
		end
	end
	-- Emit a vision event.
	Vision:event{type = "object-dead", object = self, dead = true}
end

Creature.climb = function(self)
	if self.blocking then return end
	if self.climbing then return end
	if self:can_climb_high() then
		self.jumping = nil
		self.climbing = true
		self:animate("climb high")
		Coroutine(function()
			-- Rise.
			local t = 0
			local p = self.position
			local r = self.rotation
			repeat
				local d = coroutine.yield()
				t = t + d
				self.position = p + Vector(0,2*t,0)
				self.velocity = Vector()
			until t > 0.8 * Voxel.tile_size
			-- Slide.
			t = 0
			p = self.position
			repeat
				local d = coroutine.yield()
				t = t + d
				self.position = p + r * Vector(0,0.3,-0.8) * t
			until t > 1
			self.climbing = nil
		end)
	elseif self:can_climb_low() then
		self.jumping = nil
		self.climbing = true
		self:animate("climb low")
		Coroutine(function()
			-- Rise.
			local t = 0
			local p = self.position
			local r = self.rotation
			repeat
				local d = coroutine.yield()
				t = t + d
				self.position = p + Vector(0,4*t,0)
				self.velocity = Vector()
			until t > 0.2 * Voxel.tile_size
			-- Slide.
			t = 0
			p = self.position
			repeat
				local d = coroutine.yield()
				t = t + d
				self.position = p + r * Vector(0,0.3,-1) * 2 * t
			until t > 0.5
			self.climbing = nil
		end)
	end
end

--- Causes the creature to die.
-- @param self Object.
-- @return True if killed, false if saved by Sanctuary.
Creature.die = function(self)
	if self.dead then return end
	-- Sanctuary save.
	if self.modifiers and self.modifiers.sanctuary and not self.beheaded then
		self:remove_modifier("sanctuary")
		self:damaged{amount = -10, type = "physical"}
		self:teleport{marker = "sanctuary"}
		return
	end
	-- Death dialog.
	if self.spec.dialog then
		local dialog = Dialog{object = self, name = self.spec.dialog .. " death"}
		if dialog then
			self.dialog = dialog
			self.dialog:execute()
			self.dialog = nil
		end
	end
	-- Disable controls etc.
	self:set_dead_state(true)
	return true
end

--- Equips the item passed as a parameter.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>object: Item to equip.</li></ul>
-- @return True if equipped successfully.
Creature.equip_item = function(self, args)
	local slot = args.object.spec.equipment_slot
	if not slot then return end
	local spec = args.object and args.object.spec
	-- Unequip the item in the destination slot.
	if not self:unequip_item{slot = slot} then return end
	-- Unequip items in slots reserved by the item.
	-- This handles cases such as a shield being in the left hand and a
	-- two-handed weapon being equipped into the right hand.
	if spec and spec.equipment_slots_reserved then
		for k in pairs(spec.equipment_slots_reserved) do
			if not self:unequip_item{slot = k} then return end
		end
	end
	-- Unequip items whose slot reservations conflict with the new item.
	-- This handles cases such as a two-handed weapon being in the right
	-- hand and a shield being equipped into the left hand.
	if spec then
		-- Conflict resolution can cause an error because the next key of the
		-- loop may be removed. In such a case, we catch the error and retry.
		repeat until pcall(function()
			for k,v in pairs(self.inventory.slots) do
				local conflict
				if type(k) == "string" and v.spec.equipment_slots_reserved then
					if v.spec.equipment_slots_reserved[slot] then
						conflict = true
					elseif spec.equipment_slots_reserved then
						for k1 in pairs(spec.equipment_slots_reserved) do
							if v.spec.equipment_slots_reserved[k1] then conflict = true end
						end
					end
				end
				if conflict then
					if not self:unequip_item{slot = k} then return end
				end
			end
		end)
	end
	-- Equip the item.
	self.inventory:set_object{object = args.object, slot = slot}
	-- Play the equip effect.
	if args.object and args.object.spec.effect_equip then
		Effect:play{effect = args.object.spec.effect_equip, object = self}
	end
	return true
end

--- Automatically equips the best set of items.
-- @param self Object.
-- @param args Arguments.
Creature.equip_best_items = function(self, args)
	-- Loop through all our equipment slots.
	for name in pairs(self.spec.equipment_slots) do
		-- Find the best item to place to the slot.
		local best = self:get_item{slot = name}
		local best_score = -1
		for index,item in pairs(self.inventory.slots) do
			if item.spec.equipment_slot == name then
				local score = 50 * item:get_armor_class(self)
				for k,v in pairs(item:get_weapon_influences(self)) do
					if k ~= "hatchet" then
						score = score + v
					end
				end
				if not best or score < best_score then
					best = item
					best_score = score
				end
			end
		end
		-- Place the best item to the slot.
		if best then self:equip_item{object = best} end
	end
end

--- Makes the creature face a point.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>point: Position vector.</li>
--   <li>secs: Tick length or nil for instant rotation.</li></ul>
-- @return The dot product of the current direction and the target direction.
Creature.face_point = function(self, args)
	local sdir = self.rotation * Vector(0, 0, -1)
	local edir = (args.point - self.position):normalize()
	local quat = Quaternion{dir = Vector(edir.x, 0, edir.z), up = Vector(0, 1, 0)}
	if args.secs then
		-- Interpolate rotation towards target point.
		-- TODO: Should use args.secs here somehow.
		self.rotation = self.rotation:nlerp(quat, 0.9)
		return sdir:dot(edir)
	else
		-- Instantly set the target rotation.
		self.rotation = quat
		return 1.0
	end
end

--- Gets the attack ray of the creature.
-- @param self Object.
-- @param rel Destination shift vector or nil.
-- @return Ray start point and ray end point relative to the object.
Creature.get_attack_ray = function(self, rel)
	local ctr = self.spec.aim_ray_center
	local ray1 = Vector(0, 0, -self.spec.aim_ray_start)
	local ray2 = Vector(0, 0, -self.spec.aim_ray_end)
	if rel then ray2 = ray2 + rel * self.spec.aim_ray_end end
	if self.tilt then
		local rot = Quaternion{euler = self.tilt.euler}
		local src = self.position + self.rotation * (ctr + rot * ray1)
		local dst = self.position + self.rotation * (ctr + rot * ray2)
		return src, dst
	else
		local src = self.position + self.rotation * (ctr + ray1)
		local dst = self.position + self.rotation * (ctr + ray2)
		return src, dst
	end
end

--- Returns the burdening limit of the creature.
-- @param self creature.
-- @return Burdening limit in kilograms
Creature.get_burden_limit = function(self)
	local str = self.skills:get_value{skill = "strength"} or 10
	return 100 + 4 * str
end

--- Returns true if the creature is burdened.
-- @param self Creature.
-- @return True if burdened.
Creature.get_burdened = function(self)
	return self.carried_weight > self:get_burden_limit()
end

--- Gets a modifier by name.
-- @param self Object.
-- @param name Modifer name.
Creature.get_modifier = function(self, name)
	if not self.modifiers then return end
	return self.modifiers[name]
end

--- Inflicts a modifier on the object.
-- @param self Object.
-- @param name Modifier name.
-- @param strength Modifier strength.
-- @param args Arguments passed to the modifier
Creature.inflict_modifier = function(self, name, strength, args)
	if not self.modifiers then self.modifiers = {} end
	if not self.modifiers[name] or self.modifiers[name].st < strength then
		self.modifiers[name] = {st=strength,a=args}
	end
end

Creature.jump = function(self)
	-- Check for preconditions.
	if self.blocking then return end
	if self.climbing then return end
	local t = Program.time
	if t - self.jumped < 0.5 then return end
	-- Jump or swim.
	if self.submerged and self.submerged > 0.4 then
		-- Swimming upwards.
		if self:get_burdened() then return end
		local v = self.velocity
		self.jumped = t - 0.3
		self.jumping = true
		if v.y < self.speed then
			Object.jump(self, {impulse = Vector(v.x, self.spec.swim_force * self.spec.mass, v.z)})
		end
	else
		-- Jumping.
		if not self.ground or self:get_burdened() then return end
		self.jumped = t
		self.jumping = true
		Effect:play{effect = "jump1", object = self}
		self:animate("jump")
		Coroutine(function(thread)
			Coroutine:sleep(self.spec.timing_jump * 0.02)
			if not self.realized then return end
			local v = self.velocity
			Object.jump(self, {impulse = Vector(v.x, self.spec.jump_force * self.spec.mass, v.z)})
		end)
	end
end

--- Loots the object.
-- @param self Object.
-- @param user Object doing the looting.
Creature.loot = function(self, user)
	if not self.dead and not user.sneak then return end
	return Object.loot(self, user)
end

--- Picks up an object.
-- @param self Object.
-- @param src_id ID of the picked up object.
-- @param dst_id ID of the inventory where to place the object.
-- @param dst_slot Name of the inventory slot where to place the object.
Creature.pick_up = function(self, src_id, dst_id, dst_slot)
	self:animate("pick up")
	Timer{delay = self.spec.timing_pickup * 0.02, func = function(timer)
		Actions:move_from_world_to_inv(self, src_id, dst_id, dst_slot)
		timer:disable()
	end}
end

--- Removes a modifier.
-- @param self Object.
-- @param name Modifier name.
Creature.remove_modifier = function(self, name)
	if self.modifiers and self.modifiers[name] then
		self.modifiers[name] = nil
		self:removed_modifier(name)
	end
end

--- Called when a modifier is removed.
-- @param self Object.
-- @param name Modifier name.
Creature.removed_modifier = function(self, name)
end

--- Enables or disables the blocking stance.
-- @param self Creature.
-- @param value True to block.
Creature.set_block = function(self, value)
	if self.jumping then return end
	if value and self.blocking then return end
	if not value and not self.blocking then return end
	if value then
		self.blocking = Program.time
		self:animate("block start")
	else
		self.blocking = nil
		self:animate("block stop")
	end
	self:calculate_speed()
	self:calculate_animation()
end

--- Sets the forward/backward movement state of the creature.
-- @param self Object.
-- @param value Movement rate.
Creature.set_movement = function(self, value)
	self.movement = math.min(1, math.max(-1, value))
	self:calculate_animation()
end

--- Sets a skill of the creature.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>name: Skill name.</li>
--   <li>value: New target value for the skill.</li></ul>
Creature.set_skill = function(self, name, value)
	-- Enforce species limit.
	local spec = self.spec.skills[name]
	if not spec then return end
	value = math.min(value, spec.max)
	value = math.max(value, 0)
	-- Enforce skill quota.
	if not self.skills then return end
	local t = self.skills:get_total() - self.skills:get_maximum{skill = name}
	value = math.min(value, self.spec.skill_quota - t)
	value = math.max(value, 0)
	-- Set the new maximum value.
	self.skills:set_maximum{skill = name, value = value}
end

--- Sets the strafing state of the creature.
-- @param self Object.
-- @param value Strafing rate.
Creature.set_strafing = function(self, value)
	self.strafing = value
	self:calculate_animation()
end

--- Unequips the item in the given equipment slot.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>slot: Slot name.</li></ul>
-- @return True if the slot is now empty.
Creature.unequip_item = function(self, args)
	local item = self.inventory:get_object{slot = args.slot}
	if not item then return true end
	if not self:add_item{object = item} then return end
	-- Play the unequip effect.
	if args.object and args.object.spec.effect_unequip then
		Effect:play{effect = args.object.spec.effect_unequip, object = self}
	end
	return true
end

--- Updates the state of the creature.
-- @param self Object.
-- @param secs Seconds since the last update.
Creature.update = function(self, secs)
	-- Update the state.
	self.update_timer = self.update_timer + secs
	if self.update_timer > 0.3 then
		local tick = self.update_timer
		if self.modifiers then Modifier:update(self, tick) end
		self.skills:update(tick)
		self:update_actions(tick)
		self:update_burdening(tick)
		self:update_environment(tick)
		self.update_timer = 0
	end
	-- Update the AI.
	if self.ai and not self.dead then
		self.ai:update(secs)
	end
end

Creature.update_actions = function(self, secs)
	-- Update flying.
	if self.flying and self.tilt then
		local v = self.velocity
		if math.abs(self.movement) > 0.5 then
			local e = self.tilt.euler[3]
			if e > 0 then
				self.velocity = Vector(v.x, math.max(v.y, 5*math.sin(e)), v.z)
			else
				self.velocity = Vector(v.x, math.min(v.y, 5*math.sin(e)), v.z)
			end
		else
			self.velocity = Vector()
		end
	end
	-- Update feat cooldown.
	if self.blocking then self.cooldown = self.spec.blocking_cooldown end
	if self.cooldown then
		self.cooldown = self.cooldown - secs
		if self.cooldown <= 0 then
			self.cooldown = nil
		end
	end
	-- Update auto-attack.
	if self.auto_attack and not self.cooldown and not self.attack_charge then
		self:attack_charge_start()
		self:attack_charge_end()
	end
	-- Check for falling damage.
	-- Don't update every frame.
	self.fall_timer = (self.fall_timer or 0) + secs
	if self.fall_timer > 0.3 then
		if self.velocity_prev then
			local limity = self.spec.falling_damage_speed
			local prevy = self.velocity_prev.y
			local diffy = self.velocity.y - prevy
			if prevy < -limity and diffy > limity then
				local damage = (diffy - limity) * self.spec.falling_damage_rate
				if damage > 2 then
					self:damaged{amount = damage, type = "falling"}
					if self.spec.effect_falling_damage then
						Effect:play{effect = self.spec.effect_falling_damage, object = self}
					end
				end
			end
		end
		self.velocity_prev = self.velocity
	end
	-- Play the landing animation after jumping.
	-- Initiate climbing when applicable.
	if self.jumping then
		-- Climbing.
		-- Player have an explicit climb command so auto-climb is only used by NPCs.
		if not self.client then
			self:climb()
		end
		-- Landing.
		self.jump_timer = (self.jump_timer or 0) + secs
		if self.jump_timer > 0.2 and Program.time - self.jumped > 0.8 and self.ground then
			if not self.submerged or self.submerged < 0.3 then
				self:animate("land ground")
				if self.spec.effect_landing then
					Effect:play{effect = self.spec.effect_landing, object = self}
				end
			else
				self:animate("land water")
			end
			self.jumping = nil
		end
	end
end
--- Updates the AI state of the creature.<br/>
-- You can force the AI to recheck its state immediately by calling the function.
-- This can be useful when, for example, the player has initiated a dialog and
-- you want to set the NPC into the chat mode immediately instead of waiting for
-- the next AI update cycle.
-- @param self Object.
Creature.update_ai_state = function(self)
	if self.ai then
		self.ai:update_state()
	end
end

Creature.update_burdening = function(self, secs)
	-- Don't update every frame.
	self.burden_timer = (self.burden_timer or 0) + secs
	if self.burden_timer < 1 then return end
	self.burden_timer = 0
	-- Update burdening.
	-- The burdening status may change when the contents of the inventory
	-- or any subinventory change or if the strength skill level changes.
	-- The burdening status decreases movement speed and disables jumping.
	local prev_limit = self.burden_limit or 0
	local curr_limit = math.floor(self:get_burden_limit())
	local prev_burden = self:get_burdened()
	local curr_weight = math.ceil(self:calculate_carried_weight())
	if curr_weight ~= self.carried_weight or prev_limit ~= curr_limit then
		self.carried_weight = curr_weight
		self.burden_limit = curr_limit
		self:send{packet = Packet{packets.INVENTORY_WEIGHT, "uint16", curr_weight, "uint16", curr_limit}}
	end
	local curr_burden = self:get_burdened()
	if prev_burden ~= curr_burden then
		if curr_burden then
			self:send{packet = Packet{packets.MESSAGE, "string", "You're now burdened."}}
		else
			self:send{packet = Packet{packets.MESSAGE, "string", "You're no longer burdened."}}
		end
	end
	-- Update speed.
	-- Skill regeneration affects speed too so this needs to be recalculated
	-- every now and then regardless of burdening.
	self:calculate_speed()
end

--- Updates the environment of the object and tries to fix it if necessary.
-- @param self Object.
-- @param secs Seconds since the last update.
-- @return Boolean and environment statistics. The boolean is true if the object isn't permanently stuck.
Creature.update_environment = function(self, secs)
	-- Don't update every frame.
	if not self.realized then return true end
	self.env_timer = (self.env_timer or 0) + secs
	if self.env_timer < 1.2 then return true end
	local tick = self.env_timer
	self.env_timer = 0
	-- Count tiles affecting us.
	local src,dst = self:get_tile_range()
	local env = Voxel:check_range(src, dst)
	if not env then return true end
	-- Count liquid tiles.
	local liquid = env.liquid / env.total
	local magma = env.magma / env.total
	if liquid ~= (self.submerged or 0) then
		self.submerged = liquid > 0 and liquid or nil
	end
	if magma ~= (self.submerged_in_magma or 0) then
		self.submerged_in_magma = magma > 0 and magma or nil
	end
	-- Apply liquid damage.
	if not self.dead and self.submerged then
		local magma = self.submerged_in_magma or 0
		local water = self.submerged - magma
		if magma > 0 and self.damage_from_magma ~= 0 then
			self:damaged{amount = self.spec.damage_from_magma * magma * tick, type = "liquid"}
		end
		if water > 0 and self.damage_from_water ~= 0 then
			self:damaged{amount = self.spec.damage_from_water * water * tick, type = "liquid"}
		end
	end
	return true, env
end

--- Serializes the object to a string.
-- @param self Object.
-- @return Data string.
Creature.write = function(self)
	return string.format("local self=Creature%s\n%s%s%s", serialize{
		angular = self.angular,
		beheaded = self.beheaded or nil,
		dead = self.dead,
		eye_style = self.eye_style,
		face_style = self.face_style,
		hair_style = self.hair_style,
		id = self.id,
		physics = self.physics,
		position = self.position,
		rotation = self.rotation,
		spec = self.spec.name,
		variables = self.variables},
		Serialize:encode_skills(self.skills),
		Serialize:encode_inventory(self.inventory),
		"return self")
end

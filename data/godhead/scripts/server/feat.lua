local oldinfo = Feat.get_info

--- Adds the best possible effects to the feat.
-- @param self Feat.
-- @param args Arguments.<ul>
--   <li>category: Category from which to pick effects.</li>
--   <li>user: Object whose skills and inventory to use.</li></ul>
Feat.add_best_effects = function(self, args)
	-- Solves the maximum value the effect can have without the feat
	-- becoming unusable. The solution is found by bisecting.
	local solve_effect_value = function(feat, effect)
		local i = #feat.effects + 1
		local e = {effect.name, 0}
		local step = 50
		feat.effects[i] = e
		repeat
			e[2] = e[2] + step
			if not feat:usable{user = args.user} then
				e[2] = e[2] - step
			end
			step = step / 2
		until step < 1
		feat.effects[i] = nil
		return e[2]
	end
	-- Add usable feat effects.
	-- TODO: Reject effects if this will be used for animations that can have too many.
	local anim = Featanimspec:find{name = self.animation}
	for name in pairs(args.user.spec.feat_effects) do
		if anim.effects[name] then
			local effect = Feateffectspec:find{name = name}
			if effect and (not args.category or effect.categories[args.category]) then
				local value = solve_effect_value(self, effect)
				if value >= 1 then self.effects[#self.effects + 1] = {name, value} end
			end
		end
	end
end

--- Applies the feat.
-- @param self Feat.
-- @param args Arguments.<ul>
--   <li>attacker: Attacking creature.</li>
--   <li>point: Hit point in world space.</li>
--   <li>projectile: Projectile.</li>
--   <li>target: Attacked creature.</li>
--   <li>tile: Attacked tile or nil.</li>
--   <li>weapon: Used weapon.</li></ul>
Feat.apply = function(self, args)
	-- Effects.
	local effects = {}
	local anim = Featanimspec:find{name = self.animation}
	if anim and anim.effect_impact then
		effects[anim.effect_impact] = true
	end
	for index,data in ipairs(self.effects) do
		local effect = Feateffectspec:find{name = data[1]}
		if effect and effect.effect then
			effects[effect.effect] = true
		end
	end
	for effect in pairs(effects) do
		if args.target then
			Effect:play{effect = effect, object = args.target}
		else
			Effect:play{effect = effect, point = args.point}
		end
	end
	-- Impulse.
	if anim.categories["melee"] or anim.categories["ranged"] or
	   anim.categories["self"] or anim.categories["touch"] then
		if args.target and args.attacker then
			args.target:impulse{impulse = args.attacker.rotation * Vector(0, 0, -100)}
		end
	end
	-- Cooldown.
	-- The base cooldown has already been applied but we add some extra
	-- if the target was blocking in order to penalize rampant swinging.
	if anim.categories["melee"] and args.attacker and args.target and args.target.blocking then
		if Program.time - args.target.blocking > args.target.spec.blocking_delay then
			args.attacker.cooldown = (args.attacker.cooldown or 0) * 2
		end
	end
	-- Influences.
	local info = self:get_info(args)
	for k,v in pairs(info.influences) do
		local i = Influencespec:find{name = k}
		if i then i.func(self, info, args, v) end
	end
	-- Digging.
	if anim.categories["melee"] and args.tile then
		-- Break the tile.
		if (args.weapon and args.weapon.spec.categories["mattock"]) or math.random(1, 5) == 5 then
			Voxel:damage(args.attacker, args.tile)
		end
		-- Damage the weapon.
		if args.weapon and args.weapon.spec.damage_mining then
			if not args.weapon:damaged{amount = 2 * args.weapon.spec.damage_mining * math.random(), type = "mining"} then
				args.attacker:send{packet = Packet(packets.MESSAGE, "string",
					"The " .. args.weapon.spec.name .. " broke!")}
			end
		end
	end
	-- Building.
	if anim.categories["build"] and args.tile and args.weapon then
		if args.weapon.spec.construct_tile then
			local m = Material:find{name = args.weapon.spec.construct_tile}
			local need = args.weapon.spec.construct_tile_count or 1
			local have = args.weapon.count
			if m and need <= have then
				local t,p = Utils:find_build_point(args.point, args.attacker)
				if t then
					local o = args.weapon:split{count = need}
					o:detach()
					Voxel:set_tile(p, m.id)
					if m.effect_build then
						Effect:play{effect = m.effect_build, point = p * Voxel.tile_size}
					end
				end
			end
		end
	end
end

--- Performs a feat
-- @param self Feat.
-- @param args Arguments.<ul>
--   <li>user: Object using the feat. (required)</li>
--   <li>stop: True if stopped performing, false if started.</li></ul>
-- @return True if performed successfully.
Feat.perform = function(self, args)
	local anim = Featanimspec:find{name = self.animation}
	local slot = anim and (anim.slot or (anim.required_weapon and args.user.spec.weapon_slot))
	local weapon = slot and args.user:get_item{slot = slot}
	local info = anim and self:get_info{attacker = args.user, weapon = weapon}
	-- Check for cooldown and requirements.
	if info and not args.stop then
		if args.user.cooldown then return end
		if not self:usable(args) then return end
		for k,v in pairs(info.required_reagents) do
			args.user:subtract_items{name = k, count = v}
		end
		local w = info.required_skills["willpower"]
		if w then args.user.skills:subtract{skill = "willpower", value = w} end
		if info.cooldown > 0 then
			args.user.cooldown = info.cooldown
		end
	end
	-- Calculate the charge time.
	if args.user.attack_charge then
		args.charge = Program.time - args.user.attack_charge
	end
	-- Call the feat function.
	local move
	if not info or anim.toggle or not args.stop then
		args.weapon = weapon
		if anim then
			local a = Actionspec:find{name = anim.action}
			if a then move = a.func(self, info, args) end
		end
		if self.func then self:func(args) end
	end
	-- Animate the feat.
	-- The move parameter is used by melee feats to tell which attack animation was used.
	Vision:event{type = "object-feat", object = args.user, anim = anim, move = move}
	return true
end

--- Plays the effects of the feat.
-- @param self Feat.
-- @param args Arguments.<ul>
--   <li>user: Object using the feat. (required)</li>
--   <li>stop: True if stopped performing, false if started.</li></ul>
Feat.play_effects = function(self, args)
	local anim = Featanimspec:find{name = self.animation}
	if not anim then return end
	if anim.effect then
		Effect:play{effect = anim.effect, object = args.user}
	end
	if anim.action ~= "melee" and anim.slot then
		local weapon = args.user:get_item{slot = anim.slot}
		if weapon and weapon.spec.effect_attack then
			Effect:play{effect = weapon.spec.effect_attack, object = args.user}
		end
	end
end

--- Unlocks a random feat to the player base.
-- @param self Feat class.
-- @param args Arguments.<ul>
--   <li>category: Category name or nil.</li></ul>
-- @return Feat or nil.
Feat.unlock = function(clss, args)
	print("Warning: unlocking feats isn't implemented, nor are there any locked feats.")
	return nil
end

--- Checks if the feat can be used with the given skills.
-- @param self Feat.
-- @param args Arguments.<ul>
--   <li>skills: Skills of the user.</li>
--   <li>inventory: Inventory of the user.</li>
--   <li>user: Object whose skills and inventory to use.</li></ul>
-- @return True if usable.
Feat.usable = function(self, args)
	-- Check for support by the species.
	if not args.user then return end
	local spec = args.user.spec
	if spec.type ~= "species" then return end
	if not spec.feat_anims[self.animation] then return end
	-- Get feat information.
	local anim = Featanimspec:find{name = self.animation}
	if not anim then return end
	local weapon = args.user:get_item{slot = anim.slot}
	local info = self:get_info{attacker = args.user, weapon = weapon}
	-- Check for skills.
	local skills = args.skills or args.user.skills
	for k,v in pairs(info.required_skills) do
		if not skills then return end
		local val = skills:get_value{skill = k}
		if not val or val < v then return end
	end
	-- Check for reagents.
	local inventory = args.inventory or args.user.inventory
	for k,v in pairs(info.required_reagents) do
		if not inventory then return end
		local item = inventory:find_object{name = k}
		if not item or item.count < v then return end
	end
	-- Check for ammo.
	for k,v in pairs(info.required_ammo) do
		if not inventory then return end
		local item = inventory:find_object{name = k}
		if not item or item.count < v then return end
	end
	-- Check for weapon.
	if info.required_weapon then
		if not inventory then return info.required_weapon == "melee" end
		local weapon = inventory:get_object{slot = args.user.spec.weapon_slot}
		if not weapon then return info.required_weapon == "melee" end
		if not weapon.spec.categories[info.required_weapon] then return end
	end
	return true
end


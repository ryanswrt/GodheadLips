require "common/spec"

Species = Class(Spec)
Species.type = "species"
Species.dict_id = {}
Species.dict_cat = {}
Species.dict_name = {}

--- Registers a new species.
-- @param clss Species class.
-- @param args Arguments.<ul>
--   <li>ai_enable_attack: False to prohibit attacking.</li>
--   <li>ai_enable_backstep: False to prohibit backstep.</li>
--   <li>ai_enable_block: False to prohibit blocking.</li>
--   <li>ai_enable_combat: False to prohibit combat.</li>
--   <li>ai_enable_jump: False to prohibit jumping.</li>
--   <li>ai_enable_spells: False to prohibit spell casting.</li>
--   <li>ai_enable_strafe: False to prohibit strafing.</li>
--   <li>ai_enable_wander: False to prohibit wandering.</li>
--   <li>ai_enable_walk: False to prohibit walking.</li>
--   <li>ai_enable_weapon_switch: False to prohibit weapon switching.</li>
--   <li>ai_enabled: False to disable AI for the species.</li>
--   <li>ai_offense_factor: How much the creature prefers offense over defense, in the range of [0,1].</li>
--   <li>ai_search_time: Time in seconds how long the AI will search for hidden foes.</li>
--   <li>ai_update_delay: Number of seconds between AI state updates.</li>
--   <li>ai_wait_allowed: False to prohibit waiting.</li>
--   <li>ai_wander_time: Time in seconds how long the AI will wander before going to sleep.</li>
--   <li>aim_ray_center: Center vector of the aim ray.</li>
--   <li>aim_ray_end: Aim ray end distance.</li>
--   <li>aim_ray_start: Aim ray start distance.</li>
--   <li>animations: Dictionary of animation playback rules.</li>
--   <li>blocking_armor: How much armor class blocking offsers.</li>
--   <li>blocking_cooldown: Time in seconds how long it takes to leave the blocking stance.</li>
--   <li>blocking_delay: Time in seconds how long it takes to enter the blocking stance.</li>
--   <li>body_scale: Body scale range.</li>
--   <li>damage_from_magma: Points of damage from magma per second.</li>
--   <li>damage_from_water: Points of damage from water per second.</li>
--   <li>dead: True if the creature should spawn as dead.</li>
--   <li>difficulty: The approximate difficulty of the creature in the range of [0,1].</li>
--   <li>effect_falling_damage: Name of the effect played when the creature takes falling damage.</li>
--   <li>effect_landing: Name of the effect played when the creature lands after jumping.</li>
--   <li>effect_physical_damage: Name of the effect played when the creature is hurt physically.</li>
--   <li>equipment_slots: List of equipment slots.</li>
--   <li>factions: List of factions.</li>
--   <li>falling_damage_rate: Number of points of damage per every meters per second exceeding the falling damage speed.</li>
--   <li>falling_damage_speed: Speed in meters per seconds after which the creature starts taking falling damage.</li>
--   <li>feat_animations: List of know feat types.</li>
--   <li>feat_effects: List of know feat effects.</li>
--   <li>footstep_height: Footstep height.</li>
--   <li>footstep_sound: Name of the footstep sound effect.</li>
--   <li>gravity: Gravity vector.</li>
--   <li>interactive: False to make the object not appear interactive.</li>
--   <li>inventory_items: List of inventory items to give when the creature is spawned.</li>
--   <li>inventory_size: Number of inventory slots the creature has.</li>
--   <li>jump_force: Mass-independent jump force of the creature.</li>
--   <li>loot_categories: List of item categories this creature can have as random loot.</li>
--   <li>loot_count: Minimum and maximum number of random loot items this creature can have.</li>
--   <li>mass: Mass in kilograms.</li>
--   <li>model: Model name.</li>
--   <li>skill_regen: Skill regeneration speed in units per second.</li>
--   <li>skill_quota: Number of skill points the creature can distribute over skills.</li>
--   <li>skills: List of skills.</li>
--   <li>skin_shader: Skin shader name.</li>
--   <li>skin_textures: List of skin textures.</li>
--   <li>special_effects: List of special effects to render. (see itemspec for details)</li>
--   <li>speed_run: Turning speed in meters per second.</li>
--   <li>speed_walk: Walking speed in meters per second.</li>
--   <li>swim_force: Mass-independent upward swim force of the creature.</li>
--   <li>tilt_bone: Name of the tilt bone of the creature.</li>
--   <li>tilt_limit: Tilt limit in radians of the tilting bone.</li>
--   <li>timing_attack_bow: Timing of bow firing, in frames.</li>
--   <li>timing_attack_crossbow: Timing of crossbow firing, in frames.</li>
--   <li>timing_attack_explode: Timing of explosion attack, in frames.</li>
--   <li>timing_attack_melee: Timing of melee attack impact, in frames.</li>
--   <li>timing_attack_musket: Timing of musket firing, in frames.</li>
--   <li>timing_attack_revolver: Timing of revolver firing, in frames.</li>
--   <li>timing_attack_throw: Timing of releasing the thrown item, in frames.</li>
--   <li>timing_build: Timing of releasing the thrown item, in frames.</li>
--   <li>timing_drop: Timing of dropping an item, in frames.</li>
--   <li>timing_jump: Timing of lifting off when jumping, in frames.</li>
--   <li>timing_pickup: Timing of picking up an item, in frames.</li>
--   <li>timing_spell_ranged: Timing of casting a ranged spell projectile, in frames.</li>
--   <li>timing_spell_self: Timing of casting a spell on self, in frames.</li>
--   <li>timing_spell_touch: Timing of casting a spell on touch, in frames.</li>
--   <li>view_cone: View cone angle in radians.</li>
--   <li>weapon_slot: Name of the weapon slot.</li>
--   <li>water_friction: How much being in water slows the creature down.</li>
--   <li>water_gravity: The gravity of the creature in water.</li></ul>
-- @return New species.
Species.new = function(clss, args)
	-- Copy arguments from the base species.
	-- The arguments used to initialize each species have been stored to the
	-- args field of the instance so that they can be easily copied to the
	-- inherited species here.
	local base = args.base and Species:find{name = args.base}
	if base then
		local t = {}
		for k,v in pairs(base.args) do t[k] = v end
		for k,v in pairs(args) do t[k] = v end
		args = t
	end
	-- Allocate the species.
	-- Many arguments are copied directly from the argument list but some
	-- are reset to empty arrays because they require conversion that is
	-- done later. The argument list is also stored to the species to
	-- allow easy inheritance.
	local self = Spec.new(clss, args)
	self.args = args
	-- Animations.
	-- Inheritance of animations is special in that they're inherited
	-- in per animation basis instead of the usual all or none inheritance.
	-- This allows species to replace individual animations easily.
	self.animations = {}
	if base and base.animations then
		for k,v in pairs(base.animations) do
			self.animations[k] = v
		end
	end
	if args.animations then
		-- We provide some helper node weights here since listing all of these
		-- in the specs would be way too verbose and inflexible. If one of the
		-- recognized uppercase node names is seen in the node weight list,
		-- it's replaced with the list of nodes in the mapping table.
		local translate_node_weights = function(args)
			local mapping = {
				LOWER = {"back1", "pelvis", "leg1.L", "leg2.L", "foot.L", "leg1.R", "leg2.R", "foot.R", "pelvis1", "leg1", "leg2", "leg3", "leg4", "leg5", "leg6"},
				LEGS = {"leg1.L", "leg2.L", "foot.L", "leg1.R", "leg2.R", "foot.R", "leg1", "leg2", "leg3", "leg4", "leg5", "leg6"},
				ARMS = {"arm1.L", "arm2.L", "wrist.L", "palm.L", "arm1.R", "arm2.R", "wrist.R", "palm.R"},
				ARML = {"arm1.L", "arm2.L", "wrist.L", "palm.L"},
				ARMR = {"arm1.R", "arm2.R", "wrist.R", "palm.R"},
				BACK = {"back", "back1", "back2", "back3"}}
			local w = {}
			for k,v in pairs(args) do
				local replace = mapping[k]
				if replace then
					for k1,v1 in ipairs(replace) do w[v1] = v end
				else
					w[k] = v
				end
			end
			return w
		end
		-- Add animations.
		-- Each animation in arguments is added to the animation list as is,
		-- apart from the node_weights array that has some translation rules.
		for k,v in pairs(args.animations) do
			self.animations[k] = v
			if v.node_weights then
				v.node_weights = translate_node_weights(v.node_weights)
			end
		end
	end
	-- Equipment slots.
	-- Converted from a list to a dictionary to make searching easier.
	self.equipment_slots = {}
	if args.equipment_slots then
		for k,v in pairs(args.equipment_slots) do
			self.equipment_slots[v.name] = v
		end
	end
	-- Factions.
	-- Converted from a list to a dictionary to make searching easier.
	-- The faction names are also replaced by the faction objects themselves.
	self.factions = {}
	if args.factions then
		for k,v in pairs(args.factions) do
			local f = Faction:find{name = v}
			assert(f, string.format("species %q references a missing faction %q", self.name, v))
			self.factions[v] = f
		end
	end
	-- Feats.
	-- Converted from a list to a dictionary to make searching easier.
	-- The key is the name of the feat and the value is unused.
	self.feat_anims = {}
	if args.feat_anims then
		for k,v in pairs(args.feat_anims) do
			self.feat_anims[v] = true
		end
	end
	self.feat_effects = {}
	if args.feat_effects then
		for k,v in pairs(args.feat_effects) do
			self.feat_effects[v] = true
		end
	end
	-- Inventory items.
	-- Some items are in a list form and others have the name as the key and
	-- the count as a value. All are converted to the latter form.
	self.inventory_items = {}
	if args.inventory_items then
		for k,v in pairs(args.inventory_items) do
			if type(k) == "string" then
				assert(not self.inventory_items[k], string.format("species %q has a duplicate inventory item %q", self.name, k))
				self.inventory_items[k] = v
			else
				assert(not self.inventory_items[v], string.format("species %q has a duplicate inventory item %q", self.name, v))
				self.inventory_items[v] = 1
			end
		end
	end
	-- Skills.
	-- Converted from a list to a dictionary to make searching easier.
	self.skills = {}
	if args.skills then
		for k,v in pairs(args.skills) do
			self.skills[v.name] = v
		end
	end
	-- Default values.
	-- To make things easier for the rest of the scripts, some fields are
	-- guaranteed to always have a value. The default values are set here
	-- if no value is assigned to the field yet.
	local copy = function(f, d) if self[f] == nil then self[f] = d end end
	copy("ai_enable_attack", true)
	copy("ai_enable_backstep", true)
	copy("ai_enable_block", true)
	copy("ai_enable_combat", true)
	copy("ai_enable_jump", true)
	copy("ai_enable_spells", true)
	copy("ai_enable_strafe", true)
	copy("ai_enable_wander", true)
	copy("ai_enable_walk", true)
	copy("ai_enable_weapon_switch", true)
	copy("ai_enabled", true)
	copy("ai_offense_factor", 0.75)
	copy("ai_search_time", 20)
	copy("ai_update_delay", 2)
	copy("ai_wait_allowed", true)
	copy("ai_wander_time", 60)
	copy("aim_ray_center", Vector(0, 1, 0))
	copy("aim_ray_end", 5)
	copy("aim_ray_start", 0.1)
	copy("blocking_armor", 0.5)
	copy("blocking_cooldown", 0.4)
	copy("blocking_delay", 0.4)
	copy("body_scale", {0.9, 1.1})
	copy("damage_from_magma", 6)
	copy("damage_from_water", 0)
	copy("dead")
	copy("difficulty", 0)
	copy("effect_falling_damage")
	copy("effect_landing")
	copy("effect_physical_damage")
	copy("falling_damage_rate", 10)
	copy("falling_damage_speed", 10)
	copy("footstep_height")
	copy("footstep_sound")
	copy("gravity", Vector(0,-15,0))
	copy("interactive", true)
	copy("inventory_size", 0)
	copy("jump_force", 8)
	copy("mass", 50)
	copy("skill_regen", 0.5)
	copy("skill_quota", 200)
	copy("skin_shader")
	copy("skin_textures")
	copy("special_effects", {})
	copy("speed_walk", 3)
	copy("speed_run", 6)
	copy("swim_force", 3.5)
	copy("tilt_limit", math.pi)
	copy("timing_attack_bow", 20)
	copy("timing_attack_crossbow", 20)
	copy("timing_attack_explode", 120)
	copy("timing_attack_melee", 20)
	copy("timing_attack_musket", 20)
	copy("timing_attack_revolver", 20)
	copy("timing_attack_throw", 20)
	copy("timing_build", 40)
	copy("timing_drop", 20)
	copy("timing_jump", 20)
	copy("timing_pickup", 40)
	copy("timing_spell_ranged", 40)
	copy("timing_spell_self", 40)
	copy("timing_spell_touch", 40)
	copy("view_cone", 0.8 * math.pi)
	copy("weapon_slot", "hand.R")
	copy("water_friction", 0.8)
	copy("water_gravity", Vector(0,-1,0))
	-- Precalculate combat abilities.
	self.can_melee = false
	self.can_ranged = false
	self.can_throw = false
	self.can_cast_ranged = false
	self.can_cast_self = false
	self.can_cast_touch = false
	for k,v in pairs(self.feat_anims) do
		local feat = Featanimspec:find{name = k}
		if self.ai_enable_attack then
			if feat.categories["melee"] then self.can_melee = true end
			if feat.categories["ranged"] then self.can_ranged = true end
			if feat.categories["throw"] then self.can_throw = true end
		end
		if self.ai_enable_spells then
			if feat.categories["ranged spell"] then self.can_cast_ranged = true end
			if feat.categories["spell on self"] then self.can_cast_self = true end
			if feat.categories["spell on touch"] then self.can_cast_touch = true end
		end
	end
	return self
end

--- Checks if the object is an enemy of the species.
-- @param self Species.
-- @param object Object.
-- @return True if an enemy.
Species.check_enemy = function(self, object)
	if object.spec.type == "species" then
		for name1,spec1 in pairs(self.factions) do
			for name2,spec2 in pairs(object.spec.factions) do
				if spec1.enemies[name2] then
					return true
				end
			end
		end
	end
end

--- Sets the factions of the species.
-- @param self Species.
-- @param args List of species.
Species.set_factions = function(self, args)
	if args then
		self.factions = {}
		for k,v in pairs(args) do
			self.factions[v] = Faction:find{name = v}
		end
	end
end

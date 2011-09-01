-- (materials sorted by durability)
-- weapon\matrl W  S  Q  I  C  A
-- ======================================
-- mattock      +  +  +  +  +  +
-- staff        +  -  -  -  -  -
--
-- dagger       +  +  +  +  +  +
-- hatchet      +  +  +  +  +  +
-- sword        +  +  +  +  +  +
-- club         +  -  -  -  -  -
--
-- grenade      -  -  +  -  +  -
--
-- crossbow     +  -  +  +  +  +
-- musket       -  -  +  +  +  +
-- revolver     -  -  +  +  +  +

Itemspec{
	name = "adamantium crossbow",
	model = "crossbow1",
	icon = "crossbow2",
	ammo_type = "arrow",
	animation_attack = "attack crossbow",
	animation_charge = "charge crossbow",
	animation_hold = "hold crossbow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["adamantium"] = 9},
	effect_attack = "crossbow1",
	influences_base = {physical = -15},
	influences_bonus = {dexterity = 0.01},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	mass = 15}

Itemspec{
	name = "adamantium dagger",
	model = "dagger1", -- FIXME
	icon = "dagger1",
	categories = {"melee", "weapon"},
	crafting_materials = {["adamantium"] = 3},
	effect_attack_speedline = true,
	influences_base = {physical = -10},
	influences_bonus = {dexterity = 0.01},
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "iron halberd",
	model = "halberd1",
	icon = "staff1", --FIXME
	animation_attack = "attack staff",
	animation_charge = "charge staff",
	animation_hold = "hold staff",
	categories = {"melee", "weapon"},
	crafting_materials = {["iron"] = 4},
	effect_attack_speedline = true,
	influences_base = {physical = -10},
	influences_bonus = {dexterity = 0.01},
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "iron spear",
	model = "spear1",
	icon = "staff1", --FIXME
	categories = {"melee", "weapon"},
	crafting_materials = {["iron"] = 4},
	effect_attack_speedline = true,
	influences_base = {physical = -10},
	influences_bonus = {dexterity = 0.01},
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "adamantium hatchet",
	model = "axe1",
	icon = "axe3",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["adamantium"] = 3},
	influences_base = {hatchet = -10, physical = -10},
	influences_bonus = {strength = 0.01},
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "adamantium mattock",
	model = "mattock-000",
	icon = "mattock1",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["adamantium"] = 9},
	description = "A high quality mattock used to mine ores",
	influences_base = {physical = -10},
	influences_bonus = {strength = 0.01},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	mass = 15}

Itemspec{
	name = "adamantium musket",
	model = "musket1",
	icon = "musket1",
	ammo_type = "bullet",
	animation_attack = "attack musket",
	animation_charge = "charge musket",
	animation_hold = "hold musket",
	categories = {"ranged", "weapon"},
	crafting_materials = {["adamantium"] = 15, ["log"] = 2},
	effect_attack = "musket1",
	influences_base = {physical = -25},
	influences_bonus = {intelligence = 0.005, perception = 0.005},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	mass = 15}

Itemspec{
	name = "adamantium revolver",
	model = "revolver1",
	icon = "revolver1",
	ammo_type = "bullet",
	animation_attack = "attack revolver",
	animation_charge = "charge revolver",
	animation_hold = "hold revolver",
	categories = {"ranged", "weapon"},
	crafting_materials = {["adamantium"] = 10, ["log"] = 1},
	effect_attack = "revolver1",
	influences_base = {physical = -18},
	influences_bonus = {intelligence = 0.005, perception = 0.005},
	equipment_slot = "hand.R",
	mass = 8}

Itemspec{
	name = "adamantium sword",
	model = "sword6",
	icon = "sword3",
	animation_attack = "attack sword",
	animation_charge = "charge sword",
	animation_hold = "hold sword",
	categories = {"melee", "weapon"},
	crafting_materials = {["adamantium"] = 3},
	effect_attack_speedline = true,
	influences_base = {physical = -20},
	influences_bonus = {dexterity = 0.005, strength = 0.005},
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "aquanite crossbow",
	model = "crossbow1",
	icon = "crossbow1",
	ammo_type = "arrow",
	animation_attack = "attack crossbow",
	animation_charge = "charge crossbow",
	animation_hold = "hold crossbow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["aquanite stone"] = 9},
	effect_attack = "crossbow1",
	effect_craft = "craftmetal1",
	influences_base = {cold = -6, physical = -6},
	influences_bonus = {dexterity = 0.01},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	mass = 15}

Itemspec{
	name = "aquanite dagger",
	model = "dagger1", -- FIXME
	icon = "dagger1",
	categories = {"melee", "weapon"},
	crafting_materials = {["aquanite stone"] = 3},
	damage_mining = 1,
	effect_attack_speedline = true,
	effect_craft = "craftmetal1",
	influences_base = {cold = -4, physical = -4},
	influences_bonus = {dexterity = 0.01},
	equipment_slot = "hand.R",
	health = 500,
	mass = 15}

Itemspec{
	name = "aquanite grenade",
	model = "grenade1",
	icon = "grenade1",
	mass = 3,
	animation_attack = "throw grenade",
	animation_charge = "charge grenade",
	animation_hold = "hold grenade",
	categories = {"explosive", "throwable", "weapon"},
	crafting_materials = {["aquanite stone"] = 1, ["iron ingot"] = 1},
	damage_mining = 1,
	destroy_actions = {"explode"},
	destroy_timer = 3,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 1,
	influences_base = {cold = -15, physical = -5},
	influences_bonus = {intelligence = 0.01},
	mass = 5,
	stacking = true}

Itemspec{
	name = "aquanite hatchet",
	model = "axe1",
	icon = "axe2",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["aquanite stone"] = 3},
	damage_mining = 1,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	influences_base = {cold = -4, hatchet = -7, physical = -4},
	influences_bonus = {strength = 0.01},
	health = 500,
	mass = 15}

Itemspec{
	name = "aquanite mattock",
	model = "mattock-000",
	icon = "mattock1",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"mattock", "melee", "weapon"},
	description = "A high quality mattock used to mine ores",
	crafting_materials = {["aquanite stone"] = 9},
	damage_mining = 1,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 500,
	influences_base = {cold = -3, physical = -4},
	influences_bonus = {strength = 0.01},
	mass = 15}

Itemspec{
	name = "aquanite revolver",
	model = "revolver1",
	icon = "revolver1",
	ammo_type = "bullet",
	animation_attack = "attack revolver",
	animation_charge = "charge revolver",
	animation_hold = "hold revolver",
	categories = {"ranged", "weapon"},
	crafting_materials = {["aquanite stone"] = 10, ["log"] = 1},
	effect_attack = "revolver1",
	influences_base = {cold = -7, physical = -7},
	influences_bonus = {intelligence = 0.005, perception = 0.005},
	equipment_slot = "hand.R",
	mass = 8}

Itemspec{
	name = "aquanite musket",
	model = "musket1",
	icon = "musket1",
	ammo_type = "bullet",
	animation_attack = "attack musket",
	animation_charge = "charge musket",
	animation_hold = "hold musket",
	categories = {"ranged", "weapon"},
	crafting_materials = {["aquanite stone"] = 15, ["log"] = 2},
	effect_attack = "musket1",
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	influences_base = {cold = -9, physical = -9},
	influences_bonus = {intelligence = 0.005, perception = 0.005},
	mass = 15}

Itemspec{
	name = "aquanite sword",
	model = "sword5",
	icon = "sword2",
	animation_attack = "attack sword",
	animation_charge = "charge sword",
	animation_hold = "hold sword",
	categories = {"melee", "weapon"},
	crafting_materials = {["aquanite stone"] = 3},
	damage_mining = 1,
	effect_attack_speedline = true,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 500,
	influences_base = {cold = -8, physical = -8},
	influences_bonus = {dexterity = 0.005, strength = 0.005},
	mass = 15}

Itemspec{
	name = "boomerang",
	animations = {
		["fly start"] = {animation = "fly", channel = 1, permanent = true},
		["fly stop"] = {channel = 1, permanent = true}},
	mass = 2,
	model = "boomerang1",
	icon = "boomerang1",
	categories = {"boomerang", "throwable", "weapon"},
	crafting_materials = {log = 2},
	effect_craft = "craftwood1",
	gravity_projectile = Vector(0,2,0),
	influences_base = {physical = -5},
	influences_bonus = {dexterity = 0.01},
	equipment_slot = "hand.R",
	stacking = true,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "crimson crossbow",
	model = "crossbow1",
	icon = "crossbow1",
	ammo_type = "arrow",
	animation_attack = "attack crossbow",
	animation_charge = "charge crossbow",
	animation_hold = "hold crossbow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["crimson stone"] = 9},
	effect_attack = "crossbow1",
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	influences_base = {fire = -6, physical = -6},
	influences_bonus = {dexterity = 0.01},
	mass = 15}

Itemspec{
	name = "crimson dagger",
	model = "dagger1", -- FIXME
	icon = "dagger1",
	categories = {"melee", "weapon"},
	crafting_materials = {["crimson stone"] = 3},
	damage_mining = 1,
	effect_attack_speedline = true,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 500,
	influences_base = {fire = -4, physical = -4},
	influences_bonus = {dexterity = 0.01},
	mass = 15}

Itemspec{
	name = "crimson grenade",
	model = "grenade1",
	icon = "grenade1",
	mass = 3,
	animation_attack = "throw grenade",
	animation_charge = "charge grenade",
	animation_hold = "hold grenade",
	categories = {"explosive", "throwable", "weapon"},
	crafting_materials = {["crimson stone"] = 1, ["iron ingot"] = 1},
	effect_craft = "craftmetal1",
	influences_base = {fire = -15, physical = -5},
	influences_bonus = {intelligence = 0.01},
	destroy_actions = {"explode"},
	destroy_timer = 3,
	equipment_slot = "hand.R",
	health = 1,
	mass = 5,
	stacking = true}

Itemspec{
	name = "crimson hatchet",
	model = "axe1",
	icon = "axe2",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["crimson stone"] = 3},
	damage_mining = 1,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 500,
	influences_base = {fire = -4, hatchet = -7, physical = -4},
	influences_bonus = {strength = 0.01},
	mass = 15}

Itemspec{
	name = "crimson mattock",
	model = "mattock-000",
	icon = "mattock1",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"mattock", "melee", "weapon"},
	description = "A high quality mattock used to mine ores",
	crafting_materials = {["crimson stone"] = 9},
	damage_mining = 1,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 500,
	influences_base = {fire = -4, physical = -4},
	influences_bonus = {strength = 0.01},
	mass = 15}

Itemspec{
	name = "crimson musket",
	model = "musket1",
	icon = "musket1",
	ammo_type = "bullet",
	animation_attack = "attack musket",
	animation_charge = "charge musket",
	animation_hold = "hold musket",
	categories = {"ranged", "weapon"},
	crafting_materials = {["crimson stone"] = 15, ["log"] = 2},
	effect_attack = "musket1",
	effect_craft = "craftmetal1",
	influences_base = {fire = -8, physical = -8},
	influences_bonus = {intelligence = 0.005, perception = 0.005},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	mass = 15}

Itemspec{
	name = "crimson revolver",
	model = "revolver1",
	icon = "revolver1",
	ammo_type = "bullet",
	animation_attack = "attack revolver",
	animation_charge = "charge revolver",
	animation_hold = "hold revolver",
	categories = {"ranged", "weapon"},
	crafting_materials = {["crimson stone"] = 10, ["log"] = 1},
	effect_attack = "revolver1",
	influences_base = {fire = -7, physical = -7},
	influences_bonus = {intelligence = 0.005, perception = 0.005},
	equipment_slot = "hand.R",
	mass = 8}

Itemspec{
	name = "crimson sword",
	model = "sword5",
	icon = "sword2",
	animation_attack = "attack sword",
	animation_charge = "charge sword",
	animation_hold = "hold sword",
	categories = {"melee", "weapon"},
	crafting_materials = {["crimson stone"] = 3},
	damage_mining = 1,
	effect_attack_speedline = true,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 500,
	influences_base = {fire = -8, physical = -8},
	influences_bonus = {dexterity = 0.005, strength = 0.005},
	mass = 15}

Itemspec{
	name = "iron crossbow",
	model = "crossbow1",
	icon = "crossbow1",
	ammo_type = "arrow",
	animation_attack = "attack crossbow",
	animation_charge = "charge crossbow",
	animation_hold = "hold crossbow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["iron ingot"] = 3},
	effect_attack = "crossbow1",
	effect_craft = "craftmetal1",
	influences_base = {physical = -8},
	influences_bonus = {dexterity = 0.005},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	mass = 15}

Itemspec{
	name = "iron dagger",
	model = "dagger1", -- FIXME
	icon = "dagger1",
	categories = {"melee", "weapon"},
	crafting_materials = {["iron ingot"] = 1},
	damage_mining = 1,
	effect_attack_speedline = true,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 220,
	influences_base = {physical = -5},
	influences_bonus = {dexterity = 0.01},
	mass = 5}

Itemspec{
	name = "iron hatchet",
	model = "axe1",
	icon = "axe2",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["iron ingot"] = 2},
	damage_mining = 1,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 220,
	influences_base = {hatchet = -5, physical = -5},
	influences_bonus = {strength = 0.01},
	mass = 10}

Itemspec{
	name = "iron mattock",
	model = "mattock-000",
	icon = "mattock1",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["iron ingot"] = 3},
	description = "A medium quality mattock used to mine ores",
	damage_mining = 1,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 220,
	influences_base = {physical = -5},
	influences_bonus = {strength = 0.01},
	mass = 15}

Itemspec{
	name = "iron musket",
	model = "musket1",
	icon = "musket1",
	ammo_type = "bullet",
	animation_attack = "attack musket",
	animation_charge = "charge musket",
	animation_hold = "hold musket",
	categories = {"ranged", "weapon"},
	crafting_materials = {["iron ingot"] = 5, ["log"] = 2},
	effect_attack = "musket1",
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	influences_base = {physical = -13},
	influences_bonus = {intelligence = 0.005, perception = 0.005},
	mass = 20}

Itemspec{
	name = "iron revolver",
	model = "revolver1",
	icon = "revolver1",
	ammo_type = "bullet",
	animation_attack = "attack revolver",
	animation_charge = "charge revolver",
	animation_hold = "hold revolver",
	categories = {"ranged", "weapon"},
	crafting_materials = {["iron ingot"] = 10, ["log"] = 1},
	effect_attack = "revolver1",
	influences_base = {physical = -10},
	influences_bonus = {intelligence = 0.005, perception = 0.005},
	equipment_slot = "hand.R",
	mass = 8}

Itemspec{
	name = "iron sword",
	model = "sword1",
	icon = "sword2",
	animation_attack = "attack sword",
	animation_charge = "charge sword",
	animation_hold = "hold sword",
	categories = {"melee", "weapon"},
	crafting_materials = {["iron ingot"] = 3},
	damage_mining = 1,
	effect_attack_speedline = true,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 220,
	influences_base = {physical = -5},
	influences_bonus = {dexterity = 0.005, strength = 0.005},
	mass = 15}

Itemspec{
	name = "security robot arm",
	model = "musket1",
	icon = "musket1",
	ammo_type = "bullet",
	categories = {"ranged", "weapon"},
	effect_attack = "musket1",
	influences_base = {physical = -30},
	equipment_slot = "robotarm.R",
	mass = 15}

Itemspec{
	name = "security turret barrel",
	model = "musket1",
	icon = "musket1",
	ammo_type = "bullet",
	categories = {"ranged", "weapon"},
	effect_attack = "musket1",
	influences_base = {physical = -15},
	equipment_slot = "turretbarrel",
	mass = 15}

Itemspec{
	name = "stone dagger",
	model = "dagger1", -- FIXME
	icon = "dagger1",
	categories = {"melee", "weapon"},
	crafting_materials = {["granite stone"] = 2},
	damage_mining = 1,
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	influences_base = {physical = -3},
	influences_bonus = {dexterity = 0.005},
	health = 130,
	mass = 6}

Itemspec{
	name = "stone hatchet",
	model = "stoneaxe1",
	icon = "axe1",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["granite stone"] = 3},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 130,
	influences_base = {hatchet = -3, physical = -3},
	influences_bonus = {strength = 0.01},
	mass = 9}

Itemspec{
	name = "stone mattock",
	model = "mattock-000",
	icon = "mattock1",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["granite stone"] = 4},
	description = "A low quality mattock used to mine ores",
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 130,
	influences_base = {physical = -3},
	influences_bonus = {strength = 0.01},
	mass = 12}

Itemspec{
	name = "stone sword",
	model = "sword1",
	icon = "sword1",
	animation_attack = "attack sword",
	animation_charge = "charge sword",
	animation_hold = "hold sword",
	categories = {"melee", "weapon"},
	crafting_materials = {["granite stone"] = 5},
	damage_mining = 1,
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	health = 130,
	influences_base = {physical = -5},
	influences_bonus = {dexterity = 0.002, strength = 0.005},
	mass = 15}

Itemspec{
	name = "stone's sword",
	model = "sword5",
	icon = "sword1",
	animation_attack = "attack sword",
	animation_charge = "charge sword",
	animation_hold = "hold sword",
	categories = {"melee", "weapon"},
	damage_mining = 1,
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	health = 130,
	influences_base = {physical = -5},
	influences_bonus = {dexterity = 0.002, strength = 0.005},
	mass = 15}

Itemspec{
	name = "wooden bow",
	model = "bow1",
	icon = "bow1",
	ammo_type = "arrow",
	animation_attack = "attack bow",
	animation_charge = "charge bow",
	animation_hold = "hold bow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["log"] = 5},
	effect_craft = "craftwood1",
	influences_base = {physical = -4},
	influences_bonus = {dexterity = 0.01},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	water_gravity = Vector(0,6,0),
	mass = 7}

Itemspec{
	name = "fire bow",
	model = "bow1", -- FIXME
	icon = "bow2",
	ammo_type = "fire arrow",
	animation_attack = "attack bow",
	animation_charge = "charge bow",
	animation_hold = "hold bow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["log"] = 5},
	effect_craft = "craftwood1",
	influences_base = {physical = -4},
	influences_bonus = {dexterity = 0.01},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	water_gravity = Vector(0,6,0),
	mass = 7}

Itemspec{
	name = "explosive bow",
	model = "bow1", -- FIXME
	icon = "bow2",
	ammo_type = "explosive arrow",
	animation_attack = "attack bow",
	animation_charge = "charge bow",
	animation_hold = "hold bow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["log"] = 5},
	effect_craft = "craftwood1",
	influences_base = {physical = -4},
	influences_bonus = {dexterity = 0.01},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	water_gravity = Vector(0,6,0),
	mass = 7}

Itemspec{
	name = "wooden crossbow",
	model = "crossbow1",
	icon = "crossbow1",
	ammo_type = "arrow",
	animation_attack = "attack crossbow",
	animation_charge = "charge crossbow",
	animation_hold = "hold crossbow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["log"] = 5},
	effect_attack = "crossbow1",
	effect_craft = "craftwood1",
	influences_base = {physical = -4},
	influences_bonus = {dexterity = 0.01},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	water_gravity = Vector(0,6,0),
	mass = 7}

Itemspec{
	name = "wooden dagger",
	model = "woodshortsword", -- FIXME
	icon = "dagger1",
	categories = {"melee", "weapon"},
	crafting_materials = {["log"] = 2},
	damage_mining = 1,
	effect_attack_speedline = true,
	effect_craft = "craftwood1",
	equipment_slot = "hand.R",
	health = 100,
	influences_base = {physical = -3},
	influences_bonus = {dexterity = 0.01},
	mass = 7,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "wooden hatchet",
	model = "woodhatchet",
	icon = "axe1",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["log"] = 3},
	damage_mining = 1,
	effect_attack_speedline = true,
	effect_craft = "craftwood1",
	equipment_slot = "hand.R",
	health = 100,
	influences_base = {hatchet = -3, physical = -3},
	influences_bonus = {strength = 0.01},
	mass = 7,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "wooden mattock",
	model = "mattock-000",
	icon = "mattock1",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["log"] = 4},
	damage_mining = 1,
	description = "A low quality mattock used to mine ores",
	effect_craft = "craftwood1",
	equipment_slot = "hand.R",
	health = 100,
	influences_base = {physical = -3},
	influences_bonus = {strength = 0.01},
	mass = 7,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "wooden sword",
	model = "woodsword",
	icon = "sword1",
	animation_attack = "attack sword",
	animation_charge = "charge sword",
	animation_hold = "hold sword",
	categories = {"melee", "weapon"},
	crafting_materials = {["log"] = 5},
	damage_mining = 1,
	effect_attack_speedline = true,
	effect_craft = "craftwood1",
	equipment_slot = "hand.R",
	health = 100,
	influences_base = {physical = -5},
	influences_bonus = {dexterity = 0.005, strength = 0.005},
	mass = 7,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "wooden club",
	model = "woodenclub1",
	icon = "mace1",
	animation_attack = "attack blunt",
	animation_charge = "charge blunt",
	animation_hold = "hold blunt",
	categories = {"melee", "weapon"},
	crafting_materials = {["log"] = 4},
	damage_mining = 1,
	effect_craft = "craftwood1",
	equipment_slot = "hand.R",
	health = 100,
	influences_base = {physical = -4},
	influences_bonus = {dexterity = 0.005, strength = 0.005},
	mass = 7,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "wooden staff",
	model = "spear1",
	icon = "staff1",
	animation_attack = "attack staff",
	animation_charge = "charge staff",
	animation_hold = "hold staff",
	categories = {"melee", "weapon"},
	crafting_materials = {["log"] = 5},
	damage_mining = 1,
	effect_craft = "craftwood1",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	health = 100,
	influences_base = {physical = -4},
	influences_bonus = {dexterity = 0.005, strength = 0.005},
	mass = 7,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "luminous staff",
	model = "staff1",
	icon = "staff2",
	animation_attack = "attack staff",
	animation_charge = "charge staff",
	animation_hold = "hold staff",
	categories = {"melee", "weapon"},
	crafting_materials = {["log"] = 5, ["crimson stone"] = 2},
	damage_mining = 1,
	effect_craft = "craftwood1",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	health = 300,
	influences_base = {fire = -2, physical = -4},
	influences_bonus = {perception = 0.005, willpower = 0.005},
	mass = 7,
	special_effects = {
		{node = "#flame", model = "torchfx1"},
		{node = "#flame2", model = "torchfx1"}},
	water_gravity = Vector(0,6,0)}

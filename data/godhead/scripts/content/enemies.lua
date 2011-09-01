require "content/feats"
require "content/species"

Species{
	name = "brigand",
	base = "aer",
	categories = {"enemy"},
	difficulty = 0.5,
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"iron sword", "leather top", "leather pants", "leather leggings"},
	loot_categories = {"armor", "material", "potion", "shield", "weapon"},
	loot_count = {2, 4}}

Species{
	name = "brigand king",
	base = "aer",
	categories = {"special"},
	difficulty = 1,
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	marker = "brigandking",
	inventory_items = {"adamantium sword", "iron breastplate", "iron greaves", "iron gauntlets"},
	loot_categories = {"armor", "shield", "weapon"},
	loot_count = {5, 10},
	skills = {
		{name = "dexterity", max = 80, val = 30},
		{name = "health", max = 100, val = 100},
		{name = "intelligence", max = 100, val = 0},
		{name = "perception", max = 60, val = 20},
		{name = "strength", max = 40, val = 40},
		{name = "willpower", max = 80, val = 0}}}

Species{
	name = "novice brigand archer",
	base = "aer",
	categories = {"enemy"},
	difficulty = 0.3,
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"wooden crossbow", "leather top", "leather pants", "leather leggings", ["arrow"] = 20},
	loot_categories = {"armor", "potion", "weapon"},
	loot_count = {2, 4},
	skills = {
		{name = "dexterity", max = 80, val = 30},
		{name = "health", max = 100, val = 30},
		{name = "intelligence", max = 100, val = 5},
		{name = "perception", max = 60, val = 30},
		{name = "strength", max = 40, val = 5},
		{name = "willpower", max = 80, val = 5}}}

Species{
	name = "master brigand archer",
	base = "aer",
	categories = {"enemy"},
	difficulty = 0.7,
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"iron crossbow", "iron breastplate", "iron greaves", "iron gauntlets", ["arrow"] = 40},
	loot_categories = {"armor", "potion", "weapon"},
	loot_count = {2, 4},
	skills = {
		{name = "dexterity", max = 80, val = 80},
		{name = "health", max = 100, val = 60},
		{name = "intelligence", max = 100, val = 5},
		{name = "perception", max = 60, val = 40},
		{name = "strength", max = 40, val = 5},
		{name = "willpower", max = 80, val = 5}}}

Species{
	name = "suicide bomber",
	base = "aer",
	categories = {"enemy"},
	difficulty = 0.5,
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"leather top", "leather pants", ["crimson grenade"] = 15},
	loot_categories = {"armor", "potion", "weapon"},
	loot_count = {2, 4},
	skills = {
		{name = "dexterity", max = 80, val = 5},
		{name = "health", max = 100, val = 100},
		{name = "intelligence", max = 100, val = 40},
		{name = "perception", max = 60, val = 40},
		{name = "strength", max = 40, val = 5},
		{name = "willpower", max = 80, val = 5}}}

Species{
	name = "brigandmale",
	base = "aermale",
	categories = {"enemy"},
	difficulty = 0.5,
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"leather brigand chest", "leather brigand pants", "leather boots", "leather gloves", "leather helm"},
	loot_categories = {"armor", "material", "potion", "shield", "weapon"},
	loot_count = {2, 4}}

Species{
	name = "troglodyte female",
	base = "race",
	ai_offense_factor = 0.9,
	animations = {
		["attack axe"] = {animation = "punch", channel = 2, fade_in = 0.1, weight = 100},
		["attack blunt"] = {animation = "punch", channel = 2, fade_in = 0.1, weight = 100},
		["attack sword"] = {animation = "punch", channel = 2, weight = 100},
		["attack staff"] = {animation = "punch", channel = 2, weight = 100},
		["hold axe"] = {animation = "hold-right", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold blunt"] = {animation = "hold-right", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold bulky"] = {animation = "hold-right", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold staff"] = {animation = "hold-right", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold shield"] = {animation = "hold-left", channel = 4, permanent = true, repeat_start = 10, weight = 5},
		["hold sword"] = {animation = "hold-right", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold torch"] = {animation = "hold-left", channel = 4, permanent = true, repeat_start = 10, weight = 5}},
	categories = {"enemy"},
	difficulty = 0.25,
	effect_physical_damage = "fleshimpact1",
	eye_style = "random",
	hair_style = "random",
	factions = {"evil"},
	inventory_items = {"wooden club", "trog rags"},
	loot_categories = {"material"},
	loot_count = {2, 4},
	model = "trogfem1",
	models = {
		skeleton = "trogfem1",
		arms = "trogfemarmnude1",
		head = "trogfemhead1",
		legs = "trogfemlegnude1",
		lower = "trogfemlowernude1",
		upper = "trogfemuppernude1"},
	skills = {
		{name = "dexterity", max = 15, val = 15},
		{name = "health", max = 30, val = 30},
		{name = "perception", max = 30, val = 30},
		{name = "strength", max = 15, val = 15}},
	hair_styles = {	
		{"troglong", "trogfemlonghair1"},
		{"trogshort", "trogfemshorthair1"}},
	skin_styles = {
		{""}}}

Species{
	name = "bloodworm",
	base = "base",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_enable_jump = false,
	ai_offense_factor = 1,
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	categories = {"enemy"},
	difficulty = 0,
	effect_physical_damage = "fleshimpact1",
	factions = {"evil"},
	feat_anims = {"bite"},
	feat_effects = {"strength to physical damage"},
	inventory_size = 3,
	inventory_items = {["hide"] = 2},
	jump_force = 3,
	model = "bloodworm2",
	speed_walk = 2,
	skills = {
		{name = "health", max = 30, val = 30},
		{name = "strength", max = 5, val = 5}}}


Species{
	name = "dragon",
	base = "base",
	model = "dragon1",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_offense_factor = 1,
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30},
		["attack left claw"] = {animation = "claw-left", channel = 2, weight = 30},
		["attack right claw"] = {animation = "claw-right", channel = 2, weight = 30},
		["spell ranged"] = {animation = "breathe-fire", channel = 2, weight = 30}},
	difficulty = 1,
	effect_physical_damage = "fleshimpact1",
	factions = {"evil"},
	feat_anims = {"dragon bite", "left claw", "right claw"},--, "ranged spell"},
	feat_effects = {"fire damage", "strength to physical damage"},
	inventory_size = 10,
	inventory_items = {["dragon scale"] = 5, ["adamantium"] = 4, ["crimson stone"] = 2},
	mass = 1000,
	speed_walk = 1,
	skills = {
		{name = "health", max = 100, val = 100},
		{name = "strength", max = 100, val = 100},
		{name = "willpower", max = 50, val = 50}},
	vulnerabilities = {cold = 2, fire = 0.3}}

Species{
	name = "feral devora",
	base = "devora",
	ai_offense_factor = 0.8,
	categories = {"enemy"},
	difficulty = 0.5,
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"dress"},
	loot_categories = {"material"},
	loot_count = {2, 4}}

Species{
	name = "lizardman",
	base = "base",
	model = "lizardman2",
	ai_enable_block = false,
	ai_offense_factor = 0.9,
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	categories = {"enemy"},
	difficulty = 0.3,
	effect_physical_damage = "fleshimpact1",
	factions = {"evil"},
	feat_anims = {"right hand","bite"},
	feat_effects = {"strength to physical damage"},
	inventory_size = 5,
	inventory_items = {"wooden club", ["lizard scale"] = 3},
	loot_categories = {"material"},
	loot_count = {2, 4},
	mass = 100,
	speed_walk = 3,
	equipment_slots = {
		{name = "hand.L", node = "#hand.L"},
		{name = "hand.R", node = "#hand.R"}},
	skills = {
		{name = "dexterity", max = 5, val = 5},
		{name = "health", max = 30, val = 30},
		{name = "strength", max = 15, val = 15}}}

Species{
	name = "small rat",
	base = "base",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_offense_factor = .1,
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	-- categories = {"enemy"},
	difficulty = 0,
	effect_physical_damage = "fleshimpact1",
	factions = {"neutral"},
	feat_anims = {"bite"},
	feat_effects = {"strength to physical damage"},
	inventory_size = 3,
	inventory_items = {"hide"},
	jump_force = 6,
	model = "smallrat1",
	speed_walk = 4,
	skills = {
		{name = "strength", max = 1, val = 1},
		{name = "health", max = 5, val = 5}}}
Species{
	name = "rat",
	base = "base",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_offense_factor = 1,
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	categories = {"enemy"},
	difficulty = 0,
	effect_physical_damage = "fleshimpact1",
	factions = {"evil"},
	feat_anims = {"bite"},
	feat_effects = {"strength to physical damage"},
	inventory_size = 3,
	inventory_items = {["hide"] = 2},
	jump_force = 6,
	model = "plaguerat1", -- FIXME
	speed_walk = 4,
	skills = {
		{name = "strength", max = 5, val = 5},
		{name = "health", max = 10, val = 10}}}

Species{
	name = "plague rat",
	base = "base",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_offense_factor = 1,
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	categories = {"plague"},
	difficulty = 0.7,
	effect_physical_damage = "fleshimpact1",
	factions = {"evil"},
	feat_anims = {"bite"},
	feat_effects = {"strength to physical damage"},
	inventory_size = 3,
	inventory_items = {["hide"] = 2},
	jump_force = 6,
	model = "plaguerat1",
	speed_walk = 4,
	skills = {
		{name = "health", max = 30, val = 30},
		{name = "strength", max = 5, val = 5}}}

Species{
	name = "slime",
	base = "base",
	ai_enable_block = false,
	ai_offense_factor = 0.5,
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	categories = {"random"},
	difficulty = 0,
	factions = {"evil"},
	feat_anims = {"bite"},
	feat_effects = {"strength to physical damage"},
	inventory_size = 0,
	jump_force = 4,
	model = "slime2",
	speed_walk = 2,
	skills = {
		{name = "health", max = 10, val = 10}},
	vulnerabilities = {cold = 1.5, fire = 1.5, physical = 0.6}}

Species{
	name = "plague slime",
	base = "base",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_offense_factor = 1,
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	categories = {"plague"},
	difficulty = 0.7,
	factions = {"evil"},
	feat_anims = {"bite"},
	feat_effects = {"strength to physical damage"},
	inventory_size = 0,
	jump_force = 4,
	model = "slime2",
	speed_walk = 2,
	skills = {
		{name = "health", max = 30, val = 30}},
	vulnerabilities = {cold = 1.5, fire = 1.5, physical = 0.6}}

Species{
	name = "stone imp",
	base = "base",
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	ai_enable_block = false,
	ai_enable_strafe = false,
	ai_enable_weapon_switch = false,
	ai_offense_factor = 0.8,
	categories = {"enemy", "mining"},
	difficulty = 0,
	factions = {"evil"},
	feat_anims = {"bite"},
	feat_effects = {"strength to physical damage"},
	equipment_slots = {
		{name = "hand.R", node = "#hand.R"},
		{name = "hand.L", node = "#hand.L"}},
	inventory_items = {"impstone"},
	inventory_size = 2,
	jump_force = 4,
	model = "stoneimp2",
	speed_walk = 1,
	skills = {
		{name = "health", max = 20, val = 20},
		{name = "strength", max = 10, val = 10}},
	vulnerabilities = {cold = 1.1, fire = 1.1, physical = 0.8}}

Species{
	name = "fire imp",
	base = "base",
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	ai_enable_block = false,
	ai_enable_strafe = false,
	ai_enable_weapon_switch = false,
	ai_offense_factor = 0.8,
	categories = {"enemy", "mining"},
	difficulty = 0.2,
	equipment_slots = {
		{name = "hand.R", node = "#hand.R"},
		{name = "hand.L", node = "#hand.L"}},
	factions = {"evil"},
	feat_anims = {"bite", "explode"},
	feat_effects = {"fire damage", "strength to physical damage"},
	jump_force = 4,
	model = "stoneimp2",
	speed_walk = 1,
	inventory_items = {"firestone"},
	inventory_size = 2,
	skills = {
		{name = "health", max = 20, val = 20},
		{name = "strength", max = 10, val = 10}},
	vulnerabilities = {cold = 2, fire = 0.4, physical = 0.8}}

Species{
	name = "bitingbark",
	base = "base",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_enable_jump = false,
	ai_enable_weapon_switch = false,
	ai_offense_factor = 1,
	animations = {
		["attack left claw"] = {animation = "claw-left", channel = 2, weight = 30},
		["attack right claw"] = {animation = "claw-right", channel = 2, weight = 30}},
	categories = {"enemy"},
	difficulty = 0,
	effect_physical_damage = "fleshimpact1",
	factions = {"evil"},
	feat_anims = {"left claw", "right claw"},
	feat_effects = {"strength to physical damage"},
	inventory_size = 3,
	inventory_items = {["log"] = 2},
	jump_force = 0,
	model = "bitingbark1",
	speed_walk = 1,
	skills = {
		{name = "health", max = 30, val = 30},
		{name = "strength", max = 5, val = 5}}}

require "content/factions"

------------------------------------------------------------------------------
-- Base races.

Species{
	name = "base",
	animations = {
		["attack back"] = {animation = "attack-back", channel = 2, fade_in = 0.1, weight = 100, node_weights = {LOWER = 1}},
		["attack bow"] = {animation = "attack-bow", channel = 2, weight = 100, node_weights = {LOWER = 1}},
		["attack crossbow"] = {animation = "attack-crossbow", channel = 2, weight = 5000, node_weights = {LOWER = 1}},
		["attack front"] = {animation = "attack-front", channel = 2, fade_in = 0.1, weight = 100, node_weights = {LOWER = 1}},
		["attack left"] = {animation = "attack-left", channel = 2, weight = 100, node_weights = {LOWER = 1}},
		["attack right"] = {animation = "attack-right", channel = 2, weight = 100, node_weights = {LOWER = 1}},
		["attack punch"] = {animation = "attack-punch", channel = 2, fade_out = 0.1, weight = 100, node_weights = {LOWER = 1}},
		["attack stand"] = {animation = "attack-stand", channel = 2, fade_in = 0.1, weight = 100, node_weights = {LOWER = 1}},
		["attack musket"] = {animation = "attack-musket", channel = 2, weight = 5000, node_weights = {LOWER = 1}},
		["attack revolver"] = {animation = "attack-revolver", channel = 2, weight = 5000, node_weights = {LOWER = 1}},
		["block start"] = {animation = "block", channel = 2, fade_in = 0.6, fade_out = 0.6, permanent = true, repeat_start = 10, weight = 50},
		["block stop"] = {animation = "block-stop", channel = 2},
		["build"] = {animation = "punch", channel = 2, weight = 30},
		["charge axe"] = {animation = "charge-melee", channel = 2, fade_in = 0.1, permanent = true, repeat_start = 10, weight = 100, node_weights = {LOWER = 1}},
		["charge blunt"] = {animation = "charge-melee", channel = 2, fade_in = 0.1, permanent = true, repeat_start = 10, weight = 100, node_weights = {LOWER = 1}},
		["charge bow"] = {animation = "charge-bow", channel = 2, permanent = true, repeat_start = 10, weight = 100, node_weights = {LOWER = 1}},
		["charge cancel"] = {channel = 2},
		["charge crossbow"] = {animation = "charge-crossbow", channel = 2, permanent = true, repeat_start = 10, weight = 5000, node_weights = {LOWER = 1}},
		["charge grenade"] = {animation = "charge-throw", channel = 2, permanent = true, repeat_start = 10, weight = 100, node_weights = {LOWER = 1}},
		["charge musket"] = {animation = "charge-musket", channel = 2, permanent = true, repeat_start = 10, weight = 5000, node_weights = {LOWER = 1}},
		["charge punch"] = {animation = "charge-punch", channel = 2, fade_out = 0.1, permanent = true, repeat_start = 10, weight = 100, node_weights = {LOWER = 1, ARML = 1}},
		["charge revolver"] = {animation = "charge-revolver", channel = 2, permanent = true, repeat_start = 10, weight = 5000, node_weights = {LOWER = 1}},
		["charge sword"] = {animation = "charge-melee", channel = 2, permanent = true, repeat_start = 10, weight = 100, node_weights = {LOWER = 1}},
		["charge staff"] = {animation = "charge-melee", channel = 2, permanent = true, repeat_start = 10, weight = 100, node_weights = {LOWER = 1}},
		["climb high"] = {animation = "climb-high", channel = 5, weight = 30},
		["climb low"] = {animation = "climb-low", channel = 5, weight = 30},
		["hang"] = {animation = "hang", channel = 2, weight = 30},
		["hold axe"] = {animation = "hold-axe", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold blunt"] = {animation = "hold-blunt", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold bow"] = {animation = "hold-right", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold bulky"] = {animation = "hold-bulky", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold crossbow"] = {animation = "hold-crossbow", channel = 3, permanent = true, repeat_start = 10, weight = 50},
		["hold musket"] = {animation = "hold-musket", channel = 3, permanent = true, repeat_start = 10, weight = 50},
		["hold revolver"] = {animation = "hold-revolver", channel = 3, permanent = true, repeat_start = 10, weight = 50},
		["hold staff"] = {animation = "hold-staff", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold shield"] = {animation = "hold-left", channel = 4, permanent = true, repeat_start = 10, weight = 5},
		["hold sword"] = {animation = "hold-blunt", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold torch"] = {animation = "hold-left", channel = 4, permanent = true, repeat_start = 10, weight = 5},
		["dead"] = {animation = "death", channel = 1, permanent = true, repeat_start = 10, time = 10},
		["death"] = {animation = "death", channel = 1, permanent = true, repeat_start = 10},
		["drop"] = {animation = "pickup", channel = 2, weight = 10},
		["idle"] = {animation = "idle", channel = 1, permanent = true},
		["jump"] = {animation = "jump", channel = 5, fade_out = 0.4, permanent = true, repeat_start = 10, weight = 10, node_weights = {LOWER = 1000}},
		["land ground"] = {animation = "land", channel = 5, fade_in = 0.1, fade_out = 0.5, weight = 10, node_weights = {LOWER = 1000}},
		["land water"] = {animation = "land", channel = 5, node_weights = {LOWER = 1000}},
		["pick up"] = {animation = "pickup", channel = 2, weight = 10},
		["run"] = {animation = "run", channel = 1, permanent = true, node_weights = {LOWER = 100}},
		["run left"] = {animation = "run-left", channel = 1, permanent = true, node_weights = {LOWER = 100}},
		["run right"] = {animation = "run-right", channel = 1, permanent = true, node_weights = {LOWER = 100}},
		["spell ranged"] = {animation = "cast-spell", channel = 2, weight = 30},
		["spell self"] = {animation = "spell-self", channel = 2, weight = 30},
		["spell touch"] = {animation = "cast-spell", channel = 2, weight = 30},
		["strafe left"] = {animation = "strafe-left", channel = 1, permanent = true, node_weights = {LOWER = 100}},
		["strafe right"] = {animation = "strafe-right", channel = 1, permanent = true, node_weights = {LOWER = 100}},
		["throw"] = {animation = "throw", channel = 2, weight = 10},
		["throw bulky"] = {animation = "throw-bulky", channel = 2, weight = 10},
		["throw grenade"] = {animation = "throw", channel = 2, weight = 10},
		["walk"] = {animation = "walk", channel = 1, permanent = true, node_weights = {LOWER = 100}},
		["walk back"] = {animation = "walk-back", channel = 1, permanent = true, node_weights = {LOWER = 100}}}}

Species{
	name = "race",
	base = "base",
	aim_ray_center = Vector(0, 1.5, 0),
	aim_ray_end = 5,
	aim_ray_start = 0.5,
	camera_center = Vector(0, 1.5, 0),
	effect_physical_damage = "fleshimpact1",
	equipment_slots = {
		{name = "arms"},
		{name = "feet"},
		{name = "hand.L", node = "#hand.L"},
		{name = "hand.R", node = "#hand.R"},
		{name = "head"},
		{name = "lowerbody"},
		{name = "upperbody"}},
	factions = {"good"},
	feat_anims = {"build", "right hand", "ranged", "ranged spell", "spell on self", "spell on touch", "throw"},
	feat_effects = {"home","follow","berserk", "black haze", "dig", "fire damage", "firewall", "light", "mindless march", "magic missile", "physical damage", "restore health", "sanctuary", "travel", "wilderness"},
	footstep_height = 0.3,
	footstep_sound = "step1",
	inventory_size = 20,
	skill_quota = 200,
	speed_walk = 2,
	speed_run = 6,
	tilt_bone = "backtilt",
	tilt_limit = 1.25,
	timing_build = 0.2,
	timing_pickup = 0.2,
	-- Customization.
	body_scale = {0.9, 1.05},
	eye_styles = {
		{"Default", ""}},
	hair_styles = {
		{"Bald", ""},
		{"Pigtails", "hair1"},
		{"Pigtails long", "hair3"},
		{"Long", "hair2"},
		--{"Short", "aershorthair1"} -- TODO: Adapt to head mesh changes
		{"Drill", "hair4"},
		{"Short", "hair5"}},
	skin_styles = {
		{"Default", ""}}}

Species{
	name = "aer",
	base = "race",
	animations = {
		["land ground"] = {animation = "land", channel = 5, fade_in = 0.1, fade_out = 0.5, weight = 10, node_weights = {LOWER = 1000}},
		["spell self"] = {animation = "spell-self", channel = 2, weight = 30}},
	effect_falling_damage = "aerhurt1",
	equipment_class = "aer",
	model = "aer1",
	models = {
		skeleton = "aer1",
		arms = "aerarm1",
		eyes = "aereye1",
		head = "aerhead1",
		hands = "aerhand1",
		feet = "aerfoot1",
		legs = "aerleg1",
		lower = "aerlower1",
		lower_safe = "aerbloomers1",
		upper = "aerupper1",
		upper_safe = "aerbloomerstop1"},
	skills = {
		{name = "dexterity", max = 80, val = 30},
		{name = "health", max = 100, val = 50},
		{name = "intelligence", max = 100, val = 50},
		{name = "perception", max = 60, val = 20},
		{name = "strength", max = 40, val = 20},
		{name = "willpower", max = 80, val = 30}},
	tilt_bone = {"back1", "back2", "back3"}}


------------------------------------------------------------------------------
-- Playable races.

Species{
	name = "aer-player",
	base = "aer",
	ai_enabled = false,
	models = {
		skeleton = "aer1",
		arms = "aerarm1",
		eyes = "aereye1",
		head = "aerhead1",
		hands = "aerhand1",
		feet = "aerfoot1",
		legs = "aerleg1",
		lower = "aerlower1",
		lower_safe = "aerbloomers1",
		upper = "aerupper1",
		upper_safe = "aerbloomerstop1"},
	inventory_items = {"torch", "workbench", "iron breastplate", "iron greaves"}}


------------------------------------------------------------------------------
-- Friendly creatures.


Species{
	name = "lipscitizen",
	base = "aer",
	dialog = "lips citizen",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"leather top", "leather pants", "leather leggings"}}


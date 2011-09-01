Obstaclespec = Class(Spec)
Obstaclespec.type = "obstacle"
Obstaclespec.dict_id = {}
Obstaclespec.dict_cat = {}
Obstaclespec.dict_name = {}

--- Creates a new obstacle specification.
-- @param clss Obstaclespec class.
-- @param args Arguments.<ul>
--   <li>collision_group: Collision group.</li>
--   <li>collision_mask: Collision mask.</li>
--   <li>categories: List of categories to which the obstacle belongs.</li>
--   <li>harvest_behavior: Harvest behavior: "keep"/"destroy".</li>
--   <li>harvest_effect: Effect to play when harvested.</li>
--   <li>harvest_materials: Dictionary of harvestable materials.</li>
--   <li>destroy_actions: List of actions to perform when the obstacle is destroyed.</li>
--   <li>destroy_items: List of items to spawn when the obstacle is destroyed.</li>
--   <li>health: Number of hit points the obstacle has.</li>
--   <li>interactive: False to make the object not appear interactive.</li>
--   <li>mass: Mass in kilograms.</li>
--   <li>model: Model to use for the obstacle.</li>
--   <li>name: Name of the obstacle type.</li>
--   <li>physics: Physics mode.</li>
--   <li>special_effects: List of special effects to render. (see itemspec for details)</li></ul>
-- @return New item specification.
Obstaclespec.new = function(clss, args)
	local self = Spec.new(clss, args)
	local copy = function(f, d) if self[f] == nil then self[f] = d end end
	self.harvest_enabled = args.harvest_materials ~= nil
	copy("collision_group", 0x8000)
	copy("collision_mask", 0xFF)
	copy("constraints", {})
	copy("harvest_behavior", "keep")
	copy("harvest_materials", {})
	copy("destroy_actions", {})
	copy("destroy_items", {})
	copy("gravity", Vector(0, -15, 0))
	copy("interactive", true)
	copy("mass", 10)
	copy("physics", "static")
	copy("special_effects", {})
	return self
end

--- Returns a random obstacle spec.
-- @param clss Obstaclespec class.
-- @param args Arguments.<ul>
--   <li>category: Category name.</li></ul>
-- @return Obstaclespec or nil.
Obstaclespec.random = function(clss, args)
	if args and args.category then
		local cat = clss.dict_cat[args.category]
		if not cat then return end
		return cat[math.random(#cat)]
	end
	if args and args.name then
		return clss.dict_name[args.name]
	end
	return clss.dict_id[math.random(#clss.dict_id)]
end

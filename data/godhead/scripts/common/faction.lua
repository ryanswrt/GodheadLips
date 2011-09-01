require "common/spec"

Faction = Class(Spec)
Faction.type = "faction"
Faction.dict_id = {}
Faction.dict_cat = {}
Faction.dict_name = {}

--- Creates a new faction.
-- @param clss Faction class.
-- @param args Arguments.<ul>
--   <li>categories: List of categories or nil.</li>
--   <li>name: Faction name.</li>
--   <li>enemies: List of enemy faction names or nil.</li></ul>
-- @return Faction or nil.
Faction.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.enemies = {}
	if args.enemies then
		for k,v in pairs(args.enemies) do
			self.enemies[v] = true
		end
	end
	return self
end

--- Adds an enemy faction.
-- @param self Faction.
-- @param args Arguments.<ul>
--   <li>name: Faction name.</li></ul>
Faction.add_enemy = function(self, args)
	self.enemies[args.name] = true
end

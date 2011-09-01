-- Dig terrain.
Influencespec{name = "dig", func = function(feat, info, args, value)
	if not args.tile then return end
	Voxel:damage(args.attacker, args.tile)
end}

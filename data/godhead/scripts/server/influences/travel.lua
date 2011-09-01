-- Teleport to Lips.
Influencespec{name = "travel", func = function(feat, info, args, value)
	if not args.target then return end
	args.target:teleport{region = "Lips"}
end}

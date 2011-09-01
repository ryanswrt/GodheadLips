-- Increase light duration.
Influencespec{name = "light", func = function(feat, info, args, value)
	if not args.target then return end
	args.target:inflict_modifier("light", value)
end}

-- Increase sanctuary duration.
Influencespec{name = "sanctuary", func = function(feat, info, args, value)
	if not args.target then return end
	args.target:inflict_modifier("sanctuary", value)
end}

-- Increase berserk duration.
Influencespec{name = "berserk", func = function(feat, info, args, value)
	if not args.target then return end
	args.target:inflict_modifier("berserk", value)
end}

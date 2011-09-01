Influencespec{name = "follow", func = function(feat, info, args, value)
	if not args.target then return end
	args.target:inflict_modifier("following", value, args.attacker)
end} 

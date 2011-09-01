Influencespec{name = "push", func = function(feat, info, args, value)
	if not args then return end
	if not args.target then return end
	if args.target.spec.type ~= "species" then return end
	args.target:face_point{point = args.attacker.position}
	args.target:set_movement(value)
end} 

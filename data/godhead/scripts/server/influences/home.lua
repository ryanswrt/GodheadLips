Influencespec{name = "home", func = function(feat, info, args, value)
	if not args.attacker.account then return end
	local home = args.attacker.position
	args.attacker.account.spawn_point = home
	args.attacker:send(Packet(packets.MARKER_ADD, "string", "home",
		"float", home.x, "float", home.y, "float", home.z))
end}

-- Increase or decrease health.
Influencespec{name = "health", func = function(feat, info, args, value)
	if not args.target then return end
	-- Randomize the amount.
	local val = value
	if val < 0 then
		val = math.min(-1, val + val * 0.5 * math.random())
	else
		val = math.max(1, val + val * 0.5 * math.random())
	end
	-- Apply unless friendly fire.
	if val > 0 or not args.attacker.client or not args.target.client then
		args.target:damaged{amount = -val, point = args.point, type = "physical"}
	end
	-- Anger hurt creatures.
	if info.influences.health < 0 then
		args.target:add_enemy(args.attacker)
	end
end}

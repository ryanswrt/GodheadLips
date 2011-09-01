-- Throw attack.
-- The weapon is fired at the specific time into the attack
-- animation. The collision callback of the projectile takes
-- care of damaging the hit object or tile.
Actionspec{name = "throw", func = function(feat, info, args)
	local charge = 1 + 2 * math.min(1, (args.charge or 0) / 2)
	Coroutine(function(t)
		Coroutine:sleep(args.user.spec.timing_attack_throw * 0.02)
		feat:play_effects(args)
		local proj = args.weapon:fire{
			charge = charge,
			collision = not args.weapon.spec.destroy_timer,
			feat = feat,
			owner = args.user,
			speed = 10 * charge,
			timer = args.weapon.spec.destroy_timer}
	end)
end}

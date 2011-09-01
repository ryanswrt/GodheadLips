-- Self-destruction.
-- The creature explodes after the animation has played.
Actionspec{name = "explode", func = function(feat, info, args)
	Coroutine(function(t)
		feat:play_effects(args)
		Coroutine:sleep(args.user.spec.timing_attack_explode * 0.02)
		args.user:die()
		Utils:explosion(args.user.position)
	end)
end}

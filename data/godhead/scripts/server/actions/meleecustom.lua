-- Custom melee attacks.
-- This is mainly used by monster specific special moves.
Actionspec{name = "melee custom", func = function(feat, info, args)
	Coroutine(function(t)
		feat:play_effects(args)
		Coroutine:sleep(args.user.spec.timing_attack_melee * 0.02)
		-- Get the attack ray.
		local src,dst = args.user:get_attack_ray(rel)
		-- Cast the attack ray.
		local r = Physics:cast_ray{src = src, dst = dst}
		if not r then return end
		-- Apply to the target.
		feat:apply{
			attacker = args.user,
			charge = args.charge,
			point = r.point,
			target = r.object,
			tile = r.tile,
			weapon = args.weapon}
	end)
end}

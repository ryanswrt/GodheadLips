-- Build terrain or machines.
-- While the attack animation is played, an attack ray is cast.
-- If a tile collides with the ray, a new tile is attached to it.
Actionspec{name = "build", func = function(feat, info, args)
	Coroutine(function(t)
		feat:play_effects(args)
		Coroutine:sleep(args.user.spec.timing_build * 0.02)
		local src,dst = args.user:get_attack_ray()
		local r = Physics:cast_ray{src = src, dst = dst}
		if not r or not r.tile then return end
		feat:apply{
			attacker = args.user,
			charge = args.charge,
			point = r.point,
			target = r.object,
			tile = r.tile,
			weapon = args.weapon}
	end)
end}

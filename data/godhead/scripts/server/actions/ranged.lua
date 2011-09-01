-- Ranged attack.
-- A projectile is fired at the specific time into the attack
-- animation. The collision callback of the projectile takes
-- care of damaging the hit object or tile.
Actionspec{name = "ranged", func = function(feat, info, args)
	Coroutine(function(t)
		if args.weapon then
			if args.weapon.spec.animation_attack == "attack bow" then
				Coroutine:sleep(args.user.spec.timing_attack_bow * 0.02)
			elseif args.weapon.spec.animation_attack == "attack crossbow" then
				Coroutine:sleep(args.user.spec.timing_attack_crossbow * 0.02)
			elseif args.weapon.spec.animation_attack == "attack musket" then
				Coroutine:sleep(args.user.spec.timing_attack_musket * 0.02)
			elseif args.weapon.spec.animation_attack == "attack revolver" then
				Coroutine:sleep(args.user.spec.timing_attack_revolver * 0.02)
			end
		end
		feat:play_effects(args)
		for name,count in pairs(info.required_ammo) do
			local ammo = args.user:split_items{name = name, count = count}
			if ammo then
				ammo:fire{
					charge = args.charge,
					collision = true,
					feat = feat,
					owner = args.user,
					speedline = true,
					weapon = args.weapon}
				return
			end
		end
	end)
end}

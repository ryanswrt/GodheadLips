-- Ranged spell.
-- A magical projectile is fired at the specific time into the attack
-- animation. The collision callback of the projectile takes
-- care of damaging the hit object or tile.
Actionspec{name = "ranged spell", func = function(feat, info, args)
	Coroutine(function(t)
		Coroutine:sleep(args.user.spec.timing_spell_ranged * 0.02)
		feat:play_effects(args)
		for index,data in ipairs(feat.effects) do
			local effect = Feateffectspec:find{name = data[1]}
			if effect and effect.projectile then
				Spell{effect = effect.name, feat = feat, model = effect.projectile, owner = args.user, power = data[2]}
				return
			end
		end
	end)
end}

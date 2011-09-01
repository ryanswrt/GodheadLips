-- Summon plagued beasts.
Influencespec{name = "plague", func = function(feat, info, args, value)
	for i = 1,math.random(1, math.ceil(value + 0.01)) do
		local s = Species:random{category = "plague"}
		if s then
			local p = args.point + Vector(
				-1 + 2 * math.random(),
				-1 + 2 * math.random(),
				-1 + 2 * math.random())
			local o = Creature{spec = s, position = p, random = true, realized = true}
			o:inflict_modifier("plague", 10000)
		end
	end
end}

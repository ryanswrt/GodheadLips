--- Plague modifier.
Modifier{name = "plague", func = function(self, object, args, secs)
	-- Initialize the plague timer.
	if not object.plague_timer or object.plague_timer > args.st then
		object.plague_timer = 0
	end
	-- Damage and infect every five second.
	if object.plague_timer and args.st - object.plague_timer > 5 then
		-- Damage.
		object:damaged{amount = 5, type = "disease"}
		object.plague_timer = args.st
		-- Infect.
		local near = Object:find{point = object.position, radius = 5}
		for k,v in pairs(near) do
			if math.random() > 0.1 then
				v:inflict_modifier("plague", 10000)
			end
		end
	end
	-- Plague never ends on its own since the timer is incremented.
	return {st=args.st + secs}
end}

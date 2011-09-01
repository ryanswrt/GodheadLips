--- Burning modifier.
Modifier{name = "burning", func = function(self, object, args, secs)
	-- Update the burning timer.
	if not object.burning_timer then
		object.burning_timer = secs
	else
		object.burning_timer = object.burning_timer + secs
	end
	-- Damage every second.
	if object.burning_timer > 1 then
		object:damaged{amount = math.random(4,7), type = "burning"}
		object.burning_timer = object.burning_timer - 1
	end
	-- End after a while.
	return {st=args.st - secs}
end}

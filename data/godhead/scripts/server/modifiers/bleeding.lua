--- Bleeding modifier.
Modifier{name = "bleeding", func = function(self, object, args, secs)
	-- Initialize the bleeding timer.
	if not object.bleeding_timer or object.bleeding_timer > args.st then
		object.bleeding_timer = 0
	end
	-- Damage every five second.
	if object.plague_timer and args.st - object.plague_timer > 1 then
		object:damaged{amount = 5, type = "bleeding"}
		object.plague_timer = args.st
	end
	return {st=args.st - secs}
end}

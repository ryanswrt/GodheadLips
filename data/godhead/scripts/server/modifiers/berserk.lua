--- Berserk modifier.
Modifier{name = "berserk", func = function(self, object, args, secs)
	return {st=args.st - secs}
end}

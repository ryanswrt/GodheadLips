--- Sanctuary modifier.
Modifier{name = "sanctuary", func = function(self, object, args, secs)
	return {st=args.st - secs}
end}

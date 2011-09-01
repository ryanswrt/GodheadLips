--- Light modifier.
Modifier{name = "light", func = function(self, object, args, secs)
	return {st=args.st - secs}
end}

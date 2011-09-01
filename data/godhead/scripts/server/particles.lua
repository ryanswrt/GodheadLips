Effect.play = function(clss, args)
	if args.point then
		Vision:event{type = "world-effect", point = args.point, effect = args.effect}
	elseif args.object then
		Vision:event{type = "object-effect", object = args.object, effect = args.effect}
	end
end

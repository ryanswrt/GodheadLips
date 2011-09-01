Modifier{name = "following", func = function(self, object, args, secs)
	if not object.following_timer then
		object.following_timer = secs
	else
		object.following_timer = object.following_timer + secs
	end
	if object.following_timer > 1 then
		object:face_point{point = args.a.position}
		object:set_movement(10)
		object:climb()
		object.following_timer = object.following_timer - 1
	end

	return {st=args.st - secs,a=args.a}
end}

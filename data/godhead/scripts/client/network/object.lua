Protocol:add_handler{type = "OBJECT_ANIMATED", func = function(event)
	local ok,id,anim,time = event.packet:read("uint32", "string", "float")
	if not ok then return end
	local o = Object:find{id = id}
	if not o then return end
	o:set_anim(anim, time)
end}

Protocol:add_handler{type = "OBJECT_BEHEADED", func = function(event)
	local ok,i,a,t = event.packet:read("uint32")
	if not ok then return end
	local o = Object:find{id = i}
	if not o then return end
	-- Remove the head.
	o.flags = Bitwise:bor(o.flags, Protocol.object_flags.BEHEADED)
	o:update_model()
	-- Play a particle effect.
	Effect:play_object("behead1", o, "#neck")
end}

Protocol:add_handler{type = "OBJECT_DAMAGE", func = function(args)
end}

Protocol:add_handler{type = "OBJECT_DEAD", func = function(args)
	local ok,i,b = args.packet:read("uint32", "bool")
	if not ok then return end
	-- Find the object.
	local obj = Object:find{id = i}
	if not obj then return end
	-- Update death status.
	if obj.dead == b then return end
	obj.dead = b
	if obj.rotation_real then
		obj:update_rotation(obj.rotation_real)
	end
end}

Protocol:add_handler{type = "OBJECT_DIALOG_CHOICE", func = function(args)
	-- Parse the packet.
	local ok,i,n = args.packet:read("uint32", "uint8")
	if not ok then return end
	local choices = {}
	for i = 1,n do
		local ok,m = args.packet:resume("string")
		if not ok then return end
		table.insert(choices, m)
	end
	-- Find the object.
	local obj = Object:find{id = i}
	if not obj then return end
	-- Update the dialog.
	obj:set_dialog("choice", choices)
end}

Protocol:add_handler{type = "OBJECT_DIALOG_MESSAGE", func = function(args)
	-- Parse the packet.
	local ok,i,c,m = args.packet:read("uint32", "string", "string")
	if not ok then return end
	-- Find the object.
	local obj = Object:find{id = i}
	if not obj then return end
	-- Update the dialog.
	obj:set_dialog("message", {character = c, message = m})
end}

Protocol:add_handler{type = "OBJECT_DIALOG_NONE", func = function(args)
	-- Parse the packet.
	local ok,i = args.packet:read("uint32")
	if not ok then return end
	-- Find the object.
	local obj = Object:find{id = i}
	if not obj then return end
	-- Update the dialog.
	obj:set_dialog("none")
end}

Protocol:add_handler{type = "OBJECT_FEAT", func = function(event)
	local ok,i,a,move = event.packet:read("uint32", "string", "uint8")
	if not ok then return end
	-- Find the object.
	local obj = Object:find{id = i}
	if not obj then return end
	if not obj.spec then return end
	if not obj.spec.animations then return end
	-- Find the feat animation.
	local anim = Featanimspec:find{name = a}
	if not anim then return end
	-- Find the character animation.
	-- The animation of the feat may be overridden by a weapon specific
	-- animation if there's a weapon in the slot used by the feat.
	local animation = anim.animation
	if anim.slot and obj.equipment then
		local weapon = Itemspec:find{name = obj.equipment[anim.slot]}
		if weapon and weapon.animation_attack then
			animation = weapon.animation_attack
		end
		if weapon and weapon.effect_attack_speedline then
			local w = obj.slots:get_object{slot = anim.slot}
			if w then w.speedline = Speedline{delay = 0.3, duration = 0.8, object = w} end
		end
	end
	-- Melee feats may further override the animation since controls affect
	-- what move the player performs. This is indicated with the move variable.
	if move ~= 0 then
		local map = {
			"attack stand",
			"attack left",
			"attack right",
			"attack back",
			"attack front"}
		animation = map[move] or animation
	end
	-- Play the animation.
	obj:animate_spec(animation)
end}

Protocol:add_handler{type = "OBJECT_EFFECT", func = function(event)
	local ok,i,e = event.packet:read("uint32", "string")
	if ok then
		-- Find the object.
		local obj = Object:find{id = i}
		if not obj then return end
		-- Play the effect.
		Effect:play_object(e, obj)
	end
end}

Protocol:add_handler{type = "OBJECT_HIDDEN", func = function(event)
	local ok,i = event.packet:read("uint32")
	if ok then
		local o = Object:find{id = i}
		if o then o:detach() end
		if i == Gui.active_dialog then Gui:set_dialog() end
	end
end}

Protocol:add_handler{type = "OBJECT_MOVED", func = function(event)
	local ok,i,x,y,z,tilt,rx,ry,rz,rw,vx,vy,vz = event.packet:read("uint32", "float", "float", "float", "float", "float", "float", "float", "float", "float", "float", "float")
	if ok then
		local o = Object:find{id = i}
		if not o then return end
		-- Apply position correction.
		-- Objects controlled by physics would normally float in the air
		-- due to collision margins so we need to apply compensation.
		local p = Vector(x, y, z)
		if o.type == "item" or o.type == "species" then p = p + Object.physics_position_correction end
		-- Set the target interpolation position.
		o:set_motion_state(p, Quaternion(rx, ry, rz, rw), Vector(vx, vy, vz), tilt)
		-- Adjust time scaling of movement animations.
		if o.spec and o.spec.speed_walk then
			local a = o:get_animation{channel = 1}
			if a then
				local ref
				local map = {
					["run"] = o.spec.speed_run,
					["run left"] = o.spec.speed_run,
					["run right"] = o.spec.speed_run,
					["strafe left"] = o.spec.speed_run,
					["strafe right"] = o.spec.speed_run,
					["walk"] = o.spec.speed_walk,
					["walk back"] = o.spec.speed_run}
				for k,v in pairs(o.spec.animations) do
					if v.animation == a.animation and map[k] then
						ref = map[k]
						break
					end
				end
				if ref then
					local fract = 0.3
					local speed = Vector(vx, vy, vz).length
					local scale = speed / math.max(0.1, ref)
					a.time_scale = a.time_scale * (1 - fract) + scale * fract
					o:animate(a)
				end
			end
		end
	end
end}

Protocol:add_handler{type = "OBJECT_SHOWN", func = function(event)
	local debug = function() end
	--local debug = function(...) print(string.format(...)) end
	-- Creation flags.
	local ok,id,flags = event.packet:read("uint32", "uint32")
	if not ok then return end
	debug("SHOWOBJ %d %d", id, flags)
	-- Hide old objects.
	local old = Object:find{id = id}
	if old then old:detach() end
	-- Spec.
	local spec,model,type
	if Bitwise:band(flags, Protocol.object_show_flags.SPEC) ~= 0 then
		local ok,type_,name = event.packet:resume("string", "string")
		if not ok then return end
		type = type_
		if type == "item" then spec = Itemspec:find{name = name}
		elseif type == "obstacle" then spec = Obstaclespec:find{name = name}
		elseif type == "species" then spec = Species:find{name = name} end
		debug("  SPEC %s %s", type, name)
		if not spec then return end
		model = spec.model
	else
		local ok,m = event.packet:resume("string")
		if not ok then return end
		model = m
		type = "object"
	end
	-- Create the object.
	local o
	local o_args = {collision_group = Physics.GROUP_OBJECT, flags = Bitwise:band(flags, 0xFF),
		id = id, model = model, spec = spec, type = type}
	if type == "species" then
		o = Creature(o_args)
	else
		o = Object(o_args)
		if spec and spec.special_effects then Object.dict_active[o] = 0.1 end
	end
	-- Self.
	if Bitwise:band(flags, Protocol.object_show_flags.SELF) ~= 0 then
		debug("  SELF")
		o.collision_group = Physics.GROUP_PLAYER
		Client.player_object = o
		Client.mouse_smoothing = Client.views.options.mouse_smoothing
	end
	-- Species.
	if Bitwise:band(flags, Protocol.object_show_flags.SPECIES) ~= 0 then
		debug("  SPECIES")
		local ok,dead,tilt = event.packet:resume("bool", "float")
		if not ok then return end
		o.dead = dead
		o.tilt = tilt
	end
	-- Position.
	if Bitwise:band(flags, Protocol.object_show_flags.POSITION) ~= 0 then
		debug("  POSITION")
		local ok,x,y,z = event.packet:resume("float", "float", "float")
		if not ok then return end
		debug("    %f %f %f", x, y, z)
		local p = Vector(x,y,z)
		if type == "item" or type == "species" then p = p + Object.physics_position_correction end
		o.position = p
	end
	-- Rotation.
	if Bitwise:band(flags, Protocol.object_show_flags.ROTATION) ~= 0 then
		debug("  ROTATION")
		local ok,x,y,z,w = event.packet:resume("float", "float", "float", "float")
		if not ok then return end
		debug("    %f %f %f %f", x, y, z, w)
		o.rotation = Quaternion(x,y,z,w)
	end
	-- Name.
	if Bitwise:band(flags, Protocol.object_show_flags.NAME) ~= 0 then
		debug("  NAME")
		local ok,name = event.packet:resume("string")
		if not ok then return end
		o.name = name
		debug("    %q", name)
	end
	-- Animations.
	if Bitwise:band(flags, Protocol.object_show_flags.ANIMS) ~= 0 then
		debug("  ANIMATIONS")
		local ok,num = event.packet:resume("uint8")
		if not ok then return end
		debug("    %d", num)
		for i=1,num do
			local ok,anim,time = event.packet:resume("string", "float")
			if not ok then return end
			debug("    %s %f", anim, time)
			o:set_anim(anim, time)
		end
	end
	-- Slots.
	if Bitwise:band(flags, Protocol.object_show_flags.SLOTS) ~= 0 then
		debug("  SLOTS")
		local ok,num = event.packet:resume("uint8")
		if not ok then return end
		debug("    %d", num)
		for i=1,num do
			local ok,count,spec,slot = event.packet:resume("uint32", "string", "string")
			if not ok then return end
			debug("    %d %s %s", count, spec, slot)
			o:set_slot(slot, spec, count)
		end
	end
	-- Skills.
	if Bitwise:band(flags, Protocol.object_show_flags.SKILLS) ~= 0 then
		debug("  SKILLS")
		local ok,num = event.packet:resume("uint8")
		if not ok then return end
		debug("    %d", num)
		for i=1,num do
			local ok,skill,value,max = event.packet:resume("string", "int32", "int32")
			if not ok then return end
			debug("    %s %d %d", skill, value, max)
			o:set_skill(skill, value, max)
		end
	end
	-- Body style.
	if Bitwise:band(flags, Protocol.object_show_flags.BODY_STYLE) ~= 0 then
		debug("  BODY STYLE")
		local ok,a,b,c,d,e,f,g,h,i
		-- Scale.
		ok,a = event.packet:resume("uint8")
		if not ok then return end
		o.body_scale = a / 255
		debug("    SCALE %d", a)
		-- Style.
		ok,a,b,c,d,e,f,g,h,i = event.packet:resume("uint8", "uint8", "uint8", "uint8", "uint8",
			"uint8", "uint8", "uint8", "uint8", "uint8")
		if not ok then return end
		o.body_style = {a / 255, b / 255, c / 255, d / 255, e / 255,
			f / 255, g / 255, h / 255, i / 255}
		debug("    STYLE %d %d %d %d %d %d %d %d %d", a, b, c, d, e, f, g, h, i)
		-- Skin.
		ok,a,b,c,d = event.packet:resume("string", "uint8", "uint8", "uint8")
		if not ok then return end
		o.skin_style = a
		o.skin_color = {b / 255, c / 255, d / 255}
		debug("    SKIN %s %d %d %d", a, b, c, d)
	end
	-- Head style.
	if Bitwise:band(flags, Protocol.object_show_flags.HEAD_STYLE) ~= 0 then
		debug("  HEAD STYLE")
		local ok,a,b,c,d,e,f,g,h,i,j,k,l,m,n,p
		-- Eyes.
		ok,a,b,c,d = event.packet:resume("string", "uint8", "uint8", "uint8")
		if not ok then return end
		o.eye_style = a
		o.eye_color = {b / 255, c / 255, d / 255}
		debug("    EYE %s %d %d %d", a, b, c, d)
		-- Face.
		ok,a,b,c,d,e,f,g,h,i,j,k,l,m,n,p = event.packet:resume("uint8", "uint8", "uint8", "uint8", "uint8",
			"uint8", "uint8", "uint8", "uint8", "uint8", "uint8", "uint8", "uint8", "uint8", "uint8")
		if not ok then return end
		o.face_style = {a / 255, b / 255, c / 255, d / 255, e / 255, f / 255, g / 255,
			h / 255, i / 255, j / 255, k / 255, l / 255, m / 255, n / 255, p / 255}
		debug("    FACE %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", a, b, c, d, e, f, g, h, i, j, k, l, m, n, p)
		-- Hair.
		ok,a,b,c,d = event.packet:resume("string", "uint8", "uint8", "uint8")
		if not ok then return end
		o.hair_style = a
		o.hair_color = {b / 255, c / 255, d / 255}
		debug("    HAIR %s %d %d %d", a, b, c, d)
	end
	-- Dialog.
	if Bitwise:band(flags, Protocol.object_show_flags.DIALOG) ~= 0 then
		debug("  DIALOG")
		local ok,a
		ok,a = event.packet:resume("uint8")
		if not ok then return end
		if a == 0 then
			-- Choices.
			local choices = {}
			ok,a = event.packet:resume("uint8")
			debug("    CHOICES %d", a)
			if not ok then return end
			for i = 1,a do
				ok,a = event.packet:resume("string")
				if not ok then return end
				debug("      %s", a)
				table.insert(choices, a)
			end
			o:set_dialog("choice", choices)
		elseif a == 1 then
			-- Line.
			debug("    MESSAGE", a)
			ok,a = event.packet:resume("string")
			if not ok then return end
			o:set_dialog("message", a)
			debug("      %s", a)
		else
			-- None.
			debug("    NONE", a)
			o:set_dialog("none")
		end
	end
	-- Set the spec model.
	debug("  OK")
	o.flags = Bitwise:band(flags, 0xFF)
	o:set_model()
	o:update_rotation(o.rotation, o.tilt)
	o.realized = true
	-- Initialize speed lines.
	if Bitwise:band(flags, Protocol.object_flags.SPEEDLINE) ~= 0 then
		o.speedline = Speedline{object = o}
	end
end}

Protocol:add_handler{type = "OBJECT_SKILL", func = function(event)
	local ok,id,skill,value,max = event.packet:read("uint32", "string", "int32", "int32")
	if not ok then return end
	local o = Object:find{id = id}
	if not o then return end
	o:set_skill(skill, value, max)
end}

Protocol:add_handler{type = "OBJECT_SLOT", func = function(event)
	local ok,id,count,spec,slot = event.packet:read("uint32", "uint32", "string", "string")
	if not ok then return end
	local o = Object:find{id = id}
	if not o then return end
	o:set_slot(slot, spec, count)
end}

Protocol:add_handler{type = "OBJECT_SPEECH", func = function(event)
	local ok,i,m = event.packet:read("uint32", "string")
	if ok then
		local o = Object:find{id = i}
		if o then
			local n = o.name or o.spec.name
			Speech:add{object = o, message = m}
			Gui.chat_history:append{text = "<" .. n .. "> " .. m}
			Sound:effect{object = o, effect = "spring-000"}
		end
	end
end}

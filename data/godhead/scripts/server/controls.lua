local spawn_player = function(object, client, spawnpoint)
	-- Select the spawn point.
	-- If the account doesn't have a spawn point yet, set the selected
	-- marker or the default spawn point as its spawn point.
	local home = object.account.spawn_point or Config.inst.spawn_point
	if spawnpoint and spawnpoint ~= "home" then
		-- TODO: Rather spawn to regions?
		local m = Marker:find{name = spawnpoint}
		if m then home = m.position end
	end
	if not object.account.spawn_point then
		object.account.spawn_point = home
	end
	-- Add to the map.
	Player.clients[client] = object
	object:teleport{position = home}
	object.realized = true
	-- Transmit the home marker.
	object:send(Packet(packets.MARKER_ADD, "string", "home",
		"float", home.x, "float", home.y, "float", home.z))
	-- Transmit unlocked map markers.
	for k,m in pairs(Marker.dict_name) do
		if m.unlocked then
			object:send(Packet(packets.MARKER_ADD,
				"string", m.name,
				"float", m.position.x,
				"float", m.position.y,
				"float", m.position.z))
		end
	end
	-- Transmit active and completed quests.
	for k,q in pairs(Quest.dict_name) do
		q:send{client = object}
		q:send_marker{client = object}
	end
end

Protocol:add_handler{type = "CHARACTER_CREATE", func = function(args)
	-- Make sure the client has been authenticated.
	local account = Account.dict_client[args.client]
	if not account then return end
	-- Make sure not created already.
	local player = Player:find{client = args.client}
	if player then return end
	-- Get character flags.
	local ok,na,ra,s1,s2,s3,s4,s5,s6,b1,b2,b3,b4,b5,b6,b7,b8,b9,eye,eyer,eyeg,eyeb,
	f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15,
	hair,hairr,hairg,hairb,skin,skinr,sking,skinb,spawnpoint = args.packet:read(
		"string", "string",
		-- Skills.
		"uint8", "uint8", "uint8", "uint8", "uint8", "uint8",
		-- Body style.
		"uint8", "uint8", "uint8", "uint8", "uint8",
		"uint8", "uint8", "uint8", "uint8",
		-- Eye style.
		"string", "uint8", "uint8", "uint8",
		-- Face style.
		"uint8", "uint8", "uint8", "uint8", "uint8",
		"uint8", "uint8", "uint8", "uint8", "uint8",
		"uint8", "uint8", "uint8", "uint8", "uint8",
		-- Hair style.
		"string", "uint8", "uint8", "uint8",
		-- Skin style.
		"string", "uint8", "uint8", "uint8",
		-- Spawnpoint.
		"string")
	if not ok then return end
	local spec = Species:find{name = ra .. "-player"}
	if not spec then return end
	-- TODO: Input validation.
	-- Create character. To prevent the player from falling inside the ground
	-- when spawned in a yet to be loaded region, we disable the physics of
	-- the object for a short while.
	local o = Player{
		account = account,
		body_scale = b1,
		body_style = {b2,b3,b4,b5,b6,b7,b8,b9},
		eye_style = {eye, eyer, eyeg, eyeb},
		face_style = {f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15},
		hair_style = {hair, hairr, hairg, hairb},
		name = (na ~= "" and na or "Player"),
		random = true,
		skin_style = {skin, skinr, sking, skinb},
		spec = spec}
	-- Set skills.
	local names = {"dexterity", "health", "intelligence", "perception", "strength", "willpower"}
	local values = {s1, s2, s3, s4, s5, s6}
	for i = 1,#names do o:set_skill(names[i], 0) end
	for i = 1,#names do
		o:set_skill(names[i], values[i])
		local real = o.skills:get_maximum{skill = names[i]}
		o.skills:set_value{skill = names[i], value = 0.666 * real}
	end
	-- Add to the map.
	Network:send{client = args.client, packet = Packet(packets.CHARACTER_ACCEPT)}
	o:set_client(args.client)
	spawn_player(o, args.client, spawnpoint)
	Serialize:save_account(account, o)
end}

Protocol:add_handler{type = "CLIENT_AUTHENTICATE", func = function(args)
	-- Make sure not authenticated already.
	local account = Account.dict_client[args.client]
	if account then return end
	-- Read credentials.
	local ok,login,pass = args.packet:read("string", "string")
	if not ok then
		Network:send{client = args.client, packet = Packet(packets.AUTHENTICATE_REJECT, "string", "Protocol mismatch.")}
		Network:disconnect(args.client)
		return
	end
	-- Make sure not logging in twice.
	account = Account.dict_name[login]
	if account then
		Network:send{client = args.client, packet = Packet(packets.AUTHENTICATE_REJECT, "string", "The account is already in use.")}
		return
	end
	-- Load or create the account.
	-- The password is also checked in case of an existing account. If the
	-- check fails, Account() returns nil and we disconnect the client.
	account = Account(login, pass)
	if not account then
		Network:send{client = args.client, packet = Packet(packets.AUTHENTICATE_REJECT, "string", "Invalid account name or password.")}
		Network:disconnect(args.client)
		return
	end
	account.client = args.client
	Account.dict_client[args.client] = account
	-- Create existing characters.
	local created
	if account.character then
		local func = assert(loadstring("return function()\n" .. account.character .. "\nend"))()
		if func then
			local object = func()
			if object then
				Network:send{client = args.client, packet = Packet(packets.CHARACTER_ACCEPT)}
				object.account = account
				object:set_client(args.client)
				spawn_player(object, args.client)
				created = true
			end
		end
	end
	-- Check for permissions.
	-- Check if the account has admin rights in the config file.
	-- Grant admin rights to the first client if started with --admin.
	local admin = (Config.inst.admins[login] == true)
	if Settings.admin then
		Settings.admin = nil
		Config.inst.admins[login] = true
		admin = true
	end
	-- Inform about admin privileges.
	Network:send{client = args.client, packet = Packet(packets.ADMIN_PRIVILEGE, "bool", admin)}
	-- Enter the character creation mode.
	if not created then
		Network:send{client = args.client, packet = Packet(packets.CHARACTER_CREATE)}
	end
end}

Protocol:add_handler{type = "PLAYER_EXAMINE", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,inv,slot = args.packet:read("uint32", "uint32")
		if ok then
			local object = player:find_target(inv, slot)
			if object then
				if object.examine_cb then
					object:examine_cb(player)
				else
					player:send{packet = Packet(packets.MESSAGE, "string",
						"There's nothing special about this object.")}
				end
			end
		end
	end
end}

Protocol:add_handler{type = "FEAT", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		-- Players are able to create custom feats on client side. When
		-- the feat is performed, the client sends us all the information
		-- on the feat. We then reconstruct and perform the feat.
		local ok,anim,e1,v1,e2,v2,e3,v3,on = args.packet:read("string",
			"string", "float", "string", "float", "string", "float", "bool")
		if ok then
			if anim == "" or not Featanimspec:find{name = anim} then anim = nil end
			if e1 == "" or not Feateffectspec:find{name = e1} then e1 = nil end
			if e2 == "" or not Feateffectspec:find{name = e2} then e2 = nil end
			if e3 == "" or not Feateffectspec:find{name = e3} then e3 = nil end
			local feat = Feat{animation = anim, effects = {
				e1 and {e1, math.max(1, math.min(100, v1))},
				e2 and {e2, math.max(1, math.min(100, v2))},
				e3 and {e3, math.max(1, math.min(100, v3))}}}
			feat:perform{stop = not on, user = player}
		end
	end
end}

Protocol:add_handler{type = "INVENTORY_CLOSED", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,id = args.packet:read("uint32")
		if not ok then return end
		local inv = Inventory:find{id = id}
		if not inv or inv == player.inventory then return end
		inv:unsubscribe{object = player}
	end
end}

Protocol:add_handler{type = "PLAYER_JUMP", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		player:jump()
	end
end}

Protocol:add_handler{type = "MOVE_ITEM", func = function(args)
	-- Find the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Read the source and destination types.
	local ok,src,dst,count = args.packet:read("uint8", "uint8", "uint32")
	if not ok then return end
	if src == moveitem.WORLD and dst == moveitem.WORLD then return end
	-- Read the detailed source information.
	local srcid = nil
	local srcslot = nil
	if src == moveitem.EQUIPMENT then
		ok,srcid,srcslot = args.packet:resume("uint32", "string")
		if not ok then return end
	elseif src == moveitem.INVENTORY then
		ok,srcid,srcslot = args.packet:resume("uint32", "uint8")
		if not ok then return end
	elseif src == moveitem.WORLD then
		ok,srcid = args.packet:resume("uint32")
		if not ok then return end
	else return end
	-- Read the detailed destination information.
	local dstid = nil
	local dstslot = nil
	if dst == moveitem.EQUIPMENT then
		ok,dstid,dstslot = args.packet:resume("uint32", "string")
		if not ok then return end
	elseif dst == moveitem.INVENTORY then
		ok,dstid,dstslot = args.packet:resume("uint32", "uint8")
		if not ok then return end
	elseif dst ~= moveitem.WORLD then return end
	-- Perform the item move.
	if src == moveitem.WORLD then
		player:pick_up(srcid, dstid, dstslot)
	elseif dst == moveitem.WORLD then
		player:animate("drop")
		Timer{delay = player.spec.timing_drop * 0.02, func = function(timer)
			Actions:move_from_inv_to_world(player, srcid, srcslot, count)
			timer:disable()
		end}
	else
		return Actions:move_from_inv_to_inv(player, srcid, srcslot, dstid, dstslot, count)
	end
end}

Protocol:add_handler{type = "PLAYER_BLOCK", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	local ok,v = args.packet:read("bool")
	if ok then player:set_block(v) end
end}

Protocol:add_handler{type = "PLAYER_CLIMB", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		player:climb()
	end
end}

Protocol:add_handler{type = "PLAYER_MOVE", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,v = args.packet:read("int8")
		if ok then
			if v > 0 then
				player:set_movement(1)
			elseif v < 0 then
				player:set_movement(-1)
			else
				player:set_movement(0)
			end
		end
	end
end}

Protocol:add_handler{type = "PLAYER_RESPAWN", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then return end
	player:respawn()
end}

Protocol:add_handler{type = "PLAYER_TURN", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,x,y,z,w = args.packet:read("float", "float", "float", "float")
		if ok then
			local e = Quaternion(x, y, z, w).euler
			e[3] = math.min(player.spec.tilt_limit, e[3])
			e[3] = math.max(-player.spec.tilt_limit, e[3])
			player.tilt = Quaternion{euler = {0, 0, e[3]}}
			player.rotation = Quaternion{euler = {e[1], e[2], 0}}
			Vision:event{type = "object-moved", object = o}
		end
	end
end}

Protocol:add_handler{type = "PLAYER_RUN", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,on = args.packet:read("bool")
		if ok then
			player.running = on
			player:calculate_speed()
		end
	end
end}

Protocol:add_handler{type = "PLAYER_SKILLS", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		-- Read packet data.
		local ok,s,v = args.packet:read("string", "float")
		if not ok then return end
		player:set_skill(s, v)
	end
end}

Protocol:add_handler{type = "PLAYER_ATTACK", func = function(args)
	-- Find the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Read the packet.
	local ok,v = args.packet:read("bool")
	if not ok then return end
	-- Handle auto-attack.
	if v then
		player.auto_attack = true
	else
		player.auto_attack = nil
	end
	-- Handle attacks.
	if v and player.attack_charge then return end
	if not v and not player.attack_charge then return end
	if v then
		player:attack_charge_start()
	else
		player:attack_charge_end(args)
	end
end}

Protocol:add_handler{type = "PLAYER_STRAFE", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,val = args.packet:read("int8")
		if ok then
			player:set_strafing(val / 127)
		end
	end
end}

Protocol:add_handler{type = "THROW", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,inv,slot = args.packet:read("uint32", "uint32")
		if ok then player:throw(inv, slot) end
	end
end}

Protocol:add_handler{type = "PLAYER_USE", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Read the source type.
	local ok,inv,type = args.packet:read("uint32", "uint8")
	if not ok then return end
	-- Read the source slot.
	local slot
	if type == 0 then
		ok,slot = args.packet:resume("uint32")
	else
		ok,slot = args.packet:resume("string")
	end
	if not ok then return end
	-- Use the object.
	local object = player:find_target(inv, slot)
	if object and object.use_cb then
		object:use_cb(player)
	end
end}

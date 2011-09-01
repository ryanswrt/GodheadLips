Protocol:add_handler{type = "ADMIN_DELETE", func = function(args)
	-- Check for permissions.
	local player = Player:find{client = args.client}
	if not player.admin then return player:send("You have no permission to do that.") end
	-- Delete objects.
	args.packet:read()
	while true do
		local ok,id = args.packet:resume("uint32")
		if not ok then break end
		local obj = Object:find{id = id}
		if obj then
			obj:die()
			obj:purge()
		end
	end
end}

Protocol:add_handler{type = "ADMIN_SAVE", func = function(args)
	-- Check for permissions.
	local player = Player:find{client = args.client}
	if not player.admin then return player:send("You have no permission to do that.") end
	-- Save.
	print("Saving world state...")
	Serialize:save()
	print("Done")
end}

Protocol:add_handler{type = "ADMIN_SHUTDOWN", func = function(args)
	-- Check for permissions.
	local player = Player:find{client = args.client}
	if not player.admin then return player:send("You have no permission to do that.") end
	-- Save and shutdown.
	Serialize:save()
	Program:shutdown()
end}

Protocol:add_handler{type = "ADMIN_SPAWN", func = function(args)
	-- Check for permissions.
	local player = Player:find{client = args.client}
	if not player.admin then return player:send("You have no permission to do that.") end
	-- Spawn an object.
	local ok,typ,msg = args.packet:read("string", "string")
	if ok then
		if msg == "" then msg = nil end
		if typ == "item" then
			if not msg then return end
			local spec = Itemspec:find{name = msg}
			if not spec then return end
			Item{
				spec = spec,
				position = player.position,
				random = true,
				realized = true}
		elseif typ == "item-cat" then
			if not msg then return end
			local spec = Itemspec:random{category = msg}
			if not spec then return end
			Item{
				spec = spec,
				position = player.position,
				random = true,
				realized = true}
		elseif typ == "obstacle" then
			if not msg then return end
			local spec = Obstaclespec:find{name = msg}
			if not spec then return end
			Obstacle{
				spec = spec,
				position = player.position,
				realized = true}
		elseif typ == "obstacle-cat" then
			if not msg then return end
			local spec = Obstaclespec:random{category = msg}
			if not spec then return end
			Obstacle{
				spec = spec,
				position = player.position,
				realized = true}
		elseif typ == "creature" then
			if not msg then return end
			local spec = Species:find{name = msg}
			if not spec then return end
			Creature{
				spec = spec,
				position = player.position,
				random = true,
				realized = true}
		elseif msg then
			local object = Object{
				model = msg,
				position = player.position,
				realized = true,
				static = true}
		end
	end
end}

Protocol:add_handler{type = "ADMIN_STATS", func = function(args)
	-- Check for permissions.
	local player = Player:find{client = args.client}
	if not player.admin then return player:send("You have no permission to do that.") end
	-- Count objects.
	local num_players_miss = 0
	local num_players_real = 0
	local num_creatures_idle = 0
	local num_creatures_miss = 0
	local num_creatures_real = 0
	local num_items_miss = 0
	local num_items_inv = 0
	local num_items_real = 0
	local num_obstacles_miss = 0
	local num_obstacles_real = 0
	local num_objects_miss = 0
	local num_objects_real = 0
	local num_vision_miss = 0
	local num_vision_real = 0
	for k,v in pairs(Object.objects) do
		if v.class_name == "Player" then
			if v.realized then
				num_players_real = num_players_real + 1
			else
				num_players_miss = num_players_miss + 1
			end
			if v.vision then
				for k1,v1 in pairs(v.vision.objects) do
					if k1.realized then
						num_vision_real = num_vision_real + 1
					else
						num_vision_miss = num_vision_miss + 1
					end
				end
			end
		elseif v.class_name == "Creature" then
			if v.realized then
				if v.ai and v.ai.state ~= "none" then
					num_creatures_real = num_creatures_real + 1
				else
					num_creatures_idle = num_creatures_idle + 1
				end
			else
				num_creatures_miss = num_creatures_miss + 1
			end
		elseif v.class_name == "Item" then
			if v.realized then
				num_items_real = num_items_real + 1
			elseif Inventory:find{object = v} then
				num_items_inv = num_items_inv + 1
			else
				num_items_miss = num_items_miss + 1
			end
		elseif v.class_name == "Obstacle" then
			if v.realized then
				num_obstacles_real = num_obstacles_real + 1
			else
				num_obstacles_miss = num_obstacles_miss + 1
			end
		else
			if v.realized then
				num_objects_real = num_objects_real + 1
			else
				num_objects_miss = num_objects_miss + 1
			end
		end
	end
	-- Count sectors.
	local num_sectors = 0
	for k,v in pairs(Serialize.sectors.sectors) do
		num_sectors = num_sectors + 1
	end
	-- Send stats.
	player:send{packet = Packet(packets.ADMIN_STATS, "string", string.format(
		[[Players: %d+%d
		Creatures: %d+%d+%d
		Items: %d+%d+%d
		Obstacles: %d+%d
		Others: %d+%d
		Vision: %d+%d
		Sectors: %d
		Tick update: %d ms
		Tick event: %d ms]],
		num_players_real, num_players_miss,
		num_creatures_real, num_creatures_idle, num_creatures_miss,
		num_items_real, num_items_inv, num_items_miss,
		num_obstacles_real, num_obstacles_miss,
		num_objects_real, num_objects_miss,
		num_vision_real, num_vision_miss,
		num_sectors,
		Program.profiling.update * 1000, Program.profiling.event * 1000))}
end}

require "server/region"

Generator = Class()
Generator.class_name = "Generator"
Generator.map_size = Vector(1000, 1000, 1000)
Generator.map_start = Vector(600, 600, 600) - Generator.map_size * 0.5
Generator.map_end = Vector(600, 600, 600) + Generator.map_size * 0.5
Generator.map_version = "6"
Generator.layer_offset = 1600
Generator.layer_count = 20
Generator.layer_size = Generator.map_size.y / Generator.layer_count
Generator.bin_size = 6
Generator.bin_stride = 512
Generator.bin_stride2 = 512*512

--- Creates a new map generator.
-- @param clss Generator class.
-- @return Generator.
Generator.new = function(clss)
	local self = Class.new(Generator)
	self.seed1 = math.random(10000, 60000)
	self.seed2 = math.random(10000, 60000)
	self.seed3 = math.random(10000, 60000)
	self:reset()
	return self
end

Generator.add_region = function(self, region)
	
	-- Store by name.
	table.insert(self.regions_dict_id, region)
	self.regions_dict_name[region.spec.name] = region
	-- Store by layers.
	local min,max = region:get_layer_range()
	for i=min,max do
		local dict = self.regions_dict_layer[i]
		table.insert(dict, region)
	end
	-- Add to the space partitioning table.
	self:for_each_bin(region.point, region.size, function(i)
		self.bins[i] = true
	end)
end

Generator.for_each_bin = function(self, point, size, func)
	local p = (point * (1 / self.bin_size)):floor()
	local s = (size * (1 / self.bin_size)):ceil()
	for x = p.x,p.x+s.x do
		for y = p.y,p.y+s.y do
			for z = p.z,p.z+s.z do
				func(x + self.bin_stride * y + self.bin_stride2 * z)
			end
		end
	end
end

--- Generates the world map.
-- @param self Generator.
-- @param args Arguments.
Generator.generate = function(self, args)
	local place_regions = function()
		local special = Regionspec:find{category = "special"}
		while true do
			local placed = 0
			local skipped = 0
			-- Try to place one or more regions.
			-- Regions are often placed relative to each other so we need to
			-- add them iteratively in the other of least dependencies.
			for name,reg in pairs(special) do
				if not self.regions_dict_name[reg.name] then
					local pat = Pattern:random{category = reg.pattern_category, name = reg.pattern_name}
					if not self:place_region(reg, pat) then
						skipped = skipped + 1
					else
						placed = placed + 1
					end
				end
			end
			-- The generator may sometimes run into a dead end where a region
			-- can't be placed without violating the constraints. In such a
			-- case, nil is returned and generation is restarted from scratch.
			if placed == 0 then return end
			if skipped == 0 then break end
		end
		return true
	end
	-- Remove all player characters.
	for k,v in pairs(Player.clients) do
		v:detach(true)
	end
	Player.clients = {}
	-- Reset the world.
	self:update_status(0, "Resetting world")
	Marker:reset()
	Sectors.instance:unload_world()
	-- Place special areas.
	-- Regions have dependencies so we need to place them in several passes.
	-- The region tables are filled but the map is all empty after this.
	repeat
		self:reset()
		self:update_status(0, "Placing regions")
	until place_regions()
	-- Mark roads.
	--[[self:update_status(0, "Creating roads")
	local linkn = 0
	for _,reg1 in pairs(self.regions_dict_name) do
		for _,name in ipairs(reg1.spec.links) do
			local reg2 = self.regions_dict_name[name]
			local link = reg1:create_link(reg2)
			linkn = linkn + 1
			self.links[linkn] = link
		end
	end
	for i,link in ipairs(self.links) do
		local src = link[1]:get_link_point(link[2].point)
		local dst = link[2]:get_link_point(link[1].point)
		self:mark_road(src, dst)
		self:update_status(i / linkn)
	end]]
	-- Add map markers for regions.
	-- These are used by location discovery.
	for _,reg in pairs(self.regions_dict_id) do
		local p = reg.point + reg.pattern.size * 0.5
		Marker{name = reg.spec.name, discoverable = true, position = p * Voxel.tile_size}
	end
	-- Format regions.
	-- This creates the sectors that contain special areas.
	self:update_status(0, "Formatting regions")
	for _,reg in pairs(self.regions_dict_id) do
		Voxel:fill_region{point = reg.point, size = reg.pattern.size}
	end
	-- Find used sectors.
	self:update_status(0, "Counting sectors")
	self.sectors = {}
	local sectorn = 0
	local sectors = Program.sectors
	for k in pairs(sectors) do
		self.sectors[k] = "Heightmap"
		sectorn = sectorn + 1
	end
	-- Create fractal terrain.
	self:update_status(0, "Randomizing terrain")
	local index = 0
	for k in pairs(sectors) do
		Generator.Main:generate(k)
		self:update_status(index / sectorn)
		index = index + 1
	end
	-- Paint regions.
	self:update_status(0, "Creating regions")
	for _,reg in pairs(self.regions_dict_id) do
		Voxel:fill_region{point = reg.point, size = reg.pattern.size}
		Voxel:place_pattern{point = reg.point, name = reg.pattern.name}
	end
	-- Save the map.
	self:update_status(0, "Saving the map")
	Sectors.instance:save_world(true, function(p) self:update_status(p) end)
	Sectors.instance:unload_world()
	Serialize:set_value("map_version", Generator.map_version)
	-- Save map markers.
	self:update_status(0, "Saving quests")
	Serialize:save_generator(true)
	Serialize:save_markers(true)
	Serialize:save_quests(true)
	Serialize:save_accounts(true)
	-- Discard events emitted during map generation so that they
	-- don't trigger when the game starts.
	self:update_status(0, "Finishing")
	Program:update()
	repeat until not Program:pop_event()
	Program:update()
	repeat until not Program:pop_event()
	-- Inform players of the generation being complete.
	-- All accounts were erased so clients need to re-authenticate.
	local status = Packet(packets.CLIENT_AUTHENTICATE)
	for k,v in pairs(Network.clients) do
		Network:send{client = v, packet = status}
	end
end

--- Creates ore deposits and places plants.
-- @param self Sectors.
-- @param pos Position in tiles.
-- @param size Size in tiles.
-- @param amount How many resources to create at most.
Generator.generate_resources = function(self, pos, size, amount)
	-- /FIXME
	local mats = {
		Material:find{name = "adamantium1"},
		Material:find{name = "aquanite1"},
		Material:find{name = "basalt1"},
		Material:find{name = "brittlerock1"},
		Material:find{name = "crimson1"},
		Material:find{name = "ferrostone1"},
		Material:find{name = "sand1"},
		Material:find{name = "soil1"}}
	--[[Material:find{name = "grass1"},
		Material:find{name = "ice1"},
		Material:find{name = "iron1"},
		Material:find{name = "magma1"},
		Material:find{name = "pipe1"},
		Material:find{name = "water1"},
		Material:find{name = "wood1"}]]
	local points = {
		Vector(-1,-1,-1), Vector(0,-1,-1), Vector(1,-1,-1),
		Vector(-1,0,-1), Vector(0,0,-1), Vector(1,0,-1),
		Vector(-1,1,-1), Vector(0,1,-1), Vector(1,1,-1),
		Vector(-1,-1,0), Vector(0,-1,0), Vector(1,-1,0),
		Vector(-1,0,0), Vector(0,0,0), Vector(1,0,0),
		Vector(-1,1,0), Vector(0,1,0), Vector(1,1,0),
		Vector(-1,-1,1), Vector(0,-1,1), Vector(1,-1,1),
		Vector(-1,0,1), Vector(0,0,1), Vector(1,0,1),
		Vector(-1,1,1), Vector(0,1,1), Vector(1,1,1)}
	local create_plant_or_item = function(ctr)
		local i = 0
		repeat
			i = i + 1
			ctr.y = ctr.y - 1
			if i > 10 then return end
		until Voxel:get_tile(ctr) ~= 0
		ctr.y = ctr.y + 1
		local src = ctr + Vector(-1,1,-1)
		local dst = ctr + Vector(1,3,1)
		ctr = ctr + Vector (0.5, 0, 0.5)
		if math.random() < 0.1 then
			Voxel:place_item{point = ctr, category = "generate"}
		elseif math.random() < 0.4 and Voxel:check_range(src, dst).solid == 0 then
			Voxel:place_obstacle{point = ctr, category = "tree"}
		else
			Voxel:place_obstacle{point = ctr, category = "small-plant"}
		end
	end
	local create_vein = function(ctr, mat)
		for k,v in pairs(points) do
			if math.random() < 0.3 then
				local t = Voxel:get_tile(ctr + v)
				if t ~= 0 then
					Voxel:set_tile(ctr + v, mat)
				end
			end
		end
	end
	-- Generate resources.
	local p = Vector()
	local n = amount or 10
	for i=1,n do
		p.x = pos.x + math.random(1, size.x - 2)
		p.y = pos.y + math.random(1, size.y - 2)
		p.z = pos.z + math.random(1, size.z - 2)
		local t = Voxel:get_tile(p)
		if t ~= 0 then
			local m = mats[math.random(1,#mats)]
			if m then create_vein(p, m.id, 1) end
		else
			create_plant_or_item(p)
		end
	end
end

--- Gets the ID of the sector.
-- @param self Generator.
-- @param sector Sector offset in tiles.
-- @return Vector.
Generator.get_sector_id = function(self, tile)
	local w = 128
	local s = (tile:round() * (1 / Voxel.tiles_per_line)):floor()
	return s.x + s.y * w + s.z * w^2
end

--- Gets the offset of the sector in tiles.
-- @param self Generator.
-- @param sector Sector index.
-- @return Vector.
Generator.get_sector_offset = function(self, sector)
	local w = 128
	local sx = sector % w
	local sy = math.floor(sector / w) % w
	local sz = math.floor(sector / w / w) % w
	return Vector(sx, sy, sz) * Voxel.tiles_per_line
end

--- Informs clients of the generator status.
-- @param self Generator.
-- @param client Specific client to inform or nil to inform all.
-- @return Network packet.
Generator.inform_clients = function(self, client)
	local p = Packet(packets.GENERATOR_STATUS,
		"string", self.prev_message or "",
		"float", self.prev_fraction or 0)
	if client then
		Network:send{client = client, packet = p}
	else
		for k,v in pairs(Network.clients) do
			Network:send{client = v, packet = p}
		end
	end
end

--- Marks road sectors.
-- @param self Generator.
-- @param src Source point in tiles.
-- @param dst Destination point in tiles.
Generator.mark_road = function(self, src, dst)
	local dist
	local ssec = (src * (1/Voxel.tiles_per_line)):round()
	local dsec = (dst * (1/Voxel.tiles_per_line)):round()
	local psec = ssec:copy()
	repeat
		-- Mark as road.
		local sec = self:get_sector_id(psec * Voxel.tiles_per_line)
		self.sectors[sec] = "Road"
		dist = (dsec - psec):round()
		if dist.length < 1 then break end
		-- Move horizontally.
		if math.abs(dist.x) > math.abs(dist.z) then
			if dist.x > 0 then
				psec.x = psec.x + 1
			else
				psec.x = psec.x - 1
			end
		else
			if dist.z > 0 then
				psec.z = psec.z + 1
			else
				psec.z = psec.z - 1
			end
		end
		-- Move vertically.
		-- FIXME: All in the beginning.
		if dist.y < -0.5 then
			psec.y = psec.y - 1
		elseif dist.y > 0.5 then
			psec.y = psec.y + 1
		end
	until false
	-- Mark the endpoints.
	self.sectors[self:get_sector_id(src)] = "Road"
	self.sectors[self:get_sector_id(dst)] = "Road"
end

--- Draws a corridor in the map.
-- @param self Generator.
-- @param src Source point in tiles.
-- @param dst Destination point in tiles.
Generator.paint_corridor = function(self, src, dst)
	local vec = dst - src
	local abs = Vector(math.abs(vec.x), math.abs(vec.y), math.abs(vec.z))
	-- Draw the tunnel line.
	if abs.x >= abs.y and abs.x >= abs.z then
		-- Walk along the X axis.
		local step = vec * (1/abs.x)
		for x=0,abs.x do
			Voxel:place_pattern{category = "corridorx", point = (src + step * x):floor()}
		end
	elseif abs.y >= abs.z then
		-- Walk along the Y axis.
		local step = vec * (1/abs.y)
		for y=0,abs.y do
			Voxel:place_pattern{category = "corridory", point = (src + step * y):floor()}
		end
	else
		-- Walk along the Z axis.
		local step = vec * (1/abs.z)
		for z=0,abs.z do
			Voxel:place_pattern{category = "corridorz", point = (src + step * z):floor()}
		end
	end
	-- Draw the endpoints.
	Voxel:place_pattern{category = "corridor", point = src}
	Voxel:place_pattern{category = "corridor", point = dst}
end

--- Places a region to the map.
-- @param self Generator.
-- @param reg Region spec.
-- @param pat Pattern spec.
-- @return Region or nil.
Generator.place_region = function(self, reg, pat)
	if not pat then
		error(string.format("ERROR: No pattern was found for region `%s'.", reg.name))
	end
	local size = pat.size
 	-- Determine the approximate position.
	-- Regions can be placed randomly or relative to each other. Here we
	-- decide the range of positions that are valid for the region.
	local rel = nil
	local dist = nil
	if reg.position then
		rel = Vector(reg.position[1], 0, reg.position[2])
	elseif not reg.distance then
		rel = Vector(math.random(self.map_start.x, self.map_end.x), 0, math.random(self.map_start.z, self.map_end.z))
		dist = {nil, 0.1 * self.map_size.x, 0.1 * self.map_size.x}
	elseif self.regions_dict_name[reg.distance[1]] then
		rel = self.regions_dict_name[reg.distance[1]].point
		rel = Vector(rel.x, 0, rel.z)
		dist = reg.distance
	else return end
	-- Determine the final position.
	-- Regions with a random position range are placed at different
	-- positions until one is found where they fit. Regions with fixed
	-- positions are simply placed there without any checks.
	local pos = Vector()
	local aabb = Aabb{point = pos, size = size + Vector(2,2,2)}
	if dist then
		local success
		for retry=1,50 do
			pos.x = math.random(dist[2], dist[3])
			pos.y = math.random(reg.depth[1], reg.depth[2])
			pos.z = math.random(dist[2], dist[3])
			if math.random(0, 1) == 1 then pos.x = -pos.x end
			if math.random(0, 1) == 1 then pos.z = -pos.z end
			pos = pos + rel
			aabb.point = pos - Vector(1,1,1)
			if self:validate_region_position(aabb) then
				success = true
				break
			end
		end
		if not success then return end
	else
		pos.x = rel.x
		pos.y = math.random(reg.depth[1], reg.depth[2])
		pos.z = rel.z
		aabb.point = pos - Vector(5,5,5)
	end
	-- Create the region.
	local region = Region{aabb = aabb, pattern = pat, point = pos, size = size, spec = reg}
	self:add_region(region)
	return region
end

Generator.reset = function(self)
	self.bins = {}
	self.links = {}
	self.regions_dict_id = {}
	self.regions_dict_name = {}
	self.regions_dict_layer = {}
	for i=1,self.layer_count do
		self.regions_dict_layer[i] = {}
	end
	self.sectors = {}
end

--- Updates the network status while the generator is active.
-- @param self Generator.
Generator.update_network = function(self)
	Network:update()
	while true do
		local event = Program:pop_event()
		if not event then break end
		if event.type == "login" then
			self:inform_clients(event.client)
		end
	end
end

--- Updates the status message of the generator.
-- @param self Generator.
-- @param frac Fraction of the task completed.
-- @param msg Message string.
Generator.update_status = function(self, frac, msg)
	if msg then
		print(math.ceil(frac * 100) .. "% " .. msg)
		self.prev_message = msg
		self.prev_fraction = frac
		self:inform_clients()
		self:update_network()
	elseif frac == 1 or frac > self.prev_fraction + 0.05 then
		print(math.ceil(frac * 100) .. "% " .. self.prev_message)
		self.prev_fraction = frac
		self:inform_clients()
		self:update_network()
	end
end

Generator.validate_rect = function(self, pos, size)
	if pos.x < self.map_start.x then return end
	if pos.y < self.map_start.y then return end
	if pos.z < self.map_start.z then return end
	if pos.x + size.x > self.map_end.x then return end
	if pos.y + size.y > self.map_end.y then return end
	if pos.z + size.z > self.map_end.z then return end
	local hit
	self:for_each_bin(pos, size, function(i)
		hit = hit or self.bins[i]
	end)
	return not hit
end

--- Checks if a region can be placed in the given position.
-- @param self Generator.
-- @param aabb Position of the region.
-- @return True if the position is valid.
Generator.validate_region_position = function(self, aabb)
	return self:validate_rect(aabb.point, aabb.size)
end

------------------------------------------------------------------------------

Generator.inst = Generator()

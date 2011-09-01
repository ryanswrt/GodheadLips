Sectors.seed1 = math.random(10000, 60000)
Sectors.seed2 = math.random(10000, 60000)

--- Gets the offset of the sector in tiles.
-- @param self Sectors.
-- @param sector Sector index.
-- @return Vector.
Sectors.get_sector_offset = function(self, sector)
	local w = 128
	local sx = sector % w
	local sy = math.floor(sector / w) % w
	local sz = math.floor(sector / w / w) % w
	return Vector(sx, sy, sz) * Voxel.tiles_per_line
end

Sectors.create_fractal_regions = function(self, sector, filter)
	local org = Sectors.instance:get_sector_offset(sector)
	if filter then
		-- Fill valid blocks.
		local w = 4
		local size = Vector(w,w,w)
		local aabb = Aabb{size = size}
		for x = 0,5 do aabb.point.x = org.x + w*x
			for y = 0,5 do aabb.point.y = org.y + w*y
				for z = 0,5 do aabb.point.z = org.z + w*z
					if filter(aabb) then
						Sectors.instance:create_fractal_terrain(aabb.point, aabb.size)
					end
				end
			end
		end
	else
		-- Fill the whole sector.
		local w = Voxel.tiles_per_line
		local size = Vector(w,w,w)
		Sectors.instance:create_fractal_terrain(org, size)
	end
end

Sectors.create_fractal_terrain = function(self, org, size)
	-- Create common tiles.
	local mats = {
		Material:find{name = "ferrostone1"},
		Material:find{name = "sand1"},
		Material:find{name = "soil1"}}
	local m = mats[math.random(1,#mats)]
	if m then
		local scale = Vector(0.3,0.3,0.3)
		Noise:perlin_terrain(org, org + size, m.id, 0.5, scale, 4, 4, 0.25, self.seed2)
	end
	-- Create caverns.
	local scale = Vector(0.15,0.3,0.15)
	local p = Noise:perlin_terrain(org, org + size, 0, 0.2, scale, 2, 2, 0.3, self.seed1)
end

--- Called when a sector is created by the game.
-- @param self Sectors.
-- @param sector Sector index.
-- @param terrain True if terrain was loaded.
-- @param objects Array of objects.
Sectors.created_sector = function(self, sector, terrain, objects)
	-- Create fractal terrain for newly found sectors.
	if not terrain then Generator.Main:generate(sector) end
	-- Don't spawn monsters in town sectors.
	local s = Generator.inst.sectors[sector]
	if s == "Town" then return end
	-- Decide how many monsters to spawn.
	-- The sector size is quite small so we spawn 1 monster per sector most
	-- of the time, none second often and 2 least often.
	local r = math.random()
	if r < 0.35 then return end
	local monsters = (r < 0.85) and 1 or 2
	-- Count monsters.
	-- If the sector already contains monsters, reduce the number of newly
	-- spawned monsters accodingly to not cramp the sector.
	for k,obj in pairs(objects) do
		if obj.spec.type == "species" then
			monsters = monsters - 1
			if monsters == 0 then return end
		end
	end
	-- Spawn monsters.
	-- This is done in a thread to reduce pauses when lots of sectors are
	-- being loaded. It's useful since sectors are often loaded in clusters.
	local org = self:get_sector_offset(sector)
	local spawn = Config.inst.spawn_point * Voxel.tile_scale
	Coroutine(function(thread)
		for i = 1,monsters do
			for j = 1,15 do
				local c = Vector()
				c.x = org.x + math.random(4, Voxel.tiles_per_line - 4)
				c.y = org.y + math.random(4, Voxel.tiles_per_line - 4)
				c.z = org.z + math.random(4, Voxel.tiles_per_line - 4)
				local p = Utils:find_spawn_point(c * Voxel.tile_size)
				if p then
					local d = math.min(1, (p - spawn).length / 500)
					Voxel:place_creature{point = p * Voxel.tile_scale, category = "enemy", difficulty = d}
					break
				end
				coroutine.yield()
			end
			coroutine.yield()
		end
	end)
end

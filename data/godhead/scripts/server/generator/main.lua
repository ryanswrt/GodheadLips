require "server/generator"
require "server/generator/border"
require "server/generator/dungeon"
require "server/generator/forest"
require "server/generator/ocean"
require "server/generator/road"
require "server/generator/ruins"
require "server/generator/town"
require "server/generator/volcano"
require "server/generator/heightmap"


Generator.Main = Class()
Generator.Main.class_name = "Generator.Main"
Generator.Main.scale1 = Vector(1,1,1) * (0.01 * Voxel.tile_scale)

--- Generates a sector of a suitable type.
-- @param self Main generator.
-- @param sector Sector index.
Generator.Main.generate = function(self, sector)
	local w = Voxel.tiles_per_line
	local size = Vector(w,w,w)
	local pos = Generator.inst.inst:get_sector_offset(sector)
	local t = self:get_sector(pos, size)
	local g = Generator[t]

	if pos.y > 1007 then return end
	if not g then
        print("Void travel")
		Voxel:fill_region{point = pos, size = size, tile = 0}
	else
		g:generate(pos, size)
	end
end

Generator.Main.get_cluster = function(self, pos, size)
	local s = Voxel.tiles_per_line
	local a = {}
	for x = -1,1 do
		a[x] = {}
		for y = -1,1 do
			a[x][y] = {}
			for z = -1,1 do
				a[x][y][z] = self:get_sector(pos + Vector(x*s,y*s,z*s), size)
			end
		end
	end
	return a
end

Generator.Main.get_sector = function(self, pos, size)
	-- Predefined sectors.
	-- The map generator allocates some sectors for towns. They're formatted
	-- using a specific town generator.
	local s = Generator.inst.sectors[Generator.inst:get_sector_id(pos)]
	if s then return s end
 	if not Generator.inst.inst:validate_rect(pos, size) then return "Heightmap" end
 	-- Map boundaries.
 	-- The map is surrounded by special border sectors to prevent things from
 	-- falling outside of the map.
 	local min = 10 * Voxel.tiles_per_line
 	local max = 100 * Voxel.tiles_per_line
 	if pos.x < min or pos.y < min or pos.z < min or pos.x > max or pos.y > max or pos.z > max then return "Border" end
-- 	-- Random ruins.
-- 	-- Ruins can appear anywhere so their generation is a bit special. They're
-- 	-- generated if the random number falls inside one of a few gaps in the range.
-- 	local n1 = Noise:perlin_noise(pos, self.scale1, 1, 3, 0.5, Generator.inst.seed1)
-- 	local n2 = math.abs(3 * n1)
-- 	if math.ceil(n2) - n2 < 0.2 then return "Ruins" end
-- 	-- Random sectors.
-- 	-- Other sector types are allocated a specific range inside which they appear.
-- 	-- TODO: The range should depend on the depth so that magma and water are more common in specific map layers.
-- 	if n1 < -0.9 then return "Volcano" end
-- 	if n1 > 0.8 then return "Ocean" end
-- 	if n1 > 0.2 then return "Forest" end
	return "Heightmap"
end

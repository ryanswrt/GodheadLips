require "server/generator"

Generator.Town = Class()
Generator.Town.class_name = "Generator.Town"
Generator.Town.scale1 = Vector(0.3,0.3,0.3)
Generator.Town.scale2 = Vector(0.15,0.3,0.15)
Generator.Town.mats = {
	Material:find{name = "granite1"},
	Material:find{name = "soil1"},
	Material:find{name = "grass1"}}

--- Generates a town sector.
-- @param self Town generator.
-- @param pos Offset of the generated area.
-- @param size Size of the generated area.
Generator.Town.generate = function(self, pos, size)
	-- Create terrain.
	Voxel:fill_region{point = pos, size = size, tile = 0}
	Noise:perlin_terrain(pos, pos + size, self.mats[1].id, 0.1, self.scale1, 4, 4, 0.1, Generator.inst.seed1)
	Noise:perlin_terrain(pos, pos + size, self.mats[2].id, 0.35, self.scale1, 4, 4, 0.15, Generator.inst.seed1)
	Noise:perlin_terrain(pos, pos + size, self.mats[3].id, 0.45, self.scale1, 4, 4, 0.2, Generator.inst.seed1)
end

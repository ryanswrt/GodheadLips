require "server/generator"

Generator.Volcano = Class()
Generator.Volcano.class_name = "Generator.Volcano"
Generator.Volcano.scale1 = Vector(0.3,0.3,0.3)
Generator.Volcano.scale2 = Vector(0.15,0.3,0.15)
Generator.Volcano.mats = {
	Material:find{name = "basalt1"},
	Material:find{name = "magma1"}}

--- Generates a dungeon area.
-- @param self Dungeon generator.
-- @param pos Offset of the generated area.
-- @param size Size of the generated area.
Generator.Volcano.generate = function(self, pos, size)
	-- Create granite.
	local m1 = Material:find{name = "granite1"}
	Voxel:fill_region{point = pos, size = size, tile = m1.id}
	-- Create basalt.
	Noise:perlin_terrain(pos, pos + size, self.mats[1].id, 0.2, self.scale1, 4, 4, 0.25, Generator.inst.seed2)
	-- Create caverns.
	Noise:perlin_terrain(pos, pos + size, 0, 0.15, self.scale2, 2, 2, 0.2, Generator.inst.seed1)
	-- Create magma.
	Noise:perlin_terrain(pos, pos + size, self.mats[2].id, 0.3, self.scale1, 4, 4, 0.25, Generator.inst.seed3)
	-- Create plants and ores.
	Generator.inst:generate_resources(pos, size, 5)
end

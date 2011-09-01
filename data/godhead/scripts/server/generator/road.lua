require "server/generator"

Generator.Road = Class()
Generator.Road.class_name = "Generator.Road"
Generator.Road.scale1 = Vector(0.3,0.3,0.3)
Generator.Road.scale2 = Vector(0.15,0.3,0.15)
Generator.Road.mats = {
	Material:find{name = "soil1"},
	Material:find{name = "grass1"},
	Material:find{name = "sand1"}}

--- Generates a town sector.
-- @param self Town generator.
-- @param pos Offset of the generated area.
-- @param size Size of the generated area.
Generator.Road.generate = function(self, pos, size)
	-- Create granite.
	local m1 = Material:find{name = "granite1"}
	Voxel:fill_region{point = pos, size = size, tile = m1.id}
	-- Create common tiles.
	-- FIXME: The distribution should be much more random.
	local m = self.mats[math.random(1,#self.mats)]
	if m then
		Noise:perlin_terrain(pos, pos + size, m.id, 0.5, self.scale1, 4, 4, 0.25, Generator.inst.seed2)
	end
	-- Create caverns.
	Noise:perlin_terrain(pos, pos + size, 0, 0.1, self.scale2, 2, 2, 0.3, Generator.inst.seed1)
	-- Create plants and ores.
	Generator.inst:generate_resources(pos, size, 5)
end

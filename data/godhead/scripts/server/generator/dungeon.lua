require "server/generator"

Generator.Dungeon = Class()
Generator.Dungeon.class_name = "Generator.Dungeon"
Generator.Dungeon.scale1 = Vector(0.6,0.6,0.6)
Generator.Dungeon.scale2 = Vector(0.3,0.6,0.3)
Generator.Dungeon.mats = {
	Material:find{name = "ferrostone1"},
	Material:find{name = "sand1"},
	Material:find{name = "soil1"}}

--- Generates a dungeon area.
-- @param self Dungeon generator.
-- @param pos Offset of the generated area.
-- @param size Size of the generated area.
Generator.Dungeon.generate = function(self, pos, size)
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
	Noise:perlin_terrain(pos, pos + size, 0, 0.1, self.scale2, 5, 5, 0.1, Generator.inst.seed1)
	-- Create plants and ores.
	Generator.inst:generate_resources(pos, size)
end

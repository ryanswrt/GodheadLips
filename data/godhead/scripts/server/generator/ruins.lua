require "server/generator"

Generator.Ruins = Class()
Generator.Ruins.class_name = "Generator.Ruins"
Generator.Ruins.scale1 = Vector(0.3,0.3,0.3)
Generator.Ruins.scale2 = Vector(0.15,0.3,0.15)
Generator.Ruins.pats = Pattern:find{category = "ruins"}
Generator.Ruins.mats = {
	Material:find{name = "ferrostone1"},
	Material:find{name = "sand1"},
	Material:find{name = "soil1"}}

--- Generates a ruins area.
-- @param self Ruins generator.
-- @param pos Offset of the generated area.
-- @param size Size of the generated area.
Generator.Ruins.generate = function(self, pos, size)
	-- Create granite.
	local m1 = Material:find{name = "granite1"}
	Voxel:fill_region{point = pos, size = size, tile = m1.id}
	-- Create caverns.
	Noise:perlin_terrain(pos, pos + size, 0, 0.2, self.scale2, 2, 2, 0.2, Generator.inst.seed1)
	-- Create ruins.
	-- FIXME: The pattern can overflow out of the sector if it's too big.
	local pat = self.pats[math.random(1, #self.pats)]
	Voxel:fill_region{point = pos, size = pat.size} 
	Voxel:place_pattern{name = pat.name, point = pos, rotation = math.random(0, 3)}
end

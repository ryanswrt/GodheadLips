--require "server/generator"

Generator.Heightmap = Class()
Generator.Heightmap.class_name = "Generator.Heightmap"
Generator.Heightmap.map = "island.bmp"
Generator.Heightmap.tiles = "island_tiles.bmp"
Generator.Heightmap.mats = {
    Material:find{name = "basalt1"},        -- default  0x00
    Material:find{name = "magma1"},         -- vulcano  0x01
	Material:find{name = "ice1"},           -- mountain 0x02
	Material:find{name = "water1"},         -- ocean    0x03
	Material:find{name = "granite1"},       -- rocks    0x04
	Material:find{name = "soil1"},          -- ground   0x05
	Material:find{name = "grass1"},         -- plains   0x06
    Material:find{name = "ferrostone1"},    -- underground  0x07
    Material:find{name = "sand1"}}          -- beach    0x08

--- Generates a dungeon area.
-- @param self Heightmap generator.
Generator.Heightmap.generate = function(self, pos, size)
    -- Fill the void
    Voxel:fill_region{point = pos, size = size, tile = 0}
    -- Create the ground for minimal height
	local m1 = Material:find{name = "granite1"}
    local sz = Vector( size.x, size.y, 1 )
	Voxel:fill_region{point = pos, size = sz, tile = m1.id}
    -- Load the actual terrain on top of it
    -- TODO: add offset and scale (interpolation)
	Heightmap:heightmap_load(self.map, self.tiles, pos.handle, size.handle, self.mats)
end

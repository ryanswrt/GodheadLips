--require "server/generator"

Generator.Heightmap = Class()
Generator.Heightmap.class_name = "Generator.Heightmap"
Generator.Heightmap.map = "island.bmp"
Generator.Heightmap.tiles = "island_tiles.bmp"
Generator.Heightmap.offset = Vector (0, 0, 0) -- TODO
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
    print("Pos.y :" .. pos.y)
    -- Fill the void
    Voxel:fill_region{point = pos, size = size, tile = 0}
    if pos.y < 120 then
        -- Create the ground for minimal height
        local m1 = Material:find{name = "granite1"}
        local s1 = Vector( size.x, 120-pos.y, size.z )
        if s1.y < Voxel.tiles_per_line then
            s1.y = Voxel.tiles_per_line
        end
        if 120-pos.y > size.y then
            s1.y = size.y
        end
        Voxel:fill_region{point = pos, size = s1, tile = m1.id}
    else
        -- TODO: add offset and scale (interpolation)
        Heightmap:heightmap_load(self.map, self.tiles, pos, size, self.mats)
    end
end

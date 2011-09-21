if not Los.program_load_extension("heightmap") then
	error("loading extension `heightmap' failed")
end

Heightmap = Class()
Heightmap.class_name = "Heightmap"

--- Creates voxel terrain out of an heightmap.
-- @param self      Heightmap class.
-- @param map       Filename of the height texture (FIX ME: use texture instead, if I can read it from the GPU ...)
-- @param tiles     Filename of the tiles texture (FIX ME: use texture instead, if I can read it from the GPU ...)
-- @param pos       Start position vector.
-- @param size      Size vector.
-- @param materials array of mat ID to pick from.
Heightmap.heightmap_load = function(self, map, tiles, pos, size, materials)
    if materials ~= nil then
        local pos2 = Vector(pos.x / Voxel.tiles_per_line, pos.y / Voxel.tiles_per_line, pos.z / Voxel.tiles_per_line)
        local size2 = Vector(size.x / Voxel.tiles_per_line, size.y / Voxel.tiles_per_line, size.z / Voxel.tiles_per_line)
        Los.heightmap_generate(map, tiles, pos2.handle, size2.handle, materials)
    end
end

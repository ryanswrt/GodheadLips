if not Los.program_load_extension("heightmap") then
	error("loading extension `heightmap' failed")
end

Heightmap = Class()
Heightmap.class_name = "Heightmap"

--- Creates voxel terrain out of an heightmap.
-- @param self      Heightmap class.
-- @param map       Filename of the height texture (FIX ME: use texture instead, if I can read it from the GPU ...)
-- @param tiles     Filename of the tiles texture (FIX ME: use texture instead, if I can read it from the GPU ...)
-- @param pos       Start position vector.</li>
-- @param size      Size vector. The texture image must fit.</li>
-- @param materials array of textures to pick from. They must be of equal size.</li>
Heightmap.heightmap_load = function(self, map, tiles, pos, size, materials)
    print("blah")
    if materials ~= nil then
        Los.heightmap_generate(map, tiles, pos, size, materials)
    end
end

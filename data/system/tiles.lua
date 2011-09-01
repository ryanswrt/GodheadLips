require "system/class"

if not Los.program_load_extension("tiles") then
	error("loading extension `tiles' failed")
end

------------------------------------------------------------------------------

Material = Class()
Material.class_name = "Material"

--- Creates a new material.
-- @param clss Material class.
-- @param args Arguments.
-- @return New material.
Material.new = function(clss, args)
	local self = Class.new(clss)
	self.handle = Los.material_new()
	__userdata_lookup[self.handle] = self
	if args then
		for k,v in pairs(args) do self[k] = v end
	end
	return self
end

--- Face culling toggle.
-- @name Material.cullface
-- @class table

--- Diffuse color.
-- @name Material.diffuse
-- @class table

--- Material flags.
-- @name Material.flags
-- @class table

--- Material friction.
-- @name Material.friction
-- @class table

--- Material ID.
-- @name Material.id
-- @class table

--- Material name.
-- @name Material.name
-- @class table

--- Surface shader.
-- @name Material.shader
-- @class table

--- Shininess.
-- @name Material.shininess
-- @class table

--- Specular color.
-- @name Material.specular
-- @class table

--- Texture list.
-- @name Material.texture
-- @class table

--- Texture scale factor.
-- @name Material.texture_scale
-- @class table

--- Material type.
-- @name Material.type
-- @class table

Material:add_getters{
	cullface = function(s) return Los.material_get_cullface(s.handle) end,
	diffuse = function(s) return Los.material_get_diffuse(s.handle) end,
	flags = function(s) return Los.material_get_flags(s.handle) end,
	friction = function(s) return Los.material_get_friction(s.handle) end,
	id = function(s) return Los.material_get_id(s.handle) end,
	name = function(s) return Los.material_get_name(s.handle) end,
	shader = function(s) return Los.material_get_shader(s.handle) end,
	shininess = function(s) return Los.material_get_shininess(s.handle) end,
	specular = function(s) return Los.material_get_specular(s.handle) end,
	texture = function(s) return Los.material_get_texture(s.handle) end,
	texture_scale = function(s) return Los.material_get_texture_scale(s.handle) end,
	type = function(s) return Los.material_get_type(s.handle) end}

Material:add_setters{
	cullface = function(s, v) Los.material_set_cullface(s.handle, v) end,
	diffuse = function(s, v) Los.material_set_diffuse(s.handle, v) end,
	flags = function(s, v) Los.material_set_flags(s.handle, v) end,
	friction = function(s, v) Los.material_set_friction(s.handle, v) end,
	name = function(s, v) Los.material_set_name(s.handle, v) end,
	shader = function(s, v) Los.material_set_shader(s.handle, v) end,
	shininess = function(s, v) Los.material_set_shininess(s.handle, v) end,
	specular = function(s, v) Los.material_set_specular(s.handle, v) end,
	texture = function(s, v) Los.material_set_texture(s.handle, v) end,
	texture_scale = function(s, v) Los.material_set_texture_scale(s.handle, v) end,
	type = function(s, v) Los.material_set_type(s.handle, v) end}

Material.unittest = function()
	local m = Material()
	assert(type(m.cullface) == "boolean")
	assert(type(m.diffuse) == "table")
	assert(type(m.flags) == "number")
	assert(type(m.friction) == "number")
	assert(type(m.id) == "number")
	assert(type(m.name) == "string")
	assert(type(m.shader) == "string")
	assert(type(m.shininess) == "number")
	assert(type(m.specular) == "table")
	assert(type(m.texture) == "table")
	assert(type(m.texture_scale) == "number")
	assert(type(m.type) == "string")
end

------------------------------------------------------------------------------

Voxel = Class()

--- Copies a terrain region into a packet.
-- @param self Voxel class.
-- @param args Arguments.<ul>
--   <li>point: Tile index vector.</li>
--   <li>sector: Sector index.</li>
--   <li>size: Region size, in tiles.</li></ul>
-- @return Packet writer.
Voxel.copy_region = function(self, args)
	local handle = Los.voxel_copy_region{point = args.point and args.point.handle, sector = args.sector, size = args.size}
	return Class.new(Packet, {handle = handle})
end

--- Fills a terrain region.
-- @param self Voxel class.
-- @param args Arguments.<ul>
--   <li>point: Tile index vector.</li>
--   <li>size: Size vector.</li>
--   <li>tile: Tile data.</ul>
Voxel.fill_region = function(self, args)
	return Los.voxel_fill_region{point = args.point.handle, size = args.size.handle, tile = args.tile}
end

--- Finds all blocks near the given point.
-- @param self Voxel class.
-- @param args Arguments.<ul>
--   <li>point: Position vector.</li>
--   <li>radius: Radius.</li></ul>
-- @return Table of block indices and modification stamps.
Voxel.find_blocks = function(self, args)
	return Los.voxel_find_blocks{point = args.point.handle, radius = args.radius}
end

--- Finds the tile nearest to the given point.
-- @param self Voxel class.
-- @param args Arguments.<ul>
--   <li>match: Tiles to search for. ("all"/"empty"/"full")</li>
--   <li>point: Position vector in world space.</li>
--   <li>radius: Search radius in tiles.</li></ul>
-- @return Tile and voxel index vector, or nil.
Voxel.find_tile = function(self, args)
	local t,p = Los.voxel_find_tile{match = args.match, point = args.point.handle, radius = args.radius}
	if not t then return end
	return t, Class.new(Vector, {handle = p})
end

--- Gets the data of a voxel block.
-- @param self Voxel class.
-- @param args Arguments.<ul>
--   <li>index: Block index.</li>
--   <li>packet: Packet writer.</li>
--   <li>type: Packet type.</li></ul>
-- @return Packet writer or nil.
Voxel.get_block = function(self, args)
	local handle = Los.voxel_get_block{index = args.index, packet = args.packet and args.packet.handle, type = args.type}
	if args.packet then return args.packet end
	return Class.new(Packet, {handle = handle})
end

--- Gets the contents of a tile.
-- @param self Voxel class.
-- @param args Arguments.<ul>
--   <li>1: Tile index vector.</li></ul>
-- @return Tile.
Voxel.get_tile = function(self, a)
	return Los.voxel_get_tile(a.handle)
end

--- Intersects a ray with map tiles.
-- @param self Object.
-- @param ... Arguments.<ul>
--   <li>1,start_point: Ray start point in world space.</li>
--   <li>2,end_point: Ray end point in world space.</li></ul>
-- @return Position vector in world space, tile index vector.
Voxel.intersect_ray = function(self, ...)
	local a,b = ...
	if not a.class then
		return Los.voxel_intersect_ray(a.handle, b.handle)
	else
		return Los.voxel_intersect_ray(a.start_point.handle, a.end_point.handle)
	end
end

--- Pastes a terrain region from a packet to the map.
-- @param self Voxel class.
-- @param args Arguments.<ul>
--   <li>packet: Data packet.</li>
--   <li>point: Tile index vector.</li>
--   <li>sector: Sector index.</li></ul>
Voxel.paste_region = function(self, args)
	Los.voxel_paste_region{packet = args.packet.handle, point = args.point and args.point.handle, sector = args.sector}
end

--- Update the voxel terrain state.
-- @param self Voxel class.
-- @param secs Seconds since the last update.
Voxel.update = function(self, secs)
	Los.voxel_update(secs)
end

--- Sets the contents of a block of voxels.
-- @param self Voxel class.
-- @param args Arguments.<ul>
--   <li>packet: Packet reader.</li></ul>
-- @return True on success.
Voxel.set_block = function(self, args)
	return Los.voxel_set_block{packet = args.packet.handle}
end

--- Sets the contents of a tile.
-- @param self Voxel class.
-- @param args Arguments.<ul>
--   <li>1: Tile index vector.</li>
--   <li>2: Tile number.</li></ul>
Voxel.set_tile = function(self, a, b)
	return Los.voxel_set_tile(a.handle, b)
end

--- Sets the contents of multiple tiles.
-- @param self Voxel class.
-- @param args Arguments.<ul>
--   <li>1: Tile number.</li>
--   <li>2: List of tile index vectors.</li></ul>
Voxel.set_tiles = function(self, a, b)
	local h = {}
	for k,v in pairs(b) do h[k] = b[k].handle end
	return Los.voxel_set_tiles(a.handle, h)
end

--- Number of blocks per sector edge.
-- @name Voxel.blocks_per_line
-- @class table

--- Fill type for empty sectors.
-- @name Voxel.fill
-- @class table

--- Approximate memory used by terrain, in bytes.
-- @name Voxel.memory_used
-- @class table

--- List of material IDs.
-- @name Voxel.materials
-- @class table

--- Number of tiles per sector edge.
-- @name Voxel.tiles_per_line
-- @class table

Voxel.class_getters = {
	blocks_per_line = function(s) return Los.voxel_get_blocks_per_line() end,
	fill = function(s) return Los.voxel_get_fill() end,
	materials = function(s) return Los.voxel_get_materials() end,
	memory_used = function(s) return Los.voxel_get_memory_used() end,
	tiles_per_line = function(s) return Los.voxel_get_tiles_per_line() end}

Voxel.class_setters = {
	blocks_per_line = function(s, v) Los.voxel_set_blocks_per_line(v) end,
	fill = function(s, v) Los.voxel_set_fill(v) end,
	tiles_per_line = function(s, v) Los.voxel_set_tiles_per_line(v) end}

Voxel.unittest = function()
	-- Getting and setting tiles.
	assert(Voxel:get_tile(Vector(100,101,102)) == 0)
	Voxel:set_tile(Vector(100,101,102), 5)
	assert(Voxel:get_tile(Vector(100,101,102)) == 5)
end

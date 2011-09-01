Voxel.tile_size = Program.sector_size / Voxel.tiles_per_line
Voxel.tile_scale = 1 / Voxel.tile_size

--- Checks what kind of tiles are within the range.
-- @param src Tile range start.
-- @param dst Tile range end.
-- @return Table of scan result.
Voxel.check_range = function(clss, src, dst)
	local result = {empty = 0, liquid = 0, magma = 0, solid = 0, total = 0}
	local v = Vector()
	for x = src.x,dst.x do
		v.x = x
		for y = src.y,dst.y do
			v.y = y
			for z = src.z,dst.z do
				v.z = z
				local t = clss:get_tile(v)
				if t == 0 then
					result.empty = result.empty + 1
				else
					local m = Material:find{id = t}
					if m and m.type == "liquid" then
						result.liquid = result.liquid + 1
						if m.magma then
							result.magma = result.magma + 1
						end
					else
						result.solid = result.solid + 1
					end
				end
				result.total = result.total + 1
			end
		end
	end
	return result
end

--- Places a monster to the map.
-- @param clss Voxel class.
-- @param args Arguments.<ul>
--   <li>category: Species category.</li>
--   <li>diffuculty: Maximum difficulty of the creature.</li>
--   <li>name: Species name.</li>
--   <li>point: Position vector, in tiles.</li>
--   <li>rotation: Rotation around Y axis.</li></ul>
Voxel.place_creature = function(clss, args)
	-- Choose the species.
	-- The species can by explicitly named or randomly selected from a specific
	-- category. Selection from a category can also optionally be limited by the
	-- maximum difficulty of the creature.
	local spec
	if args.category and args.difficulty then
		local cat = Species:find(args)
		if not cat then return end
		local num = 0
		local opt = {}
		local dif = args.difficulty
		for k,v in pairs(cat) do
			if v.difficulty <= dif then
				num = num + 1
				opt[num] = v
			end
		end
		if num == 0 then return end
		spec = opt[math.random(1, num)]
	else
		spec = Species:random(args)
		if not spec then return end
	end
	-- Spawn the creature.
	-- This needs to support both the client and the server so the class
	-- used varies depending on what's available.
	local clss_ = Creature or Object
	clss_.new(clss_, {
		spec = spec,
		position = args.point * clss.tile_size,
		random = true,
		realized = true,
		rotation = args.rotation and Quaternion{euler = {args.rotation * 2 * math.pi, 0, 0}}})
end

--- Places an item to the map.
-- @param clss Voxel class.
-- @param args Arguments.<ul>
--   <li>category: Item category.</li>
--   <li>name: Item name.</li>
--   <li>point: Position vector, in tiles.</li>
--   <li>rotation: Rotation around Y axis.</li></ul>
Voxel.place_item = function(clss, args)
	local spec = Itemspec:random(args)
	if not spec then return end
	local clss_ = Item or Object
	clss_.new(clss_, {
		spec = spec,
		position = args.point * clss.tile_size,
		random = true,
		realized = true,
		rotation = args.rotation and Quaternion{euler = {args.rotation * 2 * math.pi, 0, 0}}})
end

--- Places an obstacle to the map.
-- @param clss Voxel class.
-- @param args Arguments.<ul>
--   <li>name: Obstacle name.</li>
--   <li>point: Position vector, in tiles.</li>
--   <li>rotation: Rotation around Y axis.</li></ul>
Voxel.place_obstacle = function(clss, args)
	local spec = Obstaclespec:random(args)
	if not spec then return end
	local clss_ = Obstacle or Object
	clss_.new(clss_, {
		spec = spec,
		position = args.point * clss.tile_size,
		realized = true,
		rotation = args.rotation and Quaternion{euler = {args.rotation * 2 * math.pi, 0, 0}}})
end

--- Places a predefined map pattern to the map.
-- @param clss Voxel class.
-- @param args Arguments.<ul>
--   <li>name: Pattern name.</li>
--   <li>point: Position vector, in tiles.</li>
--   <li>rotation: Counter-clockwise rotation in 90 degree steps.</li></ul>
Voxel.place_pattern = function(clss, args)
	local pat = Pattern:random(args)
	if not pat then return end
	-- Initialize rotation
	local coord
	if args.rotation == 1 then
		coord = function(x,y,z) return Vector(z,y,x) end
	elseif args.rotation == 2 then
		coord = function(x,y,z) return Vector(pat.size.x-1-x,y,pat.size.z-1-z) end
	elseif args.rotation == 3 then
		coord = function(x,y,z) return Vector(pat.size.z-1-z,y,pat.size.x-1-x) end
	else
		coord = function(x,y,z) return Vector(x,y,z) end
	end
	-- Create tiles.
	for k,v in pairs(pat.tiles) do
		-- Get tile type.
		local tile = 0
		if v[4] then
			local mat = Material:find{name = v[4]}
			tile = mat and mat.id or 0
		end
		if v[5] then
			-- Fill volume of tiles.
			for x = v[1],v[1]+v[5] do
				for y = v[2],v[2]+v[6] do
					for z = v[3],v[3]+v[7] do
						Voxel:set_tile(args.point + coord(x, y, z), tile)
					end
				end
			end
		else
			-- Fill individual tile.
			Voxel:set_tile(args.point + coord(v[1], v[2], v[3]), tile)
		end
	end
	-- Create obstacles.
	for k,v in pairs(pat.obstacles) do
		local point = args.point + coord(v[1], v[2], v[3])
		clss:place_obstacle{name = v[4], point = point, rotation = v[5]}
	end
	-- Create items.
	for k,v in pairs(pat.items) do
		local point = args.point + coord(v[1], v[2], v[3])
		clss:place_item{name = v[4], point = point, rotation = v[5]}
	end
	-- Create creatures.
	for k,v in pairs(pat.creatures) do
		local point = args.point + coord(v[1], v[2], v[3])
		clss:place_creature{name = v[4], point = point, rotation = v[5]}
	end
end

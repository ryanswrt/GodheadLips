-- FIXME: These need to be cleaned up.

Actions = Class()

Skills.get_total = function(self)
	local n = self:get_names()
	local t = 0
	for k,v in pairs(n) do
		t = t + self:get_maximum{skill = v}
	end
	return t
end

Utils = Class()

--- Checks if there's room for the model in the given point in space.
-- @param clss Utils class.
-- @param point Point in world space.
-- @param model Model name.
-- @return True if there's room for the model.
Utils.check_room = function(clss, point, model)
	-- FIXME: Should rather do physics based testings.
	for k,v in pairs(Object.objects) do
		local d = (v.position - point).length
		if d < 1.5 then return end
	end
	return true
end

--- Finds a buildable point near the given point.
-- @param clss Utils class.
-- @param point Point in world space.
-- @param user Builder creature used for resolving conflicts.
-- @return Point in tiles and point in world space, or nil.
Utils.find_build_point = function(clss, point, user)
	-- Find an empty tile.
	local t,p = Voxel:find_tile{match = "empty", point = point}
	if not t then return end
	-- Return it if the character doesn't intersect with it.
	-- TODO: Should check for other objects as well.
	local tile = Aabb{point = p * Voxel.tile_size, size = Vector(1, 1, 1) * Voxel.tile_size}
	local char = Aabb{point = user.position - Vector(0.5, 0, 0.5), size = Vector(1.5, 2, 1.5)}
	if not tile:intersects(char) then return t,p end
	-- Try to resolve a better position.
	-- If the player is standing on the tile, look for an empty tile in the
	-- direction where she's facing. This makes building bridges easier.
	local above = Voxel:get_tile(p + Vector(0,1))
	local below = Voxel:get_tile(p - Vector(0,1))
	if above ~= 0 then
		p.y = p.y + 1
	elseif below ~= 0 then
		p.y = p.y - 1
	else
		return
	end
	local dir = user.rotation * Vector(0,0,-1)
	if math.abs(dir.x) > math.abs(dir.z) then
		if dir.x < 0 then
			p.x = p.x - 1
		else
			p.x = p.x + 1
		end
	else
		if dir.z < 0 then
			p.z = p.z - 1
		else
			p.z = p.z + 1
		end
	end
	t = Voxel:get_tile(p)
	if t ~= 0 then return end
	return t,p
end

--- Finds empty ground below the given point.
-- @param clss Utils class.
-- @param point Point in world space.
-- @return Point in world units, or nil.
Utils.find_empty_ground = function(clss, point)
	local t,c = Voxel:find_tile{match = "empty", point = point}
	if not t then return end
	for i=1,5 do
		if Voxel:get_tile(c + Vector(0,-i)) ~= 0 then
			return (c + Vector(0.5,1-i,0.5)) * Voxel.tile_size
		end
	end
end

--- Finds a drop point suitable for an item.
-- @param clss Utils class.
-- @param point Point in world space.
-- @return Point in world units, or nil.
Utils.find_drop_point = function(clss, point)
	-- Find an empty tile.
	local t,c = Voxel:find_tile{match = "empty", point = point, radius = Voxel.tile_size}
	if not t then return end
	-- TODO: Try to avoid other objects.
	return (c + Vector(0.5,0.5,0.5)) * Voxel.tile_size
end

--- Finds a spawn point suitable for creatures.
-- @param clss Utils class.
-- @param point Point in world space.
-- @return Point in world units, or nil.
Utils.find_spawn_point = function(clss, point)
	-- Find an empty tile.
	local t,c = Voxel:find_tile{match = "empty", point = point, radius = Voxel.tile_size}
	if not t then return end
	-- Find the floor.
	for i=1,3 do
		if Voxel:get_tile(c + Vector(0,-i)) ~= 0 then
			local p = c + Vector(0,1-i)
			-- Check for horizontal space.
			if i == 1 and Voxel:get_tile(c + Vector(0,1)) ~= 0 then return end
			-- Check for nearby ground.
			if Voxel:get_tile(p - Vector(1,0,0)) ~= 0 and
			   Voxel:get_tile(p + Vector(1,0,0)) ~= 0 and
			   Voxel:get_tile(p - Vector(0,0,1)) ~= 0 and
			   Voxel:get_tile(p + Vector(0,0,1)) ~= 0 then
				return (p + Vector(0.5,0.5,0.5)) * Voxel.tile_size
			end
		end
	end
end

--- Spawns a plant or an item.
-- @param clss Utils class.
-- @param point Point in tiles.
-- @return True if succeeded.
Utils.spawn_plant_or_item = function(clss, point)
	-- Find a spawn point.
	if Voxel:get_tile(point) ~= 0 then return end
	if Voxel:get_tile(point - Vector(0,1)) == 0 then return end
	local p = point + Vector(0.5,0,0.5)
	-- Spawn items.
	if math.random() < 0.02 then
		Voxel:place_item{point = p, category = "generate"}
		return true
	end
	-- Spawn plants.
	local src = p + Vector(-1,1,-1)
	local dst = p + Vector(1,3,1)
	if math.random() < 0.6 and Voxel:check_range(src, dst).solid == 0 then
		Voxel:place_obstacle{point = p, category = "tree"}
	else
		Voxel:place_obstacle{point = p, category = "small-plant"}
	end
	return true
end

--- Creates an explosion.
-- @param clss Utils class.
-- @param point Point in world space.
-- @param radius Radius in tiles.
-- @return True if there's room for the model.
Utils.explosion = function(clss, point, radius)
	local r1 = radius or 1
	local r2 = (r1 + 3) * Voxel.tile_size
	Effect:play{effect = "explosion1", point = point}
	-- Damage nearby tiles.
	local _,ctr = Voxel:find_tile{point = point}
	for x=-r1,r1 do
		for y=-r1,r1 do
			for z=-r1,r1 do
				local o = Vector(x,y,z)
				if o.length < r1 + 0.6 then
					Voxel:damage(nil, ctr + o)
				end
			end
		end
	end
	-- Damage nearby objects.
	for k1,v1 in pairs(Object:find{point = point, radius = r2}) do
		local diff = v1.position - point
		local frac = diff.length / r2
		local mult = 10 * math.min(100, v1.mass)
		local impulse = diff:normalize() * (mult * (1 - 0.3 * frac))
		v1:impulse{impulse = impulse, point = Vector()}
		v1:damaged{amount = 40 * (1 - frac), type = "explosion"}
	end
end

--- Handles interaction between players and terrain.
-- @param self Voxels class.
-- @param player Player object.
-- @param point Contact point.
Voxel.player_contact = function(self, player, point)
	local resist = 30
	while true do
		local t,c = Voxel:find_tile{match = "full", point = point}
		if not c or (c - point).length > 3 then break end
		local m = Material:find{id = t.terrain}
		if not m then break end
		if m.name == "trapdoor-000" then
			Voxel:erase{point = c}
		else
			if m.name == "spikes-000" then
				resist = 0
			end
			break
		end
	end
	return resist
end

Voxel.damage = function(self, user, point)
	local t = Voxel:get_tile(point)
	if t == 0 then return end
	local m = Material:find{id = t}
	if not m then return end
	-- Play collapse effect.
	if m.effect_collapse then
		local center = (point + Vector(0.5,0.5,0.5)) * Voxel.tile_size
		Effect:play{effect = m.effect_collapse, point = center}
	end
	-- Change tile type.
	local n = Material:find{name = m.mining_transform}
	Voxel:set_tile(point, n and n.id or 0)
	-- Create materials.
	if m.mining_materials then
		for k,v in pairs(m.mining_materials) do
			for i = 1,v do
				local spec = Itemspec:find{name = k}
				local item = Item{spec = spec}
				if not user or not user:add_item{object = item} then
					local offset = Vector(math.random(), math.random(), math.random())
					offset = offset * 0.7 + Vector(0.3, 0.3, 0.3)
					item.position = (point + offset) * Voxel.tile_size
					item.realized = true
				end
			end
		end
	end
	-- Spawn random monsters.
	if not n and math.random() < 0.01 then
		local spec = Species:random{category = "mining"}
		if spec then
			local offset = (point + Vector(0.5,0.1,0.5)) * Voxel.tile_size
			local object = Creature{random = true, spec = spec, position = offset, realized = true}
		end
	end
end

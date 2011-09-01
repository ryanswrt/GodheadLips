-- Spawn trees and plants.
Influencespec{name = "wilderness", func = function(feat, info, args, value)
	-- Calculate spell radius.
	local s = math.min(value, 100) / 100
	local r = 4 * s
	if args.attacker.spec.categories["devora"] then
		r = math.ceil(r * 1.5)
	end
	r = 2 + math.max(r,1)
	-- Create grass from soil.
	local soil = Material:find{name = "soil1"}
	local grass = Material:find{name = "grass1"}
	local org = (args.point * Voxel.tile_scale):floor()
	local pos = Vector()
	local hits = {}
	for x=org.x-r,org.x+r do
		pos.x = x
		for z=org.z-r,org.z+r do
			pos.z = z
			local blocked = true
			for y=org.y+r+1,org.y-r,-1 do
				pos.y = y
				local tile = Voxel:get_tile(pos)
				if not blocked then
					if tile == soil.id then
						Voxel:set_tile(pos, grass.id)
					end
					if tile == soil.id or tile == grass.id then
						local vec = Vector(x,y,z)
						table.insert(hits, vec)
					end
				end
				blocked = (tile ~= 0)
			end
		end
	end
	-- Create wilderness obstacles.
	if #hits then
		for i=1,math.ceil(s/25) do
			local k = math.random(1,#hits)
			local p = hits[k]
			if p then
				hits[k] = nil
				local obstspec = Obstaclespec:random{category = "wilderness"}
				if obstspec then
					local pos = (p + Vector(0.5,0.5,0.5)) * Voxel.tile_size
					if Utils:check_room(pos, obstspec.model) then
						local o = Obstacle{position = pos, spec = obstspec,
							random = true, realized = true}
					end
				end
			end
		end
	end
end}

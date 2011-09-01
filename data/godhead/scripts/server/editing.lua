Protocol:add_handler{type = "EDIT_TILE", func = function(args)
	-- Check for permissions.
	local player = Player:find{client = args.client}
	if not player.admin then player:send("You have no permission to do that.") end
	-- Set tile.
	local ok,x,y,z,t = args.packet:read("uint32", "uint32", "uint32", "uint8")
	if ok then
		Voxel:set_tile(Vector(x, y, z), t)
	end
end}

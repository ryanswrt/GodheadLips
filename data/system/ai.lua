require "system/tiles"

if not Los.program_load_extension("ai") then
	error("loading extension `ai' failed")
end

------------------------------------------------------------------------------

Ai = Class()

--- Solves a path between two points.
-- @param self Ai class.
-- @param args Arguments.<ul>
--   <li>start: Start point in tiles.</li>
--   <li>target: End point in tiles.</li></ul>
-- @return Array of vectors or nil.
Ai.solve_path = function(self, args)
	local r = Los.ai_solve_path{start = args.start.handle, target = args.target.handle}
	if r then
		for k,v in pairs(r) do
			r[k] = Class.new(Vector, {handle = v})
		end
	end
	return r
end

Ai.unittest = function()
	-- Create the maze.
	for x=98,102 do
		for z=98,102 do
			Voxel:set_tile(Vector(x,99,z),1)
		end
	end
	Voxel:set_tile(Vector(100,100,99), 1)
	Voxel:set_tile(Vector(100,100,100), 1)
	Voxel:set_tile(Vector(100,100,101), 1)
	assert(Voxel:get_tile(Vector(100,100,100)) == 1)
	Voxel:update()
	-- Path solving.
	local p = Ai:solve_path{start = Vector(99,100,100), target = Vector(101,100,100)}
	assert(#p == 5)
end

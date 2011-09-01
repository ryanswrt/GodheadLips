local catch = function(f)
	local s,e = pcall(f)
	if not s then print(e) end
end

------------------------------------------------------------------------------

Los.program_unittest()

require "system/class"
catch(function() Class.unittest() end)

require "system/core"
catch(function() Program.unittest() end)

require "system/thread"
catch(function() Thread.unittest() end)

require "system/file"
catch(function() File.unittest() end)

require "system/math"
catch(function() Vector.unittest() end)
catch(function() Quaternion.unittest() end)

require "system/model"
catch(function() Model.unittest() end)

require "system/object"
require "system/object-physics"
Physics.enable_simulation = true
catch(function() Object.unittest() end)

require "system/tiles"
require "system/ai"
catch(function() Material.unittest() end)
catch(function() Voxel.unittest() end)
catch(function() Ai.unittest() end)

require "system/database"
catch(function() Database.unittest() end)

require "system/vision"
catch(function() Vision.unittest() end)

require "system/password"
catch(function() Password.unittest() end)

require "system/graphics"
-- TODO

require "system/widgets"
catch(function() Widget.unittest() end)

require "system/render"
catch(function() Light.unittest() end)

-- Checks for valgrind.
require "system/model-editing"
require "system/object-render"
local create_cube_model = function(aabb)
	-- Calculate the vertices of the bounding box.
	local min = aabb.point
	local max = aabb.point + aabb.size
	local p = {
		{min.x,min.y,min.z}, {max.x,min.y,min.z}, {min.x,max.y,min.z}, {max.x,max.y,min.z},
		{min.x,min.y,max.z}, {max.x,min.y,max.z}, {min.x,max.y,max.z}, {max.x,max.y,max.z}}
	-- Face creation helpers.
	local i = 1
	local v = {}
	local addface = function(a,b,c,d,n)
		v[i] = {a[1],a[2],a[3],n[1],n[2],n[3]}
		v[i+1] = {b[1],b[2],b[3],n[1],n[2],n[3]}
		v[i+2] = {d[1],d[2],d[3],n[1],n[2],n[3]}
		v[i+3] = v[i]
		v[i+4] = v[i+2]
		v[i+5] = {c[1],c[2],c[3],n[1],n[2],n[3]}
		i = i + 6
	end
	-- Left and right.
	addface(p[1], p[3], p[5], p[7], {-1,0,0})
	addface(p[2], p[4], p[6], p[8], {1,0,0})
	-- Bottom and top.
	addface(p[1], p[2], p[5], p[6], {0,-1,0})
	addface(p[3], p[4], p[7], p[8], {0,1,0})
	-- Front and back.
	addface(p[1], p[2], p[3], p[4], {0,0,-1})
	addface(p[5], p[6], p[7], p[8], {0,0,1})
	-- Create the model.
	local model = Model()
	model:add_material{cull = false, shader = "default"}
	model:add_triangles{material = 1, vertices = v}
	model:changed()
	return model
end
for i=1,100 do
	local m1 = create_cube_model(Aabb{point = Vector(), size = Vector(3,3,3)})
	local m2 = create_cube_model(Aabb{point = Vector(), size = Vector(6,6,6)})
	local m = m1:copy()
	m:merge(m2)
	local o = Object{model = m, realized = true}
end

-- Checks for valgrind.
require "system/tiles-render"
local mat = Material{name = "test1", shader = "default", type = "rounded"}
for i=1,100 do
	Voxel:set_tile(Vector(100+10*i,100,100), mat.id)
	Voxel:update(100)
end

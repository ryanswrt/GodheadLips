Selection = Class()
Selection.class_name = "Selection"
Selection.face_dir = {
	Vector(-1,0,0), Vector(1,0,0),
	Vector(0,-1,0), Vector(0,1,0),
	Vector(0,0,-1), Vector(0,0,1)}
Selection.face_rot = {
	Quaternion{axis = Vector(0,0,1), angle = math.pi/2},
	Quaternion{axis = Vector(0,0,1), angle = -math.pi/2},
	Quaternion{axis = Vector(1,0,0), angle = math.pi},
	Quaternion{axis = Vector(1,0,0), angle = 0},
	Quaternion{axis = Vector(1,0,0), angle = -math.pi/2},
	Quaternion{axis = Vector(1,0,0), angle = math.pi/2}}

Selection.get_tile_key = function(clss, tile, face)
	return math.floor(face + 10 * tile.x + 10000 * tile.y + 10000000 * tile.z + 0.5)
end

Selection.new = function(clss, data, face)
	local self = Class.new(clss)
	if data.class_name == "Vector" then
		-- Tile.
		local p = (data + Vector(0.5,0.5,0.5)) * Voxel.tile_size
		self.key = clss:get_tile_key(data, face)
		self.tile = data
		self.face = face
		self.visual = EditorObject{model = "select1", position = p, rotation = clss.face_rot[face], realized = true}
	else
		-- Object.
		local model = self:create_face_model(face, data.rotation, data.bounding_box_physics)
		self.face = face
		self.key = data
		self.object = data
		self.visual = EditorObject{model = model, position = data.position, realized = true}
	end
	self.visual.selection = self
	return self
end

Selection.create_face_model = function(self, face, rotation, aabb)
	-- Calculate the vertices of the bounding box.
	local pad = 0.2
	local min = rotation * (aabb.point - Vector(pad,pad,pad))
	local max = rotation * (aabb.point + aabb.size + Vector(pad,pad,pad))
	min.x,max.x = math.min(min.x,max.x),math.max(min.x,max.x)
	min.y,max.y = math.min(min.y,max.y),math.max(min.y,max.y)
	min.z,max.z = math.min(min.z,max.z),math.max(min.z,max.z)
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
	local addbox = function(a,b,c,d,n)
	addface(a,{a[1]-0.1,a[2]-0.1,a[3]-0.1},c,{c[1]-0.1,c[2]-0.1,c[3]-0.1},n)
	addface(b,{b[1]-0.1,b[2]-0.1,b[3]-0.1},d,{d[1]-0.1,d[2]-0.1,d[3]-0.1},n)
	addface(a,{a[1]-0.1,a[2]-0.1,a[3]-0.1},b,{b[1]-0.1,b[2]-0.1,b[3]-0.1},n)
	addface(c,{c[1]-0.1,c[2]-0.1,c[3]-0.1},d,{d[1]-0.1,d[2]-0.1,d[3]-0.1},n)
	end
	-- Left and right.
	if face == 1 then addbox(p[1],p[3],p[5],p[7],{-1,0,0})end
	if face == 2 then addbox(p[2],p[4],p[6],p[8],{-1,0,0}) end
	-- Bottom and top.
	if face == 3 then addbox(p[1], p[2], p[5], p[6], {0,-1,0}) end
	if face == 4 then addbox(p[3], p[4], p[7], p[8], {0,1,0}) end
	-- Front and back.
	if face == 5 then addbox(p[1], p[2], p[3], p[4], {0,0,-1}) end
	if face == 6 then addbox(p[5], p[6], p[7], p[8], {0,0,1}) end
	-- Create the model.
	local model = Model()
	model:add_material{cull = false, shader = "default"}
	--model:remove_vertices()
	model:add_triangles{material = 1, vertices = v}
	model:changed()
	return model
end

Selection.detach = function(self)
	self.visual.realized = false
end

Selection.refresh = function(self)
	if not self.object then return end
	self.visual.position = self.object.position
	self.visual.rotation = self.object.rotation
end

Selection.rotate = function(self, drot)
	if not self.object then return end
	self.object.rotation = drot * self.object.rotation
	self:refresh()
end

Selection.transform = function(self, center, dpos, drot)
	if not self.object then return end
	self.object.position = drot * (self.object.position + dpos - center) + center
	self:refresh()
end

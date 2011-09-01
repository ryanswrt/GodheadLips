import re, bpy, mathutils
from bpy_extras.object_utils import object_data_add

# \brief Mirrors the object and its shape keys.
# \param obj Object to mirror.
# \param mod Mirror modifier.
def apply_shape_key_mirror(obj, mod):
	mirror_g = mod.use_mirror_vertex_groups
	mirror_u = mod.use_mirror_u
	mirror_v = mod.use_mirror_v
	mirror_x = mod.use_x
	faces = []
	face_newtoold = {}
	face_oldtonew = {}
	verts = []
	vert_merged = {}
	vert_newtoold = {}
	vert_oldtonew = {}
	# Store original counts.
	num_verts = len(obj.data.vertices)
	num_edges = len(obj.data.edges)
	num_faces = len(obj.data.faces)
	num_groups = len(obj.vertex_groups)
	num_shapes = len(obj.data.shape_keys.key_blocks)
	num_layers = 0
	if obj.data.uv_textures:
		num_layers = len(obj.data.uv_textures)
	# Add original vertices.
	for i in range(0,num_verts):
		verts.append(obj.data.vertices[i].co)
		vert_newtoold[i] = i
		vert_oldtonew[i] = i
	# Add mirrored vertices.
	for i in range(0,num_verts):
		vert = verts[i]
		if vert.x > -0.001 and vert.x < 0.001:
			vert_newtoold[i] = i
			vert_oldtonew[i] = i
			vert_merged[i] = True
		else:
			l = len(verts)
			vert_newtoold[l] = i
			vert_oldtonew[i] = l
			verts.append(mathutils.Vector((-vert.x, vert.y, vert.z)))
	# Add original faces.
	for i in range(0,num_faces):
		face = obj.data.faces[i]
		if len(face.vertices) == 3:
			indices = [face.vertices[0], face.vertices[1], face.vertices[2]]
		else:
			indices = [face.vertices[0], face.vertices[1], face.vertices[2], face.vertices[3]]
		face_newtoold[i] = i
		face_oldtonew[i] = i
		faces.append(indices)
	# Add mirrored faces.
	for i in range(0,num_faces):
		face = faces[i]
		if len(face) == 3:
			indices = [face[2], face[1], face[0]]
		else:
			indices = [face[3], face[2], face[1], face[0]]
		for j in range(0,len(face)):
			indices[j] = vert_oldtonew[indices[j]]
		l = len(faces)
		face_newtoold[l] = i
		face_oldtonew[i] = l
		faces.append(indices)
	# Create the mesh.
	mesh = bpy.data.meshes.new("LosTmp")
	mesh.from_pydata(verts, [], faces)
	mesh.update()
	# Add complete vertex data.
	for i in range(0,len(verts)):
		old_vert = obj.data.vertices[vert_newtoold[i]]
		new_vert = mesh.vertices[i]
		if i in vert_merged:
			n = old_vert.normal.copy()
			n.x = 0
			new_vert.normal = n.normalized()
		else:
			new_vert.normal = old_vert.normal.copy()
	# Create materials.
	for mat in obj.data.materials:
		mesh.materials.append(mat)
	# Create texture layers.
	for i in range(0,num_layers):
		mesh.uv_textures.new(obj.data.uv_textures[i].name)
	# Add complete face data.
	for i in range(0,len(faces)):
		old_face = obj.data.faces[face_newtoold[i]]
		new_face = mesh.faces[i]
		new_face.hide = old_face.hide
		new_face.material_index = old_face.material_index
		new_face.select = old_face.select
		new_face.use_smooth = old_face.use_smooth
		for j in range(0,num_layers):
			old_uv = obj.data.uv_textures[j].data[face_newtoold[i]]
			new_uv = mesh.uv_textures[j].data[i]
			new_uv.blend_type = old_uv.blend_type
			new_uv.hide = old_uv.hide
			new_uv.image = old_uv.image
			new_uv.pin_uv = old_uv.pin_uv
			new_uv.select_uv = old_uv.select_uv
			new_uv.use_alpha_sort = old_uv.use_alpha_sort
			new_uv.use_billboard = old_uv.use_billboard
			new_uv.use_bitmap_text = old_uv.use_bitmap_text
			new_uv.use_blend_shared = old_uv.use_blend_shared
			new_uv.use_collision = old_uv.use_collision
			new_uv.use_halo = old_uv.use_halo
			new_uv.use_image = old_uv.use_image
			new_uv.use_light = old_uv.use_light
			new_uv.use_object_color = old_uv.use_object_color
			new_uv.use_shadow_cast = old_uv.use_shadow_cast
			new_uv.use_twoside = old_uv.use_twoside
			if not mirror_u or face_newtoold[i] == i:
				new_uv.uv = [(uv[0], uv[1]) for uv in old_uv.uv]
			else:
				uv = [(1.0-uv[0], uv[1]) for uv in old_uv.uv]
				if len(uv) == 3:
					new_uv.uv = [uv[2], uv[1], uv[0]]
				else:
					new_uv.uv = [uv[3], uv[2], uv[1], uv[0]]
	# Create the object.
	object_data_add(bpy.context, mesh, operator=None)
	new_obj = bpy.context.scene.objects.active
	# Create shape keys.
	for i in range(0,num_shapes):
		old_shape = obj.data.shape_keys.key_blocks[i]
		new_obj.shape_key_add(name=old_shape.name, from_mix=False)
		new_shape = new_obj.data.shape_keys.key_blocks[i]
		for j in range(0,len(verts)):
			old_vert = old_shape.data[vert_newtoold[j]]
			new_vert = new_shape.data[j]
			new_vert.co = old_vert.co.copy()
			if j != vert_newtoold[j]:
				new_vert.co.x *= -1.0
	# Create vertex groups.
	for i in range(0,num_groups):
		old_group = obj.vertex_groups[i]
		new_group = new_obj.vertex_groups.new(name=old_group.name)
	for i in range(0,len(verts)):
		old_vert = obj.data.vertices[vert_newtoold[i]]
		new_vert = mesh.vertices[i]
		for j in range(0,len(old_vert.groups)):
			old_elem = old_vert.groups[j]
			old_group = obj.vertex_groups[old_elem.group]
			if mirror_g and i != vert_newtoold[i]:
				new_group_name = re.sub(r'\.L$', '.R', old_group.name)
				if new_group_name == old_group.name:
					new_group_name = re.sub(r'\.R$', '.L', old_group.name)
				if new_group_name in new_obj.vertex_groups:
					new_group = new_obj.vertex_groups[new_group_name]
				else:
					new_group = new_obj.vertex_groups[old_group.name]
			else:
				new_group = new_obj.vertex_groups[old_elem.group]
			new_group.add([i], old_elem.weight, 'REPLACE')
	# Copy custom properties.
	def copyprop(prop):
		try:
			v = obj[prop]
			new_obj[prop] = v
		except:
			pass
	copyprop("export")
	copyprop("file")
	copyprop("render")
	copyprop("shape")

obj = bpy.context.active_object
if obj:
	for mod in obj.modifiers:
		if mod.type == "MIRROR" and obj.data.shape_keys:
			bpy.context.user_preferences.edit.use_global_undo = False
			apply_shape_key_mirror(obj, mod)
			bpy.context.user_preferences.edit.use_global_undo = True

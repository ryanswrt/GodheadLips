bl_info = {
	"name": "Export Lips of Suna (.lmdl)",
	"author": "Lips of Suna development team",
	"version": (1, 0),
	"blender": (2, 5, 7),
	"api": 35622,
	"location": "File > Export",
	"description": "Lips of Suna (.lmdl)",
	"warning": "",
	"wiki_url": "http://sourceforge.net/apps/trac/lipsofsuna/wiki",
	"tracker_url": "http://sourceforge.net/apps/trac/lipsofsuna/wiki",
	"category": "Import-Export"}

import array, math, os, re, struct, sys, bpy, mathutils
from bpy_extras.object_utils import object_data_add

class LIEnum:
	def __init__(self):
		self.debug = False
		self.files = []
		self.MAGIC = 'lips/mdl'
		self.VERSION = 0xFFFFFFF3
		self.LIGHTFLAG_NOSHADOW = 0x01
		self.LIGHTFLAG_NEWFORMAT = 0x80
		self.MATRFLAG_BILLBOARD = 1
		self.MATRFLAG_COLLISION = 2
		self.MATRFLAG_CULLFACE = 4
		self.MATRFLAG_ALPHA = 8
		self.MATRFLAG_DEFAULT = self.MATRFLAG_COLLISION | self.MATRFLAG_CULLFACE
		self.NODETYPE_BONE = 0
		self.NODETYPE_EMPTY = 1
		self.NODETYPE_LIGHT = 2
		self.PARTICLEFLAG_NEWFORMAT = 0x80000000
		self.TEXFLAG_BILINEAR = 1
		self.TEXFLAG_CLAMP = 2
		self.TEXFLAG_MIPMAP = 4
		self.TEXFLAG_REPEAT = 8
		self.TEXFLAG_DEFAULT = self.TEXFLAG_BILINEAR | self.TEXFLAG_MIPMAP | self.TEXFLAG_REPEAT
		self.TEXTYPE_NONE = 0
		self.TEXTYPE_ENVMAP = 1
		self.TEXTYPE_IMAGE = 2
		self.euler = mathutils.Euler((-0.5 * math.pi, 0, 0))
		self.matrix = self.euler.to_matrix().to_4x4()
		self.quat = self.euler.to_quaternion()
LIFormat = LIEnum()

##############################################################################

class LIExportFinishedDialog(bpy.types.Operator):
	bl_idname = "object.lipsofsuna_export_finished_operator"
	bl_label = "EXPORTING FINISHED"

	def draw(self, context):
		layout = self.layout
		if len(LIFormat.files):
			col = layout.column()
			flow = col.row()
			col1 = flow.column()
			col1.label(text = "FILE")
			col2 = flow.column()
			col2.scale_x = 0.3
			col2.label(text = "VTX")
			col3 = flow.column()
			col3.scale_x = 0.15
			col3.label(text = "MAT")
			col4 = flow.column()
			col4.scale_x = 0.15
			col4.label(text = "ANI")
			col5 = flow.column()
			col5.scale_x = 0.15
			col5.label(text = "NOD")
			col6 = flow.column()
			col6.scale_x = 0.15
			col6.label(text = "KEY")
			for file in LIFormat.files:
				info = file.get_finish_info()
				col1.label(text = file.filename)
				col2.label(text = "%s" % info[0])
				col3.label(text = "%s" % info[1])
				col4.label(text = "%s" % info[2])
				col5.label(text = "%s" % info[3])
				col6.label(text = "%s" % info[4])
			layout.column()
		else:
			col = layout.column()
			col.label(text = "No objects were exported.")
			row = col.row()
			row.label(text = "Add the custom property 'file' to your objects.")

	def execute(self, context):
		return {'FINISHED'}

	def invoke(self, context, event):
		return context.window_manager.invoke_props_dialog(self)

bpy.utils.register_class(LIExportFinishedDialog)

##############################################################################

def material_files(material):
	def getprop(mat, prop):
		try:
			return mat[prop]
		except:
			return ""
	files = getprop(material, 'file').split(',')
	if getprop(material, 'lod') == 'true':
		return [f + 'l.lmdl' for f in files if len(f)]
	else:
		return [f + '.lmdl' for f in files if len(f)]

def object_files(object):
	def getprop(obj, prop):
		try:
			return obj[prop]
		except:
			return ""
	# No files for explicitly disabled objects.
	if getprop(object, 'export') == 'false':
		return []
	# Don't export links or proxies.
	try:
		if object.proxy or object.library:
			return []
		if object.data.library:
			return []
	except:
		pass
	# Otherwise get the list from the file property.
	files = getprop(object, 'file').split(',')
	if getprop(object, 'lod') == 'true':
		return [f + 'l.lmdl' for f in files if len(f)]
	else:
		return [f + '.lmdl' for f in files if len(f)]

def object_check_export(object, file, type='NODE'):
	def getprop(object, prop):
		try:
			return object[prop]
		except:
			return ""
	# Don't export links or proxies.
	try:
		if object.proxy or object.library:
			return False
		if object.data.library:
			return False
	except:
		pass
	# Make sure that the filename matches.
	if file not in object_files(object):
		return False
	# Make sure that the type is a requested one.
	if type == 'NODE':
		return True
	elif type == 'PARTICLE':
		if object.type != 'MESH':
			return False
		if getprop(object, 'shape') != "":
			return False
		return True
	elif type == 'RENDER':
		if object.type != 'MESH':
			return False
		if getprop(object, 'shape') != "" or getprop(object, 'render') == "false":
			return False
		return True
	elif type == 'SHAPE':
		if object.type != 'MESH':
			return False
		if getprop(object, 'shape') == "":
			return False
		return True
	else:
		return False

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

##############################################################################

class LIAnimation:

	def __init__(self, hierarchy, armat, name, tracks):
		self.hierarchy = hierarchy
		self.armature = armat
		self.name = name
		self.state = None
		self.tracks = tracks
		self.framelist = []
		# Determine the duration of the animation.
		self.frame_start = None
		self.frame_end = None
		for track in tracks:
			for strip in track.strips:
				if self.frame_start == None or self.frame_start > strip.frame_start:
					self.frame_start = int(strip.frame_start)
				if self.frame_end == None or self.frame_end < strip.frame_end:
					self.frame_end = int(strip.frame_end)
		if self.frame_start == None:
			self.frame_start = 0
		if self.frame_end == None:
			self.frame_end = 0
		if self.frame_start < 0:
			self.frame_start = 0
		# Get the list of bones moved by FK.
		self.channeldict = {}
		self.channellist = []
		for track in tracks:
			for strip in track.strips:
				for fcurve in strip.action.fcurves:
					chan = fcurve.data_path
					if chan.find("pose.bones[\"") == 0:
						chan = chan.split("\"")[1]
						self.add_channel(chan)
		# Get the list of bones moved by constraints.
		for bone in armat.pose.bones:
			for cons in bone.constraints:
				if cons.type == 'IK' and cons.target and cons.subtarget in self.channeldict:
					# Part of the IK chain of an animated bone.
					if cons.chain_count > 0:
						next = bone
						for i in range(0, cons.chain_count):
							self.add_channel(next.name)
							next = bone.parent
							if not next:
								break
					else:
						next = bone
						while next:
							self.add_channel(next.name)
							next = bone.parent
				elif cons.type == 'COPY_ROTATION' and cons.target and cons.subtarget in self.channeldict:
					# Copy rotation from an animated bone.
					self.add_channel(bone.name)

	def add_channel(self, name):
		if name in self.armature.data.bones:
			if name not in self.channeldict:
				self.channeldict[name] = True
				self.channellist.append(name)

	def process(self):
		if self.state == None:
			# TODO: Make sure that there are no solo tracks.
			# Make sure that there is no active action.
			self.armature.animation_data.action = None
			# Disable blend in/out to avoid broken frames at start or end.
			for track in self.armature.animation_data.nla_tracks:
				for strip in track.strips:
					strip.use_auto_blend = False
					strip.blend_in = 0.0
					strip.blend_out = 0.0
			# Mute tracks that don't belong to the animation.
			for track in self.armature.animation_data.nla_tracks:
				if track.name != self.name:
					track.mute = True
				else:
					track.mute = False
			# Reset the armature to the rest pose.
			self.hierarchy.rest_pose()
			# Stabilize constraints.
			bpy.context.scene.frame_set(self.frame_start)
			bpy.context.scene.frame_set(self.frame_start)
			self.state = 0
			return False
		else:
			# Switch to the processed frame.
			frame = self.frame_start + self.state
			if frame >= self.frame_end:
				return True
			bpy.context.scene.frame_set(frame)
			bpy.context.scene.frame_set(frame)
			# Get channel transformations.
			xforms = []
			for chan in self.channellist:
				node = self.hierarchy.nodedict[chan]
				xforms.append(node.get_pose_transform())
			self.framelist.append(xforms)
			self.state += 1
			return False

	def write(self, writer):
		# Writer header.
		writer.write_string(self.name)
		writer.write_int(len(self.channellist))
		writer.write_int(self.frame_end - self.frame_start)
		writer.write_marker()
		# Write channel names.
		for chan in self.channellist:
			writer.write_string(chan)
		writer.write_marker()
		# Write channel transformations.
		for frame in range(0, self.frame_end - self.frame_start):
			for chan in range(len(self.channellist)):
				xform = self.framelist[frame][chan]
				writer.write_float(xform[0].x)
				writer.write_float(xform[0].y)
				writer.write_float(xform[0].z)
				writer.write_float(xform[1].x)
				writer.write_float(xform[1].y)
				writer.write_float(xform[1].z)
				writer.write_float(xform[1].w)
				writer.write_marker()
			writer.write_marker()

class LICollision:

	# \brief Initializes a new collision shape manager.
	# \param self Shape manager.
	def __init__(self):
		self.shapes = {}

	# \brief Builds the default collision mesh from the graphical mesh.
	# \param object Mesh object.
	def add_default(self, object):
		if 'default' not in self.shapes:
			# Calculate the bounding box of the object.
			bmin = mathutils.Vector(object.bound_box[0])
			bmax = mathutils.Vector(object.bound_box[0])
			for b in object.bound_box:
				if bmin[0] > b[0]:
					bmin[0] = b[0]
				if bmin[1] > b[1]:
					bmin[1] = b[1]
				if bmin[2] > b[2]:
					bmin[2] = b[2]
				if bmax[0] < b[0]:
					bmax[0] = b[0]
				if bmax[1] < b[1]:
					bmax[1] = b[1]
				if bmax[2] < b[2]:
					bmax[2] = b[2]
			center = 0.5 * (bmax + bmin)
			# Save the old selection.
			selection = []
			for obj in bpy.data.objects:
				if obj.select:
					selection.append(obj)
			# Create an icosphere enclosing the object.
			bpy.ops.mesh.primitive_ico_sphere_add(subdivisions=2, size=0.7*(bmax - bmin).length)
			icohull = None
			for obj in bpy.data.objects:
				if obj.select:
					icohull = obj
					break
			# Add a shrinkwrap modifier.
			modifier = icohull.modifiers.new('shrinkwrap', 'SHRINKWRAP')
			modifier.target = object
			# Apply the shrinkwrap modifier.
			translation = mathutils.Matrix.Translation((center.x, center.y, center.z))
			icohull.matrix_local = object.matrix_local * translation
			oldmesh = icohull.data
			icohull.data = icohull.to_mesh(bpy.context.scene, True, 'PREVIEW')
			bpy.data.meshes.remove(oldmesh)
			# Fix the coordinate system.
			icohull.matrix_local = LIFormat.matrix * icohull.matrix_local
			bpy.ops.object.transform_apply(rotation=True)
			# Collect the resulting vertices.
			shape = LIShape('default')
			shape.add_mesh(icohull, center)
			self.shapes['default'] = shape
			# Delete the icosphere.
			bpy.ops.object.delete()
			# Restore the old selection.
			for obj in selection:
				obj.select = True

	# \brief Adds an explicit collision mesh to the list of collision shapes.
	# \param object Mesh object.
	def add_mesh(self, object):
		try:
			prop = object['shape']
			if prop not in self.shapes:
				self.shapes[prop] = LIShape(prop)
			self.shapes[prop].add_mesh(object, mathutils.Vector((0.0, 0.0, 0.0)))
		except:
			pass

	# \brief Saves all shapes.
	# \param self Shape manager.
	# \param writer Writer.
	def write(self, writer):
		if not len(self.shapes):
			return False
		writer.write_int(len(self.shapes))
		writer.write_marker()
		for name in self.shapes:
			self.shapes[name].write(writer)
		return True

class LIHierarchy:

	def __init__(self, file):
		self.file = file
		self.state = 0
		# Reset armatures to rest pose.
		self.rest_pose()
		# Build the node hierarchy.
		self.node = LINodeRoot(self)
		self.nodedict = {}
		self.nodelist = []
		self.add_node(self.node)
		self.node.add_children()
		if len(self.nodelist) == 1:
			self.nodedict = {}
			self.nodelist = []
		# Check for armatures.
		self.armature = None
		for node in self.nodelist:
			if node.object and node.object.type == 'ARMATURE':
				self.armature = node.object
				break
		# Build animations.
		self.trackdict = {}
		self.animlist = []
		if self.armature:
			# Get the list of track names. We group tracks with matching names
			# together so that they can be compiled into a single animation.
			for track in self.armature.animation_data.nla_tracks:
				if track.name not in self.trackdict:
					self.trackdict[track.name] = [track]
				else:
					self.trackdict[track.name].append(track)
			# Build animations from tracks. We create a new animation for each
			# unique track name present in the NLA editor.
			for name in self.trackdict:
				self.animlist.append(LIAnimation(self, self.armature, name, self.trackdict[name]))

	def add_node(self, node):
		self.nodedict[node.name] = node
		self.nodelist.append(node)

	def get_progress_info(self):
		if self.state >= len(self.animlist):
			return None
		anim = self.animlist[self.state]
		return anim.name

	def process(self):
		if self.state >= len(self.animlist):
			return True
		anim = self.animlist[self.state]
		if anim.process():
			self.state += 1
		return False

	# \brief Resets all armatures in the scene to their rest positions.
	def rest_pose(self):
		active = bpy.context.scene.objects.active
		for obj in bpy.data.objects:
			if obj.type == 'ARMATURE' and not obj.library:
				bpy.context.scene.objects.active = obj
				bpy.ops.object.mode_set(mode='POSE', toggle=False)
				bpy.ops.pose.select_all(action='SELECT')
				bpy.ops.pose.loc_clear()
				bpy.ops.pose.rot_clear()
				bpy.ops.pose.scale_clear()
				bpy.ops.object.mode_set(mode='OBJECT', toggle=False)
		bpy.context.scene.objects.active = active

	def write(self, writer):
		if not len(self.node.nodes):
			return False
		writer.write_int(1)
		self.node.write(writer)
		return True

	def write_anims(self, writer):
		if not len(self.animlist):
			return False
		writer.write_int(len(self.animlist))
		for anim in self.animlist:
			anim.write(writer)
		return True

class LIMaterial:

	def __init__(self, index, mat, img):
		self.index = index
		self.indices = []
		self.diffuse = [1, 1, 1, 1]
		self.emission = 0
		self.flags = LIFormat.MATRFLAG_DEFAULT
		self.name = 'default'
		self.shader = 'default'
		self.shininess = 0
		self.specular = [1, 1, 1, 1]
		self.strands = [0.2, 0.0, 0.0]
		self.textures = []
		# Material properties.
		if mat:
			self.name = mat.name
			try:
				self.shader = mat["shader"]
			except:
				pass
			try:
				if mat["cull"] == "false":
					self.flags &= ~LIFormat.MATRFLAG_CULLFACE
			except:
				pass
			try:
				if mat["collision"] == "false":
					self.flags &= ~LIFormat.MATRFLAG_COLLISION
			except:
				pass
			if mat.use_transparency:
				self.flags |= LIFormat.MATRFLAG_ALPHA
			self.diffuse[0] = mat.diffuse_color[0]
			self.diffuse[1] = mat.diffuse_color[1]
			self.diffuse[2] = mat.diffuse_color[2]
			self.diffuse[3] = mat.alpha
			self.emission = mat.emit
			self.shininess = mat.specular_hardness / 511.0 * 128.0
			self.specular[0] = mat.specular_color[0]
			self.specular[1] = mat.specular_color[1]
			self.specular[2] = mat.specular_color[2]
			self.specular[3] = mat.specular_intensity
			self.strands[0] = mat.strand.root_size
			self.strands[1] = mat.strand.tip_size
			self.strands[2] = mat.strand.shape
		# Material textures.
		if mat:
			index = 0
			for slot in mat.texture_slots:
				if slot and slot.texture and slot.texture.type == 'IMAGE':
					tex = slot.texture
					while len(self.textures) < index:
						self.textures.append(LITexture(len(self.textures), None, None))
					self.textures.append(LITexture(index, tex, tex.image))
				index = index + 1
		# Face texture.
		if img and ((not mat) or (mat and mat.use_face_texture)):
			if not len(self.textures):
				self.textures.append(0)
			self.textures[0] = LITexture(0, None, img)

	def write_indices(self, writer):
		writer.write_int(len(self.indices))
		for i in self.indices:
			writer.write_int(i)
		writer.write_marker()

	def write_info(self, writer):
		writer.write_int(self.flags)
		writer.write_float(self.emission)
		writer.write_float(self.shininess)
		writer.write_float(self.diffuse[0])
		writer.write_float(self.diffuse[1])
		writer.write_float(self.diffuse[2])
		writer.write_float(self.diffuse[3])
		writer.write_float(self.specular[0])
		writer.write_float(self.specular[1])
		writer.write_float(self.specular[2])
		writer.write_float(self.specular[3])
		writer.write_float(self.strands[0])
		writer.write_float(self.strands[1])
		writer.write_float(self.strands[2])
		writer.write_int(len(self.textures))
		writer.write_string(self.shader)
		writer.write_marker()
		for tex in self.textures:
			tex.write(writer)

class LIMesh:

	def __init__(self, obj, file):
		mesh = obj.data
		self.file = file
		# Initialize storage.
		self.matdict = {}
		self.matlist = []
		self.shapekeydict = {}
		self.shapekeylist = []
		self.vertdict = {} # bl_vertex -> los_index
		self.vertlist = [] # los_index -> los_vertex
		self.vertmapping = [] # los_index -> bl_index
		self.weightgroupdict = {}
		self.weightgrouplist = []
		# Emit faces.
		self.emit_faces(obj, mesh)
		# Emit shape keys.
		if obj.data.shape_keys:
			for bl_key in obj.data.shape_keys.key_blocks:
				key = LIShapeKey(self, obj, bl_key)
				if not key.empty:
					self.shapekeydict[bl_key.name] = key
					self.shapekeylist.append(key)

	def emit_face(self, obj, mesh, face):
		# Vertices.
		verts = [mesh.vertices[x] for x in face.vertices]
		indices = [x for x in face.vertices]
		# Material attributes.
		# Materials may have a file property that limits exporting of the face
		# to a certain file. We need to check that the file is correct and skip
		# the face if it isn't.
		idx = face.material_index
		bmat = None
		bimg = None
		if idx < len(obj.material_slots):
			bmat = obj.material_slots[idx].material
			files = material_files(bmat)
			if len(files) and self.file not in files:
				return
		# Texture attributes.
		if mesh.uv_textures.active:
			buvs = mesh.uv_textures.active.data[face.index]
			bimg = buvs.image
			uvs = buvs.uv
		else:
			uvs = ((0, 0), (0, 0), (0, 0), (0, 0))
		# Emit material.
		key = (bmat and idx or -1, bimg and bimg.name or '')
		if key not in self.matdict:
			mat = LIMaterial(len(self.matlist), bmat, bimg)
			self.matdict[key] = mat
			self.matlist.append(mat)
		else:
			mat = self.matdict[key]
		# Emit triangles.
		for i in range(0, len(verts)):
			# Vertex attributes.
			bvert = verts[i]
			no = face.use_smooth and bvert.normal or face.normal
			uv = uvs[i]
			# Emit vertex.
			key = (indices[i], no.x, no.y, no.z, uv[0], uv[1])
			if key not in self.vertdict:
				# Add weights.
				weights = []
				for weight in bvert.groups:
					group = obj.vertex_groups[weight.group]
					if group.name not in self.weightgroupdict:
						grpidx = len(self.weightgrouplist)
						self.weightgroupdict[group.name] = grpidx
						self.weightgrouplist.append(group.name)
					else:
						grpidx = self.weightgroupdict[group.name]
					weights.append((grpidx, weight.weight))
				# Add vertex.
				vert = LIVertex(len(self.vertlist), bvert.co, no, uv, weights)
				self.vertmapping.append(indices[i])
				self.vertdict[key] = len(self.vertlist)
				self.vertlist.append(vert)
			else:
				index = self.vertdict[key]
				vert = self.vertlist[index]
			# Emit index.
			mat.indices.append(vert.index)

	def emit_faces(self, obj, mesh):
		for face in mesh.faces:
			self.emit_face(obj, mesh, face)

	def write_bounds(self, writer):
		if not len(self.vertlist):
			return False
		min = self.vertlist[0].co.copy()
		max = self.vertlist[0].co.copy()
		for vert in self.vertlist:
			if min.x > vert.co.x: min.x = vert.co.x
			if min.y > vert.co.y: min.y = vert.co.y
			if min.z > vert.co.z: min.z = vert.co.z
			if max.x < vert.co.x: max.x = vert.co.x
			if max.y < vert.co.y: max.y = vert.co.y
			if max.z < vert.co.z: max.z = vert.co.z
		writer.write_float(min.x)
		writer.write_float(min.y)
		writer.write_float(min.z)
		writer.write_float(max.x)
		writer.write_float(max.y)
		writer.write_float(max.z)
		writer.write_marker()
		return True

	def write_groups(self, writer):
		if not len(self.matlist):
			return False
		writer.write_int(len(self.matlist))
		writer.write_marker()
		index = 0
		for mat in self.matlist:
			writer.write_int(index)
			mat.write_indices(writer)
			index += 1
		return True

	def write_materials(self, writer):
		if not len(self.matlist):
			return False
		writer.write_int(len(self.matlist))
		writer.write_marker()
		for mat in self.matlist:
			mat.write_info(writer)
		return True

	def write_shape_keys(self, writer):
		if not len(self.shapekeylist):
			return False
		writer.write_int(len(self.shapekeylist))
		writer.write_marker()
		for key in self.shapekeylist:
			key.write(writer)
		return True

	def write_vertices(self, writer):
		if not len(self.vertlist):
			return False
		writer.write_int(len(self.vertlist))
		writer.write_marker()
		for vert in self.vertlist:
			vert.write(writer)
		return True

	def write_weights(self, writer):
		if not len(self.weightgrouplist):
			return False
		writer.write_int(len(self.weightgrouplist))
		for group in self.weightgrouplist:
			writer.write_string(group)
			writer.write_string(group)
		writer.write_marker()
		writer.write_int(len(self.vertlist))
		writer.write_marker()
		for vertex in self.vertlist:
			vertex.write_weights(writer)
		return True

class LINode:

	# \brief Creates a new generic object node.
	# \param object Blender object.
	# \param parent Parent node.
	def __init__(self, parent, object):
		self.type = LIFormat.NODETYPE_EMPTY
		self.nodes = []
		self.object = object
		self.parent = parent
		self.hierarchy = self.parent.hierarchy
		self.loc,self.rot = self.get_rest_transform()
		# Remove the number part so that it's possible to have multiple
		# objects that have anchors with the same names.
		self.name = '#' + re.sub("\\.[0-9]+", "", object.name)

	def add_object(self, object):
		if object_check_export(object, self.hierarchy.file, 'NODE'):
			if object.type == 'ARMATURE':
				self.add_node(LINodeArmature(self, object))
			if object.type == 'EMPTY':
				self.add_node(LINode(self, object))
			if object.type == 'LAMP' and (object.data.type == 'SPOT' or object.data.type == 'POINT'):
				self.add_node(LINodeLight(self, object))

	def add_node(self, node):
		self.nodes.append(node)
		self.hierarchy.add_node(node)

	# \brief Recursively adds children to the node.
	def add_children(self):
		# Add child objects.
		for obj in bpy.data.objects:
			if self.object == obj.parent and not obj.parent_bone:
				self.add_object(obj)
		# Recurse to children.
		for node in self.nodes:
			node.add_children()

	# \brief Gets the parent relative rest transformation of the node.
	def get_rest_transform(self):
		matrix = self.object.matrix_local.copy()
		if self.object.parent_bone:
			parentbone = self.object.parent.pose.bones[self.object.parent_bone]
			parentmatr = parentbone.matrix * mathutils.Matrix.Translation(self.parent.len)
			matrix = parentmatr.inverted() * matrix
		elif self.object.parent:
			parentmatr = self.object.parent.matrix_local
			matrix = parentmatr.inverted() * matrix
		loc = matrix.to_translation()
		rot = matrix.to_3x3().to_quaternion().normalized()
		return (loc, rot)

	# \brief Writes the node to a stream.
	# \param self Node.
	# \param writer Writer.
	def write(self, writer):
		writer.write_int(self.type)
		writer.write_int(len(self.nodes))
		writer.write_string(self.name)
		writer.write_float(self.loc.x)
		writer.write_float(self.loc.y)
		writer.write_float(self.loc.z)
		writer.write_float(self.rot.x)
		writer.write_float(self.rot.y)
		writer.write_float(self.rot.z)
		writer.write_float(self.rot.w)
		self.write_special(writer)
		writer.write_marker()
		for node in self.nodes:
			node.write(writer)

	def write_special(self, writer):
		pass

class LINodeArmature(LINode):

	def __init__(self, parent, object):
		LINode.__init__(self, parent, object)
		self.name = object.name

	def add_children(self):
		# Add root bones.
		for bone in self.object.data.bones.values():
			if not bone.parent:
				self.add_node(LINodeBone(self, bone))
		# Add child objects.
		LINode.add_children(self)

class LINodeBone(LINode):

	def __init__(self, parent, bone):
		self.type = LIFormat.NODETYPE_BONE
		self.nodes = []
		self.object = parent.object
		self.parent = parent
		self.hierarchy = self.parent.hierarchy
		self.bone = bone
		self.pose_bone = self.object.pose.bones[bone.name]
		self.name = bone.name
		self.len = mathutils.Vector((0, bone.length, 0))
		self.head = bone.head.copy()
		self.matrix = bone.matrix.copy()
		self.loc,self.rot = self.get_rest_transform()

	def add_children(self):
		# Add child bones.
		for bone in self.object.data.bones.values():
			if bone.parent == self.bone:
				self.add_node(LINodeBone(self, bone))
		# Add child objects.
		for obj in bpy.data.objects:
			if self.object == obj.parent and self.bone.name == obj.parent_bone:
				self.add_object(obj)
		# Recurse to children.
		for node in self.nodes:
			node.add_children()

	# \brief Gets the parent relative rest transformation of the node.
	def get_rest_transform(self):
		matrix = self.get_head_rest_matrix()
		if self.bone.parent:
			matrix = self.parent.get_tail_rest_matrix().inverted() * matrix
		loc = matrix.to_translation()
		rot = matrix.to_3x3().to_quaternion().normalized()
		return (loc, rot)

	# \brief Gets the parent relative pose transformation of the node.
	def get_pose_transform(self):
		rest = self.get_head_rest_matrix()
		pose = self.get_head_pose_matrix()
		if self.bone.parent:
			rest = self.parent.get_tail_rest_matrix().inverted() * rest
			pose = self.parent.get_tail_pose_matrix().inverted() * pose
		matrix = rest.inverted() * pose
		loc = matrix.to_translation()
		rot = matrix.to_3x3().to_quaternion().normalized()
		return (loc, rot)

	# \brief Gets the armature space rest matrix of the bone head.
	def get_head_rest_matrix(self):
		return self.bone.matrix_local.copy()

	# \brief Gets the armature space rest matrix of the bone tail.
	def get_tail_rest_matrix(self):
		return self.get_head_rest_matrix() * mathutils.Matrix.Translation(self.len)

	# \brief Gets the armature space pose transformation of the bone head.
	def get_head_pose_matrix(self):
		return self.pose_bone.matrix.copy()

	# \brief Gets the armature space pose transformation of the bone tail.
	def get_tail_pose_matrix(self):
		return self.get_head_pose_matrix() * mathutils.Matrix.Translation(self.len)

	def write_special(self, writer):
		writer.write_float(self.len.x)
		writer.write_float(self.len.y)
		writer.write_float(self.len.z)

class LINodeLight(LINode):

	def __init__(self, parent, object):
		LINode.__init__(self, parent, object)
		self.type = LIFormat.NODETYPE_LIGHT
		self.flags = LIFormat.LIGHTFLAG_NEWFORMAT
		if object.data.shadow_method != 'NOSHADOW':
			self.flags |= LIFormat.LIGHTFLAG_NOSHADOW
		if object.data.type == 'SPOT':
			self.clip_start = object.data.shadow_buffer_clip_start
			self.clip_end = object.data.shadow_buffer_clip_end
			self.spot_cutoff = object.data.spot_size
			self.spot_size = 0.5 * object.data.spot_size
			self.spot_blend = 32.0 * object.data.spot_blend
		else:
			self.clip_start = 0.1
			self.clip_end = 100
			self.spot_cutoff = math.pi
			self.spot_size = math.pi
			self.spot_blend = 0
		self.ambient = [0, 0, 0, 1]
		self.ambient[0] = bpy.context.scene.world.ambient_color[0]
		self.ambient[1] = bpy.context.scene.world.ambient_color[1]
		self.ambient[2] = bpy.context.scene.world.ambient_color[2]
		self.diffuse = [0, 0, 0, 1]
		self.diffuse[0] = object.data.energy * object.data.color[0]
		self.diffuse[1] = object.data.energy * object.data.color[1]
		self.diffuse[2] = object.data.energy * object.data.color[2]
		self.specular = [0, 0, 0, 1]
		self.specular[0] = self.diffuse[0]
		self.specular[1] = self.diffuse[1]
		self.specular[2] = self.diffuse[2]
		self.equation = [1, 1, 1]
		self.equation[1] = object.data.linear_attenuation / object.data.distance
		self.equation[2] = object.data.quadratic_attenuation / (object.data.distance * object.data.distance)

	def write_special(self, writer):
		writer.write_int(self.flags)
		writer.write_float(self.spot_cutoff)
		writer.write_float(self.clip_start)
		writer.write_float(self.clip_end)
		writer.write_float(self.ambient[0])
		writer.write_float(self.ambient[1])
		writer.write_float(self.ambient[2])
		writer.write_float(self.ambient[3])
		writer.write_float(self.diffuse[0])
		writer.write_float(self.diffuse[1])
		writer.write_float(self.diffuse[2])
		writer.write_float(self.diffuse[3])
		writer.write_float(self.specular[0])
		writer.write_float(self.specular[1])
		writer.write_float(self.specular[2])
		writer.write_float(self.specular[3])
		writer.write_float(self.equation[0])
		writer.write_float(self.equation[1])
		writer.write_float(self.equation[2])
		writer.write_float(self.spot_size)
		writer.write_float(self.spot_blend)

class LINodeRoot(LINode):

	def __init__(self, hierarchy):
		self.type = LIFormat.NODETYPE_EMPTY
		self.nodes = []
		self.object = None
		self.parent = None
		self.hierarchy = hierarchy
		self.name = '#'
		self.rot = LIFormat.quat
		self.loc = mathutils.Vector((0, 0, 0))

	# \brief Recursively adds children to the node.
	def add_children(self):
		def add_object(object):
			if object_check_export(object, self.hierarchy.file, 'NODE'):
				if object.type == 'ARMATURE':
					self.add_node(LINodeArmature(self, object))
				if object.type == 'EMPTY':
					self.add_node(LINode(self, object))
				if object.type == 'LAMP' and (object.data.type == 'SPOT' or object.data.type == 'POINT'):
					self.add_node(LINodeLight(self, object))
		# Add child objects.
		for object in bpy.data.objects:
			if object.parent == None:
				# Add unparented objects.
				add_object(object)
			elif object.parent.type == "MESH":
				# Add objects parented to meshes that belong to the same file.
				if object_check_export(object.parent, self.hierarchy.file, 'NODE'):
					add_object(object)
		# Recurse to children.
		for node in self.nodes:
			node.add_children()

class LIParticle:

	def __init__(self):
		self.frame_start = 0
		self.frame_end = 0
		self.frames = []

	def add_frame(self, frame):
		self.frames.append(frame)
		self.frame_end += 1

	def is_empty(self):
		return len(self.frames) <= 1

	# \brief Saves the particle.
	# \param self Particle.
	# \param writer Writer.
	def write(self, writer):
		# FIXME: The first frame is broken for some reason.
		if len(self.frames):
			del self.frames[0]
		writer.write_int(self.frame_start)
		writer.write_int(len(self.frames))
		for f in self.frames:
			writer.write_float(f.x)
			writer.write_float(f.y)
			writer.write_float(f.z)
		writer.write_marker()

class LIParticles:

	def __init__(self, file):
		# Create particle systems.
		self.file = file
		self.particlesystems = []
		obj = bpy.context.scene.objects.active
		for object in bpy.context.scene.objects:
			if object_check_export(object, self.file, 'PARTICLE'):
				for sys in object.particle_systems:
					self.particlesystems.append(LIParticleSystem(object, sys))
		# Calculate total animation length.
		self.frame_start = 0
		self.frame_end = 0
		self.frame_end_emit = 0
		for sys in self.particlesystems:
			if self.frame_end_emit < sys.frame_end:
				self.frame_end_emit = sys.frame_end
			if self.frame_end < sys.frame_end + sys.lifetime:
				self.frame_end = sys.frame_end + sys.lifetime
		# Animate particle systems.
		if len(self.particlesystems):
			for frame in range(self.frame_start, self.frame_end):
				# Switch frame.
				bpy.context.scene.frame_set(frame)
				# Add particle frame.
				for sys in self.particlesystems:
					sys.add_frame()

	# \brief Saves all particle systems.
	# \param self Particle manager.
	# \param writer Writer.
	def write(self, writer):
		if not len(self.particlesystems):
			return False
		writer.write_int(len(self.particlesystems))
		writer.write_marker()
		for sys in self.particlesystems:
			sys.write(writer)
		return True

class LIParticleSystem:

	def __init__(self, object, system):
		self.object = object
		self.system = system
		self.particles = []
		self.frame_start = 0
		self.frame_end = int(self.system.settings.frame_end)
		self.lifetime = int(self.system.settings.lifetime)
		self.particle_size = self.system.settings.particle_size
		# Set material properties.
		self.shader = "particle"
		self.texture = "particle1"
		mat = self.system.settings.material - 1
		if mat < len(self.object.material_slots):
			# Set material shader.
			mat = self.object.material_slots[mat]
			try:
				self.shader = mat.material["shader"]
			except:
				pass
			# Set material texture.
			tex = mat.material.texture_slots[0]
			if tex and tex.texture and tex.texture.type == "IMAGE":
				img = tex.texture.image
				self.texture = os.path.splitext(os.path.basename(img.filepath))[0]
		# Create particles.
		for sys in self.system.particles:
			self.particles.append(LIParticle())

	def add_frame(self):
		i = 0
		for bpar in self.system.particles:
			lpar = self.particles[i]
			if bpar.alive_state == 'UNBORN':
				lpar.frame_start += 1
			elif bpar.alive_state == 'ALIVE':
				lpar.add_frame((bpar.location - self.object.location) * LIFormat.matrix)
			i += 1

	# \brief Saves the particle system.
	# \param self Particle system.
	# \param writer Writer.
	def write(self, writer):
		for i in range(len(self.particles) - 1, -1, -1):
			if self.particles[i].is_empty():
				del self.particles[i]
		writer.write_int(LIFormat.PARTICLEFLAG_NEWFORMAT)
		writer.write_int(self.frame_start)
		writer.write_int(self.frame_end)
		writer.write_int(self.frame_end + self.lifetime)
		writer.write_float(self.particle_size)
		writer.write_string(self.shader)
		writer.write_string(self.texture)
		writer.write_int(len(self.particles))
		writer.write_marker()
		for par in self.particles:
			par.write(writer)

class LIShape:

	def __init__(self, name):
		self.name = name
		self.parts = []

	def add_mesh(self, obj, offset):
		self.parts.append(LIShapePart(obj, offset))

	def write(self, writer):
		writer.write_string(self.name)
		writer.write_int(len(self.parts))
		writer.write_marker()
		for p in self.parts:
			p.write(writer)

class LIShapePart:

	def __init__(self, obj, offset):
		self.vertices = []
		matrix = LIFormat.matrix.copy().to_3x3() * obj.matrix_world.to_3x3()
		for v in obj.data.vertices:
			self.vertices.append(v.co * matrix + offset)

	def write(self, writer):
		writer.write_int(len(self.vertices))
		writer.write_marker()
		for v in self.vertices:
			writer.write_float(v.x)
			writer.write_float(v.y)
			writer.write_float(v.z)
			writer.write_marker()

class LIShapeKey:

	def __init__(self, mesh, obj, bl_key):
		self.name = bl_key.name
		self.vertices = []
		self.empty = True
		# Calculate shape key vertex normals.
		dict_index = {}
		for face in obj.data.faces:
			coords = [bl_key.data[face.vertices[i]].co for i in range(len(face.vertices))]
			if len(face.vertices) == 3:
				nml = mathutils.geometry.normal(coords[0], coords[1], coords[2])
			else:
				nml = mathutils.geometry.normal(coords[0], coords[1], coords[2], coord[3])
			for i in face.vertices:
				if i not in dict_index:
					dict_index[i] = [nml]
				else:
					dict_index[i].append(nml)
		dict_normal = {}
		for i in dict_index:
			arr = dict_index[i]
			nml = mathutils.Vector((0, 0, 0))
			for n in arr:
				nml += n
			nml.normalize()
			dict_normal[i] = nml
		# Create shape key vertices.
		for li_idx in range(len(mesh.vertmapping)):
			bl_idx = mesh.vertmapping[li_idx]
			key = bl_key.data[bl_idx]
			vtx = obj.data.vertices[bl_idx]
			if bl_idx in dict_normal:
				nml = dict_normal[bl_idx]
			else:
				nml = vtx.normal
			self.vertices.append((key.co, nml))
			if key.co != vtx.co:
				self.empty = False

	def write(self, writer):
		writer.write_string(self.name)
		writer.write_int(len(self.vertices))
		writer.write_marker()
		for v in self.vertices:
			writer.write_float(v[0].x)
			writer.write_float(v[0].y)
			writer.write_float(v[0].z)
			writer.write_float(v[1].x)
			writer.write_float(v[1].y)
			writer.write_float(v[1].z)
			writer.write_marker()

class LITexture:

	def __init__(self, index, tex, img):
		# Defaults.
		self.index = index
		self.string = ''
		self.flags = LIFormat.TEXFLAG_DEFAULT
		self.type = LIFormat.TEXTYPE_NONE
		# Parameters.
		if tex:
			self.flags = 0
			if tex.extension == 'REPEAT':
				self.flags |= LIFormat.TEXFLAG_CLAMP
			else:
				self.flags |= LIFormat.TEXFLAG_REPEAT
			if tex.use_interpolation:
				self.flags |= LIFormat.TEXFLAG_BILINEAR
			if tex.use_mipmap:
				self.flags |= LIFormat.TEXFLAG_MIPMAP
		# Filename.
		if img:
			self.type = LIFormat.TEXTYPE_IMAGE
			self.string = os.path.splitext(os.path.basename(img.filepath))[0]

	def write(self, writer):
		writer.write_int(self.type)
		writer.write_int(self.flags)
		writer.write_int(0)#512)
		writer.write_int(0)#512)
		writer.write_string(self.string)
		writer.write_marker()

class LIVertex:

	def __init__(self, index, co, no, uv, we):
		self.index = index
		self.co = co
		self.no = no
		self.uv = uv
		self.we = we

	def write(self, writer):
		writer.write_float(self.uv[0])
		writer.write_float(1.0 - self.uv[1])
		writer.write_float(self.no.x)
		writer.write_float(self.no.y)
		writer.write_float(self.no.z)
		writer.write_float(self.co.x)
		writer.write_float(self.co.y)
		writer.write_float(self.co.z)
		writer.write_marker()

	def write_weights(self, writer):
		writer.write_int(len(self.we))
		for weight in self.we:
			writer.write_int(weight[0])
			writer.write_float(weight[1])
		writer.write_marker()

##############################################################################

class LIFile:

	def __init__(self, file):
		self.junkobjs = []
		self.tempobjs = []
		self.origobjs = {}
		self.coll = LICollision()
		self.hier = None
		self.mesh = None
		self.object = None
		self.state = 0
		# Determine the file name.
		# Try to save to ../graphics/ if it exists and fallback to ./ otherwise.
		self.filename = file
		path1 = os.path.split(bpy.data.filepath)[0]
		path2 = os.path.join(os.path.split(path1)[0], "graphics")
		if os.path.exists(path2):
			self.filepath = os.path.join(path2, file)
		else:
			self.filepath = os.path.join(path1, file)

	def get_finish_info(self):
		info = [0, 0, 0, 0, 0]
		if self.mesh:
			info[0] = len(self.mesh.vertlist)
			info[1] = len(self.mesh.matlist)
		if self.hier:
			info[2] = len(self.hier.animlist)
			info[3] = len(self.hier.nodelist)
		if self.mesh:
			info[4] = len(self.mesh.shapekeylist)
		return info

	def get_progress_info(self):
		if self.hier:
			info = self.hier.get_progress_info()
			if info:
				return "Exporting %s (%s)" % (self.filename, info)
		return "Exporting %s" % self.filename

	# \brief Iteratively collects the data belonging to the file.
	# \return True when done, False if more work remains.
	def process(self):
		# Find objects.
		if self.state == 0:
			for obj in bpy.data.objects:
				self.origobjs[obj] = True
			self.state += 1
			return False
		# Create collision shapes.
		elif self.state == 1:
			for obj in bpy.data.objects:
				if object_check_export(obj, self.filename, 'SHAPE'):
					self.coll.add_mesh(obj)
			self.state += 1
			return False
		# Select meshes.
		elif self.state == 2:
			for obj in bpy.data.objects:
				sel = False
				if object_check_export(obj, self.filename, 'RENDER'):
					sel = True
				obj.select = sel
			self.state += 1
			return False
		# Duplicate the meshes and select them.
		elif self.state == 3:
			bpy.ops.object.duplicate()
			for obj in bpy.data.objects:
				if obj.select:
					if obj in self.origobjs:
						# Unselect originals.
						obj.select = False
					else:
						# Select duplicates.
						obj.select = True
						self.tempobjs.append(obj)
			self.state += 1
			return False
		# Apply modifiers to the duplicated meshes.
		# This is currently setup so that multires and armature are the only
		# modifiers that can coexist with shape keys. If any other modifiers
		# exist, the shape key information is lost.
		elif self.state == 4:
			for obj in self.tempobjs:
				# Remove unwanted modifiers.
				num = 0
				for mod in obj.modifiers:
					if mod.type == 'ARMATURE' or mod.type == 'MULTIRES':
						obj.modifiers.remove(mod)
					elif mod.type == "MIRROR" and obj.data.shape_keys:
						apply_shape_key_mirror(obj, mod)
						self.junkobjs.append(obj)
						obj.select = False
						newobj = bpy.context.scene.objects.active
						newobj.select = True
						self.tempobjs[self.tempobjs.index(obj)] = newobj
					else:
						num += 1
				# Apply modifiers.
				if num:
					oldmesh = obj.data
					obj.data = obj.to_mesh(bpy.context.scene, True, 'PREVIEW')
					bpy.data.meshes.remove(oldmesh)
			self.state += 1
			return False
		# Simplify the duplicated meshes.
		elif self.state == 5:
			for obj in self.tempobjs:
				# Apply transformation.
				bpy.context.scene.objects.active = obj
				bpy.ops.object.location_clear()
				bpy.ops.object.transform_apply(scale=True)
				obj.matrix_local = LIFormat.matrix * obj.matrix_local
				bpy.ops.object.transform_apply(rotation=True)
				# Triangulate.
				if obj.mode != 'EDIT':
					bpy.ops.object.editmode_toggle()
				bpy.ops.mesh.select_all(action='SELECT')
				bpy.ops.mesh.quads_convert_to_tris()
				bpy.ops.object.editmode_toggle()
				# Tidy vertex weights.
				bpy.ops.object.vertex_group_clean(all_groups=True)
			self.state += 1
			return False
		# Join the duplicated meshes.
		elif self.state == 6:
			if len(self.tempobjs) > 1:
				bpy.ops.object.join()
			if len(self.tempobjs):
				self.object = bpy.context.scene.objects.active
			self.state += 1
			return False
		# Build the mesh.
		elif self.state == 7:
			if self.object != None:
				try:
					self.mesh = LIMesh(self.object, self.filename)
				except Exception as e:
					bpy.ops.object.delete()
					raise e
			self.state += 1
			return False
		# Build the default collision shape.
		elif self.state == 8:
			# Remove the armature and build a convex hull using shrinkwrap.
			if self.mesh:
				for mod in self.object.modifiers:
					self.object.modifiers.remove(mod)
				self.coll.add_default(self.object)
			self.state += 1
			return False
		# Delete the temporary mesh.
		elif self.state == 9:
			need = False
			if self.object != None:
				bpy.ops.object.delete()
				need = True
			for obj in self.junkobjs:
				obj.select = True
				need = True
			if need:
				bpy.ops.object.delete()
			self.state += 1
			return False
		# Build the node hierarchy.
		elif self.state == 10:
			self.hier = LIHierarchy(self.filename)
			self.state += 1
			return False
		# Bake skeletal animations.
		elif self.state == 11:
			if self.hier == None or self.hier.process():
				self.state += 1
			return False
		# Build particle animations.
		elif self.state == 12:
			self.particles = LIParticles(self.filename)
			self.state += 1
			return False
		else:
			return True

	def write(self):
		if not self.mesh and not self.hier:
			return
		if LIFormat.debug:
			debug = open(self.filepath + ".dbg.txt", "w")
		else:
			debug = None
		self.file = open(self.filepath, "wb")
		# Header.
		data = LIWriter(debug)
		data.clear(LIFormat.MAGIC)
		data.write_int(LIFormat.VERSION)
		data.write_int(0)
		data.write_marker()
		self.write_block(LIFormat.MAGIC, data)
		if self.mesh:
			# Bounds.
			data.clear("bou")
			if self.mesh.write_bounds(data):
				self.write_block("bou", data)
			# Materials.
			data.clear("mat")
			if self.mesh.write_materials(data):
				self.write_block("mat", data)
			# Vertices.
			data.clear("ver")
			if self.mesh.write_vertices(data):
				self.write_block("ver", data)
			# Indices.
			data.clear("fac")
			if self.mesh.write_groups(data):
				self.write_block("fac", data)
			# Weights.
			data.clear("wei")
			if self.mesh.write_weights(data):
				self.write_block("wei", data)
			# Shape keys.
			data.clear("shk")
			if self.mesh.write_shape_keys(data):
				self.write_block("shk", data)
			# TODO: Hairs.
		if self.hier:
			# Nodes.
			data.clear("nod")
			if self.hier.write(data):
				self.write_block("nod", data)
			# Animations.
			data.clear("ani")
			if self.hier.write_anims(data):
				self.write_block("ani", data)
		# Shapes.
		data.clear("sha")
		if self.coll.write(data):
			self.write_block("sha", data)
		# Particles.
		data.clear("par")
		if self.particles.write(data):
			self.write_block("par", data)
		# Done.
		self.file.close()
		if debug:
			debug.close()

	def write_block(self, name, data):
		self.file.write(name.encode())
		self.file.write(struct.pack('B', 0))
		self.file.write(struct.pack("!I", data.pos))
		self.file.write(data.data)

class LIWriter:

	def __init__(self, debug):
		self.data = bytearray()
		self.pos = 0
		self.debug = debug

	def clear(self, block):
		if self.debug:
			self.debug.write("\n=== %s ===\n" % block)
		self.data = bytearray()
		self.pos = 0

	# Appends an integer.
	def write_int(self, value):
		if self.debug:
			self.debug.write("%d " % value)
		self.data += struct.pack("!I", value)
		self.pos += 4

	# Appends a floating point number.
	def write_float(self, value):
		if self.debug:
			self.debug.write("%f " % value)
		self.data += struct.pack("!f", value)
		self.pos += 4

	# Appends a marker when in debug mode.
	def write_marker(self):
		if self.debug:
			self.debug.write("\n")

	# Appends a string.
	def write_string(self, value):
		if self.debug:
			self.debug.write("\"" + value + "\" ")
		self.data += value.encode()
		self.data += struct.pack('B', 0)
		self.pos += len(value) + 1

##############################################################################

class LIObjectExportSettingsOperator(bpy.types.Operator):
	bl_idname = "wm.lipsofsuna_export_settings"
	bl_label = "Exporting Settings"
	bl_description = 'Control what data is exported of this object'

	li_export = bpy.props.EnumProperty(name='Export mode', description='The type of data exported from the object', items=[\
		('particle', 'Particle animations', ''),\
		('shape', 'Collision shape', ''),\
		('render', 'Graphics', ''),\
		('none', 'Disabled', '')],
		default='none')
	li_file = bpy.props.StringProperty(name='File name', description='Target file name without path or extension')
	li_shape = bpy.props.StringProperty(name='Shape name', description='The name of the exported collision shape')

	def draw(self, context):
		layout = self.layout
		box = layout.column()
		box.prop(self, 'li_export')
		box.prop(self, 'li_file')
		box.prop(self, 'li_shape')

	def execute(self, context):
		def setprop(obj, name, value):
			try:
				if value:
					obj[name] = value
				else:
					del obj[name]
			except:
				pass
		if not len(self.li_file):
			try:
				path,name = os.path.split(bpy.data.filepath)
				self.li_file = os.path.splitext(name)[0]
			except:
				self.li_file = 'unnamed'
		if self.li_export == 'render':
			setprop(context.object, 'file', self.li_file)
			setprop(context.object, 'render', None)
			setprop(context.object, 'shape', None)
			setprop(context.object, 'export', None)
		elif self.li_export == 'shape':
			setprop(context.object, 'file', self.li_file)
			setprop(context.object, 'render', 'false')
			setprop(context.object, 'shape', self.li_shape or 'default')
			setprop(context.object, 'export', None)
		elif self.li_export == 'particle':
			setprop(context.object, 'file', self.li_file)
			setprop(context.object, 'render', 'false')
			setprop(context.object, 'shape', None)
			setprop(context.object, 'export', None)
		elif self.li_export == 'none':
			setprop(context.object, 'export', 'false')
		else:
			setprop(context.object, 'file', None)
			setprop(context.object, 'render', None)
			setprop(context.object, 'shape', None)
		return {'FINISHED'}

	def invoke(self, context, event):
		def getprop(obj, name):
			try:
				v = str(obj[name])
				if len(v):
					return v
				return None
			except:
				return None
		target = getprop(context.object, 'file')
		if not target:
			self.li_export = 'none'
			self.li_file = ''
			self.li_shape = ''
		elif getprop(context.object, 'render') == 'false':
			shape = getprop(context.object, 'shape')
			if shape:
				self.li_export = 'shape'
				self.li_file = target
				self.li_shape = shape
			else:
				self.li_export = 'particle'
				self.li_file = target
				self.li_shape = ''
		else:
			self.li_export = 'render'
			self.li_file = target
			self.li_shape = ''
		return context.window_manager.invoke_props_dialog(self)

class LIObjectPropertyPanel(bpy.types.Panel):
	bl_idname = "OBJECT_PT_lipsofsuna_object_property"
	bl_label = "Lips of Suna"
	bl_space_type = 'PROPERTIES'
	bl_region_type = 'WINDOW'
	bl_context = "object"
	bl_options = {'DEFAULT_CLOSED'}

	@classmethod
	def poll(cls, context):
		return (context.object is not None)

	def draw(self, context):
		layout = self.layout
		col = layout.column()
		col.operator("wm.lipsofsuna_export_settings", text="Exporting Settings", icon="FILE")

bpy.utils.register_class(LIObjectExportSettingsOperator)

##############################################################################

class LIExporter(bpy.types.Operator):
	'''Export to Lips of Suna (.lmdl)'''

	bl_idname = "export.lipsofsuna"
	bl_label = 'Export to Lips of Suna (.lmdl)'

	def execute(self, context):
		if self.process():
			bpy.context.scene.frame_set(bpy.context.scene.frame_current)
			return {'RUNNING_MODAL'}
		else:
			return {'FINISHED'}

	def invoke(self, context, event):
		LIFormat.files = []
		self.done = False
		self.state = 0
		self.files = []
		self.message = ""
		self.orig_layers = [x for x in bpy.context.scene.layers]
		self.orig_frame = bpy.context.scene.frame_current
		# Initialize the state.
		bpy.context.scene.layers = [True for x in range(0, 20)]
		if bpy.context.scene.objects.active:
			bpy.ops.object.mode_set(mode='OBJECT', toggle=False)
		# Find exported files.
		for obj in bpy.data.objects:
			for file in object_files(obj):
				if file not in self.files:
					self.files.append(file)
		self.files.sort()
		# Start exporting the files.
		# It seems that there's no way to update the GUI without user
		# input so we have to export in one pass even though our code
		# could easily do it iteratively.
		bpy.context.user_preferences.edit.use_global_undo = False
		while self.process():
			pass
		bpy.context.user_preferences.edit.use_global_undo = True
		return {'FINISHED'}

	def process(self):
		if self.state < len(self.files):
			# Get the processed file.
			if len(LIFormat.files) <= self.state:
				file = LIFile(self.files[self.state])
				LIFormat.files.append(file)
			else:
				file = LIFormat.files[self.state]
			# Process the file some.
			if file.process():
				file.write()
				self.state += 1
			self.message = file.get_progress_info()
			return True
		elif self.state == len(self.files):
			# Restore state.
			bpy.context.scene.layers = self.orig_layers
			bpy.context.scene.frame_set(self.orig_frame)
			# Show the results.
			bpy.ops.object.lipsofsuna_export_finished_operator('INVOKE_DEFAULT')
			self.state += 1
			return False
		else:
			return False

##############################################################################

def menu_func(self, context):
	self.layout.operator(LIExporter.bl_idname, text="Lips of Suna (.lmdl)")

def register():
	bpy.utils.register_module(__name__)
	bpy.types.INFO_MT_file_export.append(menu_func)

def unregister():
	bpy.utils.unregister_module(__name__)
	bpy.types.INFO_MT_file_export.remove(menu_func)

if __name__ == "__main__":
	register()

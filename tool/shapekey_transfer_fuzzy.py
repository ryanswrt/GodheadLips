import bpy, mathutils

def create_vertex_mapping(srcobj, dstobj):
	dict_dsttosrc = {}
	for i in range(len(srcobj.data.vertices)):
		srcvtx = srcobj.data.vertices[i]
		for j in range(len(dstobj.data.vertices)):
			dstvtx = dstobj.data.vertices[j]
			if (srcvtx.co - dstvtx.co).length < 0.01:
				dict_dsttosrc[j] = i
	return dict_dsttosrc

def shape_key_transfer_fuzzy(srcobj, srckey, dstobj, dstkey, dict_dsttosrc):
	for i in range(len(srckey.data)):
		if i in dict_dsttosrc:
			vtx_dstkey = dstkey.data[i]
			vtx_srckey = srckey.data[dict_dsttosrc[i]]
			vtx_dstkey.co = vtx_srckey.co

dstobj = bpy.context.active_object
if dstobj:
	for srcobj in bpy.context.selected_objects:
		if srcobj == dstobj:
			continue;
		# Disable undo.
		bpy.context.user_preferences.edit.use_global_undo = False
		toggled = False
		if dstobj.mode == "EDIT":
			toggled = True
			bpy.ops.object.editmode_toggle()
		# Copy shape keys.
		dict_dsttosrc = create_vertex_mapping(srcobj, dstobj)
		for srckey in srcobj.data.shape_keys.key_blocks:
			if not dstobj.data.shape_keys or srckey.name not in dstobj.data.shape_keys.key_blocks:
				dstobj.shape_key_add(name=srckey.name, from_mix=False)
			dstkey = dstobj.data.shape_keys.key_blocks[srckey.name]
			shape_key_transfer_fuzzy(srcobj, srckey, dstobj, dstkey, dict_dsttosrc)
		# Restore state.
		if toggled:
			bpy.ops.object.editmode_toggle()
		bpy.context.user_preferences.edit.use_global_undo = True
		break

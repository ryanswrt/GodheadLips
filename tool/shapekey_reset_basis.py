import bpy, mathutils

def reset_shape_key_verts(obj, bl_key, bl_basis):
	for bl_idx in range(len(bl_key.data)):
		vtx_key = bl_key.data[bl_idx]
		vtx_basis = bl_basis.data[bl_idx]
		vtx = obj.data.vertices[bl_idx]
		if vtx.select:
			diff = vtx_key.co - vtx_basis.co
			vtx_key.co = vtx_basis.co

obj = bpy.context.active_object
if obj and obj.data.shape_keys:
	key = obj.active_shape_key
	if "Basis" in obj.data.shape_keys.key_blocks:
		basis = obj.data.shape_keys.key_blocks["Basis"]
		bpy.context.user_preferences.edit.use_global_undo = False
		if obj.mode == "EDIT":
			toggled = True
			bpy.ops.object.editmode_toggle()
		reset_shape_key_verts(obj, key, basis)
		if toggled:
			bpy.ops.object.editmode_toggle()
		bpy.context.user_preferences.edit.use_global_undo = True

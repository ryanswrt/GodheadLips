import re, bpy, mathutils
from bpy_extras.object_utils import object_data_add

# \brief Bakes the normalmap of the object from its multires data.
def bake_normalmap():
	# Find the objects.
	old_objs = []
	for obj in bpy.context.selected_objects:
		obj.select = False
		old_objs.append(obj)
	# Duplicate the objects.
	new_objs = []
	for obj in old_objs:
		obj.select = True
		bpy.context.scene.objects.active = obj
		bpy.ops.object.duplicate(linked=False, mode='TRANSLATION')
		new_obj = bpy.context.active_object
		new_objs.append(new_obj)
		obj.select = False
	# Join the duplicated objects.
	for obj in new_objs:
		obj.select = True
		bpy.context.scene.objects.active = obj
	if len(new_objs):
		bpy.ops.object.join()
	obj_lo = bpy.context.active_object
	# Delete shape keys.
	while obj_lo.data.shape_keys:
		if len(obj_lo.data.shape_keys.key_blocks):
			obj_lo.active_shape_key_index = 1
		bpy.ops.object.shape_key_remove()
	# Delete materials.
	while obj_lo.active_material:
		bpy.ops.object.material_slot_remove()
	# Create the high and low poly meshes.
	bpy.ops.object.duplicate(linked=False, mode='INIT')
	obj_hi = bpy.context.active_object
	obj_hi.data = obj_hi.to_mesh(bpy.context.scene, True, 'RENDER')
	obj_hi.select = True
	obj_lo.data = obj_lo.to_mesh(bpy.context.scene, True, 'PREVIEW')
	bpy.context.scene.objects.active = obj_lo
	# Delete modifiers.
	for mod in obj_hi.modifiers:
		obj_hi.modifiers.remove(mod)
	for mod in obj_lo.modifiers:
		obj_lo.modifiers.remove(mod)
	# Setup baking.
	bpy.context.scene.render.bake_type = 'NORMALS'
	bpy.context.scene.render.bake_normal_space = 'TANGENT'
	bpy.context.scene.render.use_bake_selected_to_active = True
	# Bake the image.
	bpy.ops.object.bake_image()
	# Delete the objects.
	obj_lo.select = True
	obj_hi.select = True
	bpy.ops.object.delete()

bpy.context.user_preferences.edit.use_global_undo = False
bake_normalmap()
bpy.context.user_preferences.edit.use_global_undo = True

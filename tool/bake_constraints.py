import array, math, os, struct, re, sys, bpy, mathutils

def get_tail_rest_matrix(bone):
	length = mathutils.Vector((0, bone.length, 0))
	return bone.matrix_local.copy() * mathutils.Matrix.Translation(length)

def get_tail_pose_matrix(pose_bone):
	length = mathutils.Vector((0, pose_bone.length, 0))
	return pose_bone.matrix.copy() * mathutils.Matrix.Translation(length)

def get_rest_transform(bone):
	matrix = bone.matrix_local.copy()
	if bone.parent:
		matrix = get_tail_rest_matrix(bone.parent).inverted() * matrix
	loc = matrix.to_translation()
	rot = matrix.to_3x3().to_quaternion().normalized()
	return (loc, rot)

def get_pose_transform(bone, pose_bone):
	rest = bone.matrix_local.copy()
	pose = pose_bone.matrix.copy()
	if bone.parent:
		rest = get_tail_rest_matrix(bone.parent).inverted() * rest
		pose = get_tail_pose_matrix(pose_bone.parent).inverted() * pose
	matrix = rest.inverted() * pose
	return matrix

# Bakes the selected bones for the current frame.
def bake_selected_bones():
	for b in bpy.context.selected_pose_bones:
		armature = bpy.context.active_object.data
		b.matrix_basis = get_pose_transform(armature.bones[b.name], b)
		bpy.ops.anim.keyframe_insert_menu(type='LocRot')

# Bakes the frames required by the selected bones.
def bake_required_frames():
	# Get the list of constraint targets.
	targets = {}
	armature = bpy.context.active_object.data
	pose = bpy.context.active_object.pose
	for bone in pose.bones:
		for cons in bone.constraints:
			if cons.type == 'IK' or cons.type == 'COPY_ROTATION':
				if cons.target and cons.subtarget:
					targets[cons.subtarget] = True
	# Get the list of frames to bake.
	frames = {}
	action = bpy.context.active_object.animation_data.action
	groups = {action.groups[name] for name in targets if name in action.groups}
	for group in groups:
		def find_channel(name):
			name = "pose.bones[\"%s\"].location" % name
			for channel in group.channels:
				if channel.data_path == name:
					return channel
			return None
		channel = find_channel(group.name)
		if channel:
			for keyframe in channel.keyframe_points:
				frames[int(keyframe.co.x)] = True
	# Bake the detected frames.
	for frame in sorted(frames.keys()):
		bpy.context.scene.frame_set(frame)
		bpy.context.scene.frame_set(frame)
		bake_selected_bones()

bake_required_frames()


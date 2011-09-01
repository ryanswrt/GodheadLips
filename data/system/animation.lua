require "system/object"

if not Los.program_load_extension("animation") then
	error("loading extension `animation' failed")
end

------------------------------------------------------------------------------

--- Sets or clears an animation.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>animation: Animation name.</li>
--   <li>channel: Channel number.</li>
--   <li>fade_in: Fade in duration in seconds.</li>
--   <li>fade_out: Fade out duration in seconds.</li>
--   <li>node_weight: Sets blending weight multipliers for individual nodes.</li>
--   <li>weight: Blending weight.</li>
--   <li>time: Starting time.</li>
--   <li>time_scale: Time scaling factor.</li>
--   <li>permanent: True if should keep repeating.</li></ul>
--   <li>repeat_start: Starting time when repeating.</li>
-- @return True if started a new animation.
Object.animate = function(self, args)
	return Los.object_animate(self.handle, args)
end

--- Fades out an animation channel.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>channel: Channel number.</li>
--   <li>duration: Fade duration in seconds.</li></ul>
Object.animate_fade = function(self, args)
	return Los.object_animate_fade(self.handle, args)
end

--- Edits the pose of a node.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>channel: Channel number.</li>
--   <li>frame: Frame number.</li>
--   <li>node: Node name.</li>
--   <li>position: Position change relative to rest pose.</li>
--   <li>rotation: Rotation change relative to rest pose.</li>
--   <li>scale: Scale factor.</li></ul>
Object.edit_pose = function(self, args)
	local p = args.position and args.position.handle
	local r = args.rotation and args.rotation.handle
	return Los.object_edit_pose(self.handle, {channel = args.channel, frame = args.frame, node = args.node, position = p, rotation = r, scale = args.scale})
end

--- Finds a bone or an anchor by name.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>name: Node name.</li>
--   <li>space: Coordinate space. ("local"/"world")</li></ul>
-- @return Position and rotation, or nil if not found.
Object.find_node = function(self, args)
	local p,r = Los.object_find_node(self.handle, args)
	if not p then return end
	return Class.new(Vector, {handle = p}), Class.new(Quaternion, {handle = r})
end

--- Gets animation information for the given animation channel.<br/>
-- If an animation is looping in the channel, a table containing the fields
-- animation, time, and weight is returned.
-- @param self Server class.
-- @param args Arguments.<ul>
--   <li>channel: Channel number. (required)</li></ul>
-- @return Animation info table or nil.
Object.get_animation = function(self, args)
	return Los.object_get_animation(self.handle, args)
end

--- Gets the list of active animations.
-- @param self Server class.
-- @return Animation list.
Object.get_animations = function(self)
	return Los.object_get_animations(self.handle)
end

--- Updates the animations of the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>secs: Tick length.</li></ul>
Object.update_animations = function(self, args)
	return Los.object_update_animations(self.handle, args)
end

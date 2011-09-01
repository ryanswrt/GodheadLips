if not Los.program_load_extension("camera") then
	error("loading extension `camera' failed")
end

Camera = Class()
Camera.class_name = "Camera"

--- Creates a new camera.
-- @param clss Camera class.
-- @param args Arguments.
Camera.new = function(clss, args)
	local self = Class.new(clss)
	self.handle = Los.camera_new()
	__userdata_lookup[self.handle] = self
	if args then
		for k,v in pairs(args) do self[k] = v end
	end
	return self
end

--- Moves the camera forward or backward.
-- @param self Camera.
-- @param args Arguments.<ul>
--   <li>rate: Movement rate.</li>
--   <li>keep: True if should keep moving.</li></ul>
Camera.move = function(self, args)
	Los.camera_move(self.handle, args)
end

--- Creates a picking ray for the current camera configuration.
-- @param self Camera.
-- @param args Arguments.<ul>
--   <li>cursor: Cursor position, in pixels.</li>
--   <li>far: Ray end distance, in world coordinate units.</li>
--   <li>near: Ray start distance, in world coordinate units.</li></ul>
-- @return Start point, end point.
Camera.picking_ray = function(self, args)
	local a,b = Los.camera_picking_ray(self.handle, {cursor = args.cursor.handle, far = args.far, near = args.near})
	return Class.new(Vector, {handle = a}), Class.new(Vector, {handle = b})
end

--- Resets the look spring transformation of the camera.
-- @param self Camera.
Camera.reset = function(self)
	Los.camera_reset(self.handle)
end

--- Sets the tilting rate of the camera.
-- @param self Camera.
-- @param args Arguments.<ul>
--   <li>rate: Tilting rate.</li>
--   <li>keep: True if should keep tilting.</li></ul>
Camera.tilt = function(self, args)
	Los.camera_tilt(self.handle, args)
end

--- Sets the turning rate of the camera.
-- @param self Camera.
-- @param args Arguments.<ul>
--   <li>rate: Turning rate.</li>
--   <li>keep: True if should keep turning.</li></ul>
Camera.turn = function(self, args)
	Los.camera_turn(self.handle, args)
end

--- Warps the camera to the target point.
-- @param self Camera.
Camera.warp = function(self)
	Los.camera_warp(self.handle, args)
end

--- Animates the camera.
--
-- @param self Camera.
-- @param args Arguments.<ul>
--   <li>1,secs: Tick length.</li></ul>
Camera.update = function(self, args)
	Los.camera_update(self.handle, args)
end

--- Adjusts the zoom of the camera.
-- @param self Camera.
-- @param args Arguments.<ul>
--   <li>rate: Zooming rate.</li>
--   <li>keep: True if should keep zooming.</li></ul>
Camera.zoom = function(self, args)
	Los.camera_zoom(self.handle, args)
end

--- The collision group of the camera.
-- @name Camera.collision_group
-- @class table

--- The collision mask of the camera.
-- @name Camera.collision_mask
-- @class table

--- Far plane of the camera.
-- @name Camera.far
-- @class table

--- The field of view of the camera.
-- @name Camera.fov
-- @class table

--- Camera mode.<br/>
-- Recognized values: "first-person"/"manual"/"third-person".
-- @name Camera.mode
-- @class table

--- Modelview matrix.
-- @name Camera.modelview
-- @class table

--- Near plane of the camera.
-- @name Camera.far
-- @class table

--- Camera position.
-- @name Camera.position
-- @class table

--- The position smoothing factor of the camera.
-- @name Camera.position_smoothing
-- @class table

--- Projection matrix.
-- @name Camera.projection
-- @class table

--- Camera rotation.
-- @name Camera.rotation
-- @class table

--- The rotation smoothing factor of the camera.
-- @name Camera.rotation_smoothing
-- @class table

--- The position of the target of third person camera.
-- @name Camera.center
-- @class table

--- The rotation of the target of third person camera.
-- @name Camera.center
-- @class table

--- The viewport of the camera.
-- @name Camera.viewport
-- @class table

Camera:add_getters{
	collision_group = function(s) return Los.camera_get_collision_group(s.handle) end,
	collision_mask = function(s) return Los.camera_get_collision_mask(s.handle) end,
	fov = function(s) return Los.camera_get_fov(s.handle) end,
	mode = function(s) return Los.camera_get_mode(s.handle) end,
	modelview = function(s) return Los.camera_get_modelview(s.handle) end,
	position = function(s) return Class.new(Vector, {handle = Los.camera_get_position(s.handle)}) end,
	position_smoothing = function(s) return Los.camera_get_position_smoothing(s.handle) end,
	projection = function(s) return Los.camera_get_projection(s.handle) end,
	rotation = function(s) return Class.new(Quaternion, {handle = Los.camera_get_rotation(s.handle)}) end,
	rotation_smoothing = function(s) return Los.camera_get_rotation_smoothing(s.handle) end,
	target_position = function(s) return Class.new(Vector, {handle = Los.camera_get_target_position(s.handle)}) end,
	target_rotation = function(s) return Class.new(Quaternion, {handle = Los.camera_get_target_rotation(s.handle)}) end,
	viewport = function(s) return Los.camera_get_viewport(s.handle) end}

Camera:add_setters{
	collision_group = function(s, v) Los.camera_set_collision_group(s.handle, v) end,
	collision_mask = function(s, v) Los.camera_set_collision_mask(s.handle, v) end,
	far = function(s, v) Los.camera_set_far(s.handle, v) end,
	fov = function(s, v) Los.camera_set_fov(s.handle, v) end,
	mode = function(s, v) Los.camera_set_mode(s.handle, v) end,
	near = function(s, v) Los.camera_set_near(s.handle, v) end,
	position_smoothing = function(s, v) Los.camera_set_position_smoothing(s.handle, v) end,
	rotation_smoothing = function(s, v) Los.camera_set_rotation_smoothing(s.handle, v) end,
	target_position = function(s, v) Los.camera_set_target_position(s.handle, v.handle) end,
	target_rotation = function(s, v) Los.camera_set_target_rotation(s.handle, v.handle) end,
	viewport = function(s, v) Los.camera_set_viewport(s.handle, v) end}

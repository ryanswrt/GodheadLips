require "system/class"
require "system/math"

if not Los.program_load_extension("graphics") then
	error("loading extension `graphics' failed")
end

------------------------------------------------------------------------------

--- Clears the screen.
-- @param clss Program class.
Program.clear_buffer = function(clss)
	Los.client_clear_buffer()
end

--- Launches a server.
-- @param clss Program class.
-- @param args Arguments.<ul>
--   <li>1,args: String to pass to the server.</li></ul>
-- @return True on success.
Program.launch_server = function(clss, args)
	return Los.client_host(args)
end

--- Takes a screenshot.
-- @param clss Program class.
-- @return Screenshot filename.
Program.capture_screen = function(clss)
	return Los.client_screenshot()
end

--- Sets the current video mode.
-- @param clss Program class.
-- @param ... Arguments.<ul>
--   <li>1,width: Window width in pixels.</li>
--   <li>2,height: Window height in pixels.</li>
--   <li>3,fullscreen: True for fullscreen.</li></ul>
-- @return True on success.
Program.set_video_mode = function(clss, ...)
	return Los.client_set_video_mode(...)
end

--- Copies the rendered scene to the screen.
-- @param clss Program class.
Program.swap_buffers = function(clss)
	Los.client_swap_buffers()
end

--- Movement mode flag.
-- @name Program.cursor_grabbed
-- @class table

--- The current cursor position.
-- @name Program.cursor_position
-- @class table

--- Short term average frames per second.
-- @name Program.fps
-- @class table

--- Gets the version of the OpenGL renderer used.
-- @name Program.opengl_version
-- @class table

--- Current video mode (read-only).
-- @name Program.video_mode
-- @class table

--- List of supported fullscreen modes (read-only).
-- @name Program.video_modes
-- @class table

--- Main window title.
-- @name Program.window_title
-- @class table

Program:add_class_getters{
	cursor_grabbed = function(s) return Los.client_get_moving() end,
	cursor_position = function(s) return Class.new(Vector, {handle = Los.client_get_cursor_pos()}) end,
	fps = function(s) return Los.client_get_fps() end,
	opengl_version = function(s) return Los.program_get_opengl_version() end,
	video_mode = function(s) return Los.client_get_video_mode() end,
	video_modes = function(s) return Los.client_get_video_modes() end,
	window_title = function(s) return s.window_title or "" end}

Program:add_class_setters{
	cursor_grabbed = function(s, v) Los.client_set_moving(v) end,
	window_title = function(s, v)
		s._window_title = v
		Los.client_set_title(v)
	end}

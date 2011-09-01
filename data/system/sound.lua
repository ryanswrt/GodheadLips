require "system/class"
require "system/math"

if not Los.program_load_extension("sound") then
	error("loading extension `sound' failed")
end

------------------------------------------------------------------------------

Sound = Class()
Sound.class_name = "Sound"

--- Plays a sound effect.
-- @param clss Sound class.
-- @param args Arguments.<ul>
--   <li>effect: Sample string.</li>
--   <li>object: Object.</li>
--   <li>pitch: Pitch shift multiplier.</li>
--   <li>positional: False to make the sound non-positional.</li>
--   <li>repeating: True to make the sound repeat.</li>
--   <li>velocity: Velocity vector.</li>
--   <li>volume: Volume multiplier.</li></ul>
Sound.effect = function(clss, args)
	Los.sound_effect{effect = args.effect, object = args.object.handle, pitch = args.pitch, positional = args.positional,
		repeating = args.repeating, velocity = args.velocity and args.velocity.handle, volume = args.volume}
end

--- Position of the listener.
-- @name Sound.listener_position
-- @class table

--- Rotation of the listener.
-- @name Sound.listener_rotation
-- @class table

--- Velocity of the listener.
-- @name Sound.listener_velocity
-- @class table

--- Music track name.
-- @name Sound.music
-- @class table

--- Music fading time.
-- @name Sound.music_fading
-- @class table

--- Music offset in seconds.
-- @name Sound.music_offset
-- @class table

--- Music volume.
-- @name Sound.music_volume
-- @class table

Sound.class_getters = {
	listener_position = function(s) return Class.new(Vector, {handle = Los.sound_get_listener_position()}) end,
	listener_rotation = function(s) return Class.new(Vector, {handle = Los.sound_get_listener_rotation()}) end,
	listener_velocity = function(s) return Class.new(Vector, {handle = Los.sound_get_listener_velocity()}) end,
	music_offset = function(s) return Los.sound_get_music_offset() end}

Sound.class_setters = {
	listener_position = function(s, v) return Los.sound_set_listener_position(v.handle) end,
	listener_rotation = function(s, v) return Los.sound_set_listener_rotation(v.handle) end,
	listener_velocity = function(s, v) return Los.sound_set_listener_velocity(v.handle) end,
	music = function(s, v) Los.sound_set_music(v) end,
	music_fading = function(s, v) Los.sound_set_music_fading(v) end,
	music_offset = function(s, v) Los.sound_set_music_offset(v) end,
	music_volume = function(s, v) Los.sound_set_music_volume(v) end}

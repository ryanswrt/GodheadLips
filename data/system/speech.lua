require "system/object"

if not Los.program_load_extension("speech") then
	error("loading extension `speech' failed")
end

------------------------------------------------------------------------------

Speech = Class()
Speech.class_name = "Speech"

--- Displays a message above the object.
-- @param self Speech class.
-- @param args Arguments.<ul>
--   <li>diffuse: Diffuse color.</li>
--   <li>fade_exponent: Fade exponent.</li>
--   <li>fade_time: Fade time in seconds, or nil.</li>
--   <li>life_time: Life time in seconds, or nil.</li>
--   <li>font: Font name or nil.</li>
--   <li>object: Object.</li>
--   <li>message: Speech string.</li></ul>
Speech.add = function(self, args)
	Los.speech_add{diffuse = args.diffuse, fade_exponent = args.fade_exponent,
		fade_time = args.fade_time, life_time = args.life_time, font = args.font,
		object = args.object.handle, message = args.message}
end

--- Draws the speech texts to the framebuffer.
-- @param self Speech class.
-- @param args Arguments.<ul>
--   <li>modelview: Modelview matrix.</li>
--   <li>projection: Projection matrix.</li>
--   <li>viewport: Viewport array.</li></ul>
Speech.draw = function(self, args)
	Los.speech_draw(args)
end

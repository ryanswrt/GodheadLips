require "system/object"

if not Los.program_load_extension("object-render") then
	error("loading extension `object-render' failed")
end

------------------------------------------------------------------------------

--- Deforms the mesh of the object according to its animation pose.
-- @param self Object.
Object.deform_mesh = function(self)
	return Los.object_deform_mesh(self.handle)
end

--- Starts the particle animation of the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>loop: True to loop infinitely.</li>
--   <li>time: Animation offset in seconds.</li></ul>
Object.particle_animation = function(self, args)
	return Los.object_particle_animation(self.handle, args)
end

--- Set or clears the additional effect layer of the object.
-- @param self Object.
-- @param ... Arguments.<ul>
--   <li>params: Array numbers to be passed to the shader.</li>
--   <li>shader: Shader name.</li></ul>
Object.set_effect = function(self, ...)
	Los.object_set_effect(self.handle, ...)
end

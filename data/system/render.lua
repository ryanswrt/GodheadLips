require "system/class"
require "system/math"

if not Los.program_load_extension("render") then
	error("loading extension `render' failed")
end

------------------------------------------------------------------------------

Light = Class()
Light.class_name = "Light"

--- Creates a new light source.
-- @param clss Light class.
-- @param args Arguments.
-- @return New light source.
Light.new = function(clss, args)
	local self = Class.new(clss)
	self.handle = Los.light_new()
	__userdata_lookup[self.handle] = self
	if args then
		for k,v in pairs(args) do self[k] = v end
	end
	return self
end

--- The ambient color of the light source.
-- @name Light.ambient
-- @class table

--- The diffuse color of the light source.
-- @name Light.diffuse
-- @class table

--- Enables or disables the light.
-- @name Light.enabled
-- @class table

--- The attenuation equation of the light source.
-- @name Light.equation
-- @class table

--- Gets or sets the position of the light.
-- @name Light.position
-- @class table

--- The priority of the light, higher means more important.
-- @name Light.priority
-- @class table

--- Gets or sets the rotation of the light.
-- @name Light.rotation
-- @class table

--- Enables or disables shadow casting.
-- @name Light.shadow_casting
-- @class table

--- Far place distance of the shadow projection.
-- @name Light.shadow_far
-- @class table

--- Near place distance of the shadow projection.
-- @name Light.shadow_near
-- @class table

--- The specular color of the light source.
-- @name Light.specular
-- @class table

--- Spot cutoff angle of the light, in radians.
-- @name Light.spot_cutoff
-- @class table

--- Spot exponent of the light.
-- @name Light.spot_cutoff
-- @class table

Light:add_getters{
	ambient = function(s) return Los.light_get_ambient(s.handle) end,
	diffuse = function(s) return Los.light_get_diffuse(s.handle) end,
	enabled = function(s) return Los.light_get_enabled(s.handle) end,
	equation = function(s) return Los.light_get_equation(s.handle) end,
	position = function(s) return Class.new(Vector, {handle = Los.light_get_position(s.handle)}) end,
	priority = function(s) return Los.light_get_priority(s.handle) end,
	rotation = function(s) return Class.new(Quaternion, {handle = Los.light_get_rotation(s.handle)}) end,
	shadow_casting = function(s) return Los.light_get_shadow_casting(s.handle) end,
	shadow_far = function(s) return Los.light_get_shadow_far(s.handle) end,
	shadow_near = function(s) return Los.light_get_shadow_near(s.handle) end,
	specular = function(s) return Los.light_get_specular(s.handle) end,
	spot_cutoff = function(s) return Los.light_get_spot_cutoff(s.handle) end,
	spot_exponent = function(s) return Los.light_get_spot_exponent(s.handle) end}

Light:add_setters{
	ambient = function(s, v) Los.light_set_ambient(s.handle, v) end,
	diffuse = function(s, v) Los.light_set_diffuse(s.handle, v) end,
	enabled = function(s, v) Los.light_set_enabled(s.handle, v) end,
	equation = function(s, v) Los.light_set_equation(s.handle, v) end,
	position = function(s, v) Los.light_set_position(s.handle, v.handle) end,
	priority = function(s, v) Los.light_set_priority(s.handle, v) end,
	rotation = function(s, v) Los.light_set_rotation(s.handle, v.handle) end,
	shadow_casting = function(s, v) Los.light_set_shadow_casting(s.handle, v) end,
	shadow_far = function(s, v) Los.light_set_shadow_far(s.handle, v) end,
	shadow_near = function(s, v) Los.light_set_shadow_near(s.handle, v) end,
	specular = function(s, v) Los.light_set_specular(s.handle, v) end,
	spot_cutoff = function(s, v) Los.light_set_spot_cutoff(s.handle, v) end,
	spot_exponent = function(s, v) Los.light_set_spot_exponent(s.handle, v) end}

Light.unittest = function()
	-- Position.
	local l = Light{position = Vector(1,2,3)}
	assert(l.position.x == 1)
	assert(l.position.y == 2)
	assert(l.position.z == 3)
	-- Equation.
	l.equation = {3,2,1}
	assert(l.equation[1] == 3)
	assert(l.equation[2] == 2)
	assert(l.equation[3] == 1)
	-- Enabling.
	l.enabled = false
	assert(l.enabled == false)
	l.enabled = true
	assert(l.enabled == true)
end

------------------------------------------------------------------------------

Render = Class()
Render.class_name = "Render"

--- Anisotrophic filtering setting.
-- @name Render.anisotrophy
-- @class table

Render.class_getters = {
	anisotrophy = function(s) return Los.render_get_anisotrophy() end}

Render.class_setters = {
	anisotrophy = function(s, v) Los.render_set_anisotrophy(v) end}

------------------------------------------------------------------------------

Scene = Class()
Scene.class_name = "Scene"

--- Creates a new scene rendering buffer.
-- @param clss Scene class.
-- @param args Arguments.
-- @return Scene.
Scene.new = function(clss, args)
	local self = Class.new(clss)
	self.handle = Los.scene_new()
	__userdata_lookup[self.handle] = self
	if args then
		for k,v in pairs(args) do self[k] = v end
	end
	return self
end

--- Renders the scene.
-- @param self Scene.
-- @param args Arguments.<ul>
--   <li>hdr: True to enable HDR.</li>
--   <li>modelview: Modelview matrix.</li>
--   <li>multisamples: Number of multisamples. Pass arguments:</li>
--   <li>postproc_passes: Array of post-processing passes.<ul>
--     <li>mipmaps: True to generate mipmaps.</li>
--     <li>shader: Post-processing shader name.</li></ul></li>
--   <li>projection: Projection matrix.</li>
--   <li>render_passes: Array of render passes. Pass arguments:<ul>
--     <li>pass: Pass number.</li>
--     <li>sorting: True to enable sorting.</li></ul></li>
--   <li>viewport: Viewport array.</li></ul>
Scene.render = function(self, args)
	Los.scene_render(self.handle, args)
end

------------------------------------------------------------------------------

Shader = Class()
Shader.class_name = "Shader"

--- Creates a new shader.
-- @param clss Shader class.
-- @param args Arguments.<ul>
--   <li>name: Unique shader name.</li>
--   <li>sort: True to allow depth sorting for materials with this shader.</li>
--   <li>transform_feedback: True to enable transform feedback.</li>
--   <li>passN_blend: True to enable blending.</li>
--   <li>passN_blend_src: Source blend function.</li>
--   <li>passN_blend_dst: Destination blend function.</li>
--   <li>passN_depth_test: False to disable depth test.</li>
--   <li>passN_depth_write: False to disable depth writes.</li>
--   <li>passN_depth_func: Depth test function.</li>
--   <li>passN_fragment: Fragment program code.</li>
--   <li>passN_geometry: Geometry program code.</li>
--   <li>passN_vertex: Vertex program code.</li></ul>
-- @return New shader.
Shader.new = function(clss, args)
	local self = Class.new(clss)
	self.handle = Los.shader_new(args)
	__userdata_lookup[self.handle] = self
	return self
end

--- Recompiles the shader from new source code.
-- @param self Shader.
-- @param args Arguments.<ul>
--   <li>sort: True to allow depth sorting for materials with this shader.</li>
--   <li>transform_feedback: True to enable transform feedback.</li>
--   <li>passN_blend: True to enable blending.</li>
--   <li>passN_blend_src: Source blend function.</li>
--   <li>passN_blend_dst: Destination blend function.</li>
--   <li>passN_depth_test: False to disable depth test.</li>
--   <li>passN_depth_write: False to disable depth writes.</li>
--   <li>passN_depth_func: Depth test function.</li>
--   <li>passN_fragment: Fragment program code.</li>
--   <li>passN_geometry: Geometry program code.</li>
--   <li>passN_vertex: Vertex program code.</li></ul>
Shader.compile = function(self, args)
	Los.shader_compile(self.handle, args)
end

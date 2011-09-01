Bloom = Class()
Bloom.exposure = Options.inst.bloom_exposure   -- The intensity of the bloom halo.
Bloom.luminance = Options.inst.bloom_luminance -- The smallest luminance value that gets bloomed.

--- Initializes the bloom filter.
-- @param clss Bloom class.
Bloom.init = function(clss)
	clss.shader = Shader{name = "postprocess-hdr"}
	clss:compile()
end

--- Recompiles the bloom filters.
-- @param clss Bloom class.
Bloom.compile = function(clss)
	clss.shader:compile{
		pass1_depth_test = false,
		pass1_depth_write = false,
		pass1_vertex = clss.code_vertex,
		pass1_fragment = clss:code_fragment()}
end

------------------------------------------------------------------------------

Bloom.code_vertex = [[
out vec2 var_texcoord;
void main()
{
	var_texcoord = LOS_texcoord;
	gl_Position = vec4(LOS_coord, 1.0);
}]]

Bloom.code_fragment = function(clss)
return [[
in vec2 var_texcoord;
const float bloom_exposure = ]] .. clss.exposure .. [[;
const float bloom_luminance = ]] .. clss.luminance .. [[;
vec4 los_postproc_bloom()
{
	/* An approximate blurred sample is calculated with mipmap abuse. The
	   sampling points are intentionally placed between pixels and mipmap
	   levels to offload some of the blurring to the texture sampler. */
	vec4 d = vec4(LOS_material_param_0.xy, 0.5*LOS_material_param_0.xy);
	vec3 h1 = textureLod(LOS_diffuse_texture_0, var_texcoord - 3.5 * d.xw - d.wy, 6.5).rgb;
	vec3 h2 = textureLod(LOS_diffuse_texture_0, var_texcoord + 3.5 * d.xw + d.wy, 6.5).rgb;
	vec3 h3 = textureLod(LOS_diffuse_texture_0, var_texcoord - 1.5 * d.xw + d.wy, 2.5).rgb;
	vec3 h4 = textureLod(LOS_diffuse_texture_0, var_texcoord + 1.5 * d.xw - d.wy, 2.5).rgb;
	vec3 v1 = textureLod(LOS_diffuse_texture_0, var_texcoord - 3.5 * d.wy - d.xw, 6.5).rgb;
	vec3 v2 = textureLod(LOS_diffuse_texture_0, var_texcoord + 3.5 * d.wy + d.xw, 6.5).rgb;
	vec3 v3 = textureLod(LOS_diffuse_texture_0, var_texcoord - 1.5 * d.wy + d.xw, 2.5).rgb;
	vec3 v4 = textureLod(LOS_diffuse_texture_0, var_texcoord + 1.5 * d.wy - d.xw, 2.5).rgb;
	vec3 sample = 0.125*(h1+h2+h3+h4+v1+v2+v3+v4);
	float luminance = dot(sample, vec3(0.3, 0.59, 0.11));
	float bloomed = max(0.0, luminance - bloom_luminance);
	float exposure = 1.0 - exp(-bloom_exposure * bloomed);
	vec3 bloom = sample * exposure;
	return vec4(bloom, 1.0);
}
void main()
{
	vec4 bloom = los_postproc_bloom();
	vec4 color = texture(LOS_diffuse_texture_0, var_texcoord);
	LOS_output_0 = color + bloom;
}]]
end

------------------------------------------------------------------------------

Bloom:init()

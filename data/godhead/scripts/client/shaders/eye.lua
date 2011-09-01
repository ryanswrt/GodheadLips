Shader{name = "eye",

-- Low quality program.
-- No lighting, color adjustment.
low = {
pass4_animated = true,
pass4_depth_func = "lequal",
pass4_depth_write = true,
pass4_vertex = [[
out vec2 F_texcoord;
void main()
{
	]] .. Shader.los_animation_default() .. [[
	vec4 tmp = LOS_matrix_modelview * vec4(anim_coord,1.0);
	F_texcoord = LOS_texcoord;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass4_fragment = [[
in vec2 F_texcoord;
void main()
{
	vec4 diffuse = texture(LOS_diffuse_texture_0, F_texcoord);
	LOS_output_0 = mix(LOS_material_diffuse, vec4(diffuse.rgb,1.0), diffuse.a);
}]]},

-- Medium quality program.
-- Fragment lighting, color adjustment.
medium = {
pass1_animated = true,
pass1_color_write = false,
pass1_depth_func = "lequal",
pass1_vertex = [[
void main()
{
	]] .. Shader.los_animation_default() .. [[
	vec4 tmp = LOS_matrix_modelview * vec4(anim_coord,1.0);
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass1_fragment = [[
void main()
{
}]],
pass4_animated = true,
pass4_depth_func = "equal",
pass4_depth_write = false,
pass4_vertex = [[
out vec3 F_coord;
out vec3 F_halfvector[LOS_LIGHT_MAX];
out vec3 F_lightvector[LOS_LIGHT_MAX];
out vec3 F_normal;
out vec2 F_texcoord;
void main()
{
	]] .. Shader.los_animation_default() .. [[
	vec4 tmp = LOS_matrix_modelview * vec4(anim_coord,1.0);
	]] .. Shader.los_lighting_vectors("F_lightvector", "F_halfvector", "tmp.xyz") .. [[
	F_coord = tmp.xyz;
	F_normal = LOS_matrix_normal * LOS_normal;
	F_texcoord = LOS_texcoord;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass4_fragment = [[
in vec3 F_coord;
in vec3 F_halfvector[LOS_LIGHT_MAX];
in vec3 F_lightvector[LOS_LIGHT_MAX];
in vec3 F_normal;
in vec2 F_texcoord;
void main()
{
	vec3 normal = normalize(F_normal);
	vec4 diffuse = texture(LOS_diffuse_texture_0, F_texcoord);
	diffuse = mix(LOS_material_diffuse, vec4(diffuse.rgb,1.0), diffuse.a);
	]] .. Shader.los_lighting_default("F_coord", "normal", "F_lightvector", "F_halfvector") .. [[
	LOS_output_0 = diffuse * lighting;
}]]}}

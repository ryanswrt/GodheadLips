Shader{name = "animslime",

-- Medium quality program.
medium = {
sort = true,
pass6_animated = true,
pass6_blend = true,
pass6_blend_src = "src_alpha",
pass6_blend_dst = "one_minus_src_alpha",
pass6_depth_func = "lequal",
pass6_depth_write = false,
pass6_vertex = [[
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
	F_normal = LOS_matrix_normal * anim_normal;
	F_texcoord = LOS_texcoord;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass6_fragment = [[
in vec3 F_coord;
in vec3 F_halfvector[LOS_LIGHT_MAX];
in vec3 F_lightvector[LOS_LIGHT_MAX];
in vec3 F_normal;
in vec2 F_texcoord;
void main()
{
	vec3 normal = normalize(F_normal);
	vec4 diffuse = texture2D(LOS_diffuse_texture_0, F_texcoord);
	]] .. Shader.los_lighting_default("F_coord", "normal", "F_lightvector", "F_halfvector") .. [[
	LOS_output_0 = LOS_material_diffuse * diffuse * lighting;
}]]}}

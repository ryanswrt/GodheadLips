Shader{name = "adamantium",

-- Medium quality program.
medium = {
pass1_color_write = false,
pass1_depth_func = "lequal",
pass1_vertex = [[
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass1_fragment = [[
void main()
{
}]],
pass4_blend = true,
pass4_blend_src = "one",
pass4_blend_dst = "one",
pass4_depth_func = "equal",
pass4_depth_write = false,
pass4_vertex = [[
out vec3 F_coord;
out vec3 F_halfvector[LOS_LIGHT_MAX];
out vec3 F_lightvector[LOS_LIGHT_MAX];
out vec3 F_normal;
out vec2 F_texcoord;
out vec2 F_texcoord1;
out float F_splatting;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	]] .. Shader.los_lighting_vectors("F_lightvector", "F_halfvector", "tmp.xyz") .. [[
	F_coord = tmp.xyz;
	F_normal = LOS_matrix_normal * LOS_normal;
	vec3 refr = normalize(reflect(normalize(F_coord), normalize(F_normal)));
	F_texcoord = LOS_texcoord;
	F_texcoord1 = 0.99 * LOS_texcoord + 0.88 * (refr.xy + refr.zz);
	F_splatting = length(LOS_normal) - 1.0;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass4_fragment = [[
in vec3 F_coord;
in vec3 F_halfvector[LOS_LIGHT_MAX];
in vec3 F_lightvector[LOS_LIGHT_MAX];
in vec3 F_normal;
in vec2 F_texcoord;
in vec2 F_texcoord1;
in float F_splatting;
void main()
{
	vec3 normal = normalize(F_normal);
	vec4 diffuse0 = mix(texture(LOS_diffuse_texture_0, F_texcoord), texture(LOS_diffuse_texture_0, F_texcoord1), 0.5);
	vec4 diffuse1 = texture(LOS_diffuse_texture_1, F_texcoord);
	vec4 diffuse = LOS_material_diffuse * mix(diffuse0, diffuse1, F_splatting);
	vec4 specular_splat = mix(LOS_material_specular, vec4(0.5), F_splatting);
	float shininess_splat = mix(LOS_material_shininess, 1.0, clamp(F_splatting, 0.0, 1.0));
	]] .. Shader.los_lighting_default("F_coord", "normal", "F_lightvector", "F_halfvector",
		"specular_splat", "shininess_splat") .. [[
	LOS_output_0 = LOS_material_diffuse * diffuse * lighting;
}]]}}

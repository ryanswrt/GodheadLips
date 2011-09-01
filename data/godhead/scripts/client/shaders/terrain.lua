Shader{name = "terrain",

-- Low quality program.
-- No lighting.
low = {
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
pass4_depth_func = "equal",
pass4_depth_write = false,
pass4_vertex = [[
out vec4 F_color;
out vec2 F_texcoord;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	F_color = LOS_color;
	F_texcoord = LOS_texcoord;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass4_fragment = [[
in vec4 F_color;
in vec2 F_texcoord;
void main()
{
	vec4 diffuse = texture(LOS_diffuse_texture_0, F_texcoord);
	LOS_output_0 = F_color * LOS_material_diffuse * diffuse;
}]]},

-- Medium quality program.
-- Normal mapping, texture splatting, fragment lighting.
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
pass4_depth_func = "equal",
pass4_depth_write = false,
pass4_vertex = [[
out vec4 F_color;
out vec3 F_coord;
out vec3 F_halfvector[LOS_LIGHT_MAX];
out vec3 F_lightvector[LOS_LIGHT_MAX];
out vec3 F_normal;
out vec3 F_tangent;
out vec2 F_texcoord;
out float F_splatting;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	]] .. Shader.los_lighting_vectors("F_lightvector", "F_halfvector", "tmp.xyz") .. [[
	F_color = LOS_color;
	F_coord = tmp.xyz;
	F_normal = LOS_matrix_normal * LOS_normal;
	F_tangent = LOS_matrix_normal * LOS_tangent;
	F_texcoord = LOS_texcoord;
	F_splatting = 1.0 - LOS_color.a;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass4_fragment = Shader.los_normal_mapping .. [[
in vec4 F_color;
in vec3 F_coord;
in vec3 F_halfvector[LOS_LIGHT_MAX];
in vec3 F_lightvector[LOS_LIGHT_MAX];
in vec3 F_normal;
in vec3 F_tangent;
in vec2 F_texcoord;
in float F_splatting;
void main()
{
	vec3 normal = los_normal_mapping(F_normal, F_tangent, F_texcoord, LOS_diffuse_texture_1);
	normal = mix(normal, normalize(F_normal), F_splatting);
	vec4 diffuse0 = texture(LOS_diffuse_texture_0, F_texcoord);
	vec4 diffuse1 = texture(LOS_diffuse_texture_2, F_texcoord);
	vec4 diffuse = LOS_material_diffuse * mix(diffuse0, diffuse1, F_splatting);
	vec4 specular_splat = mix(LOS_material_specular, vec4(0.5), F_splatting);
	float shininess_splat = mix(1.0, 1.0, F_splatting);
	]] .. Shader.los_lighting_default("F_coord", "normal", "F_lightvector", "F_halfvector",
		"specular_splat", "shininess_splat") .. [[
	LOS_output_0 = diffuse * max(vec4(-1.0) + F_color + lighting, vec4(0.0));
	LOS_output_0.a = 1.0;
}]]}}

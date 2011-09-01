Shader{name = "diffnormspec",

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
out vec2 F_texcoord;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	F_texcoord = LOS_texcoord;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass4_fragment = [[
in vec2 F_texcoord;
void main()
{
	vec4 diffuse = texture(LOS_diffuse_texture_0, F_texcoord);
	LOS_output_0 = LOS_material_diffuse * diffuse;
}]]},

-- Medium quality program.
-- Normal mapping, specular mapping.
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
out vec3 F_coord;
out vec3 F_halfvector[LOS_LIGHT_MAX];
out vec3 F_lightvector[LOS_LIGHT_MAX];
out vec3 F_normal;
out vec3 F_tangent;
out vec2 F_texcoord;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	]] .. Shader.los_lighting_vectors("F_lightvector", "F_halfvector", "tmp.xyz") .. [[
	F_coord = tmp.xyz;
	F_normal = LOS_matrix_normal * LOS_normal;
	F_tangent = LOS_matrix_normal * LOS_tangent;
	F_texcoord = LOS_texcoord;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass4_fragment = Shader.los_normal_mapping .. [[
in vec3 F_coord;
in vec3 F_halfvector[LOS_LIGHT_MAX];
in vec3 F_lightvector[LOS_LIGHT_MAX];
in vec3 F_normal;
in vec3 F_tangent;
in vec2 F_texcoord;
void main()
{
	vec3 normal = los_normal_mapping(F_normal, F_tangent, F_texcoord, LOS_diffuse_texture_1);
	vec4 diffuse = texture(LOS_diffuse_texture_0, F_texcoord);
	vec4 specular = LOS_material_specular * texture(LOS_diffuse_texture_2, F_texcoord);
	]] .. Shader.los_lighting_default("F_coord", "normal", "F_lightvector", "F_halfvector", "specular") .. [[
	LOS_output_0 = LOS_material_diffuse * diffuse * lighting;
}]]}}

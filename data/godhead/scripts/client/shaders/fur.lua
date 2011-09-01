Shader{name = "fur",

-- Low quality program.
-- No lighting.
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
	LOS_output_0 = LOS_material_diffuse * diffuse;
}]]},

-- Medium quality program.
-- Vertex lighting.
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
out vec4 F_light;
out vec2 F_texcoord;
void main()
{
	]] .. Shader.los_animation_default() .. [[
	vec4 tmp = LOS_matrix_modelview * vec4(anim_coord,1.0);
	vec3 normal = LOS_matrix_normal * anim_normal;
	F_texcoord = LOS_texcoord;
	vec3 halfvector[LOS_LIGHT_MAX];
	vec3 lightvector[LOS_LIGHT_MAX];
	]] .. Shader.los_lighting_vectors("lightvector", "halfvector", "tmp.xyz") .. [[
	]] .. Shader.los_lighting_default("tmp.xyz", "normal", "lightvector", "halfvector") .. [[
	F_light = lighting;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass4_fragment = [[
in vec4 F_light;
in vec2 F_texcoord;
void main()
{
	vec4 diffuse = texture(LOS_diffuse_texture_0, F_texcoord);
	LOS_output_0 = LOS_material_diffuse * diffuse * F_light;
}]]},

-- High quality program.
-- Vertex lighting, geometry layers.
high = {
sort = true,
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
pass6_animated = true,
pass6_blend = true,
pass6_depth_func = "lequal",
pass6_depth_write = false,
pass6_vertex = [[
out vec3 G_coord;
out vec4 G_light;
out vec3 G_normal;
out vec3 G_tangent;
out vec2 G_texcoord;
void main()
{
	]] .. Shader.los_animation_default() .. [[
	vec4 tmp = LOS_matrix_modelview * vec4(anim_coord,1.0);
	G_coord = tmp.xyz;
	G_normal = LOS_matrix_normal * anim_normal;
	G_texcoord = LOS_texcoord;
	vec3 halfvector[LOS_LIGHT_MAX];
	vec3 lightvector[LOS_LIGHT_MAX];
	]] .. Shader.los_lighting_vectors("lightvector", "halfvector", "tmp.xyz") .. [[
	]] .. Shader.los_lighting_default("tmp.xyz", "G_normal", "lightvector", "halfvector") .. [[
	G_light = lighting;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass6_geometry = [[
layout(triangles) in;
layout(triangle_strip, max_vertices=30) out;
in vec3 G_coord[3];
in vec4 G_light[3];
in vec3 G_normal[3];
in vec3 G_tangent[3];
in vec2 G_texcoord[3];
out float F_layer;
out vec4 F_color;
out vec4 F_light;
out vec2 F_texcoord;
out vec2 F_texcoord2;
void main()
{
	int i;
	int j;
	for(j = 0 ; j < 10 ; j++)
	{
		F_layer = float(j) / 10.0;
		vec4 color = vec4(1.0, 1.0, 1.0, 1.0 - F_layer);
		for(i = 0 ; i < gl_VerticesIn ; i++)
		{
			F_color = color;
			F_light = G_light[i];
			F_texcoord = G_texcoord[i];
			F_texcoord2 = G_texcoord[i] + 0.1 * vec2(sin(F_layer), cos(F_layer));
			vec3 coord = G_coord[i] + G_normal[i] * F_layer * 0.03;
			gl_Position = LOS_matrix_projection * vec4(coord, 1.0);
			EmitVertex();
		}
		EndPrimitive();
	}
}]],
pass6_fragment = [[
in float F_layer;
in vec4 F_color;
in vec4 F_light;
in vec2 F_texcoord;
in vec2 F_texcoord2;
void main()
{
	vec4 diffuse = texture(LOS_diffuse_texture_0, F_texcoord);
	vec4 mask = texture(LOS_diffuse_texture_1, F_texcoord2);
	if(F_layer <= 0.0001)
		LOS_output_0 = LOS_material_diffuse * diffuse * F_light;
	else
		LOS_output_0 = LOS_material_diffuse * diffuse * F_color * mask * F_light;
}]]}}

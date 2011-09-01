Shader{name = "water",

-- High quality program.
-- Fragment lighting, procedural diffuse, alpha to coverage.
high = {
pass1_alpha_to_coverage = true,
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
	LOS_output_0 = vec4(0.0, 0.0, 0.0, 0.5);
}]],
pass4_alpha_to_coverage = true,
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
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	]] .. Shader.los_lighting_vectors("F_lightvector", "F_halfvector", "tmp.xyz") .. [[
	F_coord = LOS_coord.xyz;
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
float swirlynoise(in vec3 p)
{
	vec4 x = vec4(0.1,0.2,0.3,0.4);
	vec4 v1 = texture(LOS_diffuse_texture_0, 0.09*p.xy*vec2(1.0,1.5));
	vec4 v2 = texture(LOS_diffuse_texture_0, 0.11*p.xz*vec2(0.5,1.0));
	vec4 v3 = texture(LOS_diffuse_texture_0, 0.13*p.yz*vec2(1.5,0.5));
	vec4 c1 = texture(LOS_diffuse_texture_0, v1.ra + v2.gb);
	vec4 c2 = texture(LOS_diffuse_texture_0, v1.gb + v2.ra);
	return dot(vec4(c1.r,c1.g,c1.b,c1.a), x) *
	       dot(vec4(c2.r,c1.g,c1.b,c2.a), x) *
	       dot(vec4(c1.r,c2.g,c1.b,c1.a), x) *
	       dot(vec4(c2.r,c2.g,c1.b,c2.a), x) *
	       dot(vec4(c1.r,c1.g,c2.b,c1.a), x) *
	       dot(vec4(c2.r,c1.g,c2.b,c2.a), x) *
	       dot(vec4(c1.r,c2.g,c2.b,c1.a), x) *
	       dot(vec4(c2.r,c2.g,c2.b,c2.a), x) * 10.0;
}
void main()
{
	vec3 normal = normalize(F_normal);
	vec3 x = F_coord.xyz + vec3(0.0, 0.1*LOS_time, 0.0);
	vec3 y = F_coord.yzx + vec3(0.0, -0.3*LOS_time, 0.0);
	float a = swirlynoise(1.0 * x) + swirlynoise(2.0 * y);
	a = pow(2.7, a);
	x = F_coord.xyz + vec3(0.0, -0.3*LOS_time, -0.3*LOS_time);
	y = F_coord.yzx + vec3(0.0, -0.3*LOS_time, -0.3*LOS_time);
	float b = swirlynoise(2.5 * vec3(x.x, 0.8*x.y, x.z)) + swirlynoise(2.5 * vec3(x.x+0.05, 0.8*x.y, x.z+0.05));
	b = pow(4.0, b);
	vec4 diffuse = vec4(0.3*a,0.2+0.3*a,0.1+0.7*a,1.0) * vec4(0.8*b,0.8*b,0.1+b,1.0);
	]] .. Shader.los_lighting_default("F_coord", "normal", "F_lightvector", "F_halfvector") .. [[
	LOS_output_0 = LOS_material_diffuse * diffuse * lighting * vec4(1.0,1.0,1.0,0.5);
}]]}}

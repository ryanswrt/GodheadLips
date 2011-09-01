Shader{name = "highlight",

-- Medium quality program.
medium = {
pass4_blend = true,
pass4_blend_src = "src_alpha",
pass4_blend_dst = "one",
pass4_depth_func = "equal",
pass4_depth_write = false,
pass4_vertex = [[
out vec3 F_normal;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	F_normal = LOS_matrix_normal * LOS_normal;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass4_fragment = [[
in vec3 F_normal;
void main()
{
	float a = pow(1.0 - abs(normalize(F_normal).z), 1.5);
	LOS_output_0 = vec4(2.1 * a, 2.1 * a, 1.1 * a, 0.1 + 0.2 * a);
}]]}}


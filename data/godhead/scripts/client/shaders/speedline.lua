Shader{name = "speedline",

-- Medium quality program.
medium = {
sort = true,
pass6_blend = true,
pass6_blend_src = "src_alpha",
pass6_blend_dst = "one_minus_src_alpha",
pass6_depth_func = "lequal",
pass6_depth_write = false,
pass6_vertex = [[
out float F_alpha;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	F_alpha = 0.5*LOS_normal.x;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass6_fragment = [[
in float F_alpha;
void main()
{
	LOS_output_0 = vec4(LOS_material_diffuse.rgb, F_alpha);
}]]}}

Shader{name = "widget",

-- Medium quality program.
medium = {
pass1_blend = true,
pass1_blend_src = "src_alpha",
pass1_blend_dst = "one_minus_src_alpha",
pass1_depth_test = false,
pass1_depth_write = false,
pass1_vertex = [[
out vec2 F_texcoord;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord, 1.0);
	gl_Position = LOS_matrix_projection * tmp;
	F_texcoord = LOS_texcoord;
}]],
pass1_fragment = [[
in vec2 F_texcoord;
void main()
{
	LOS_output_0 = LOS_material_diffuse * texture(LOS_diffuse_texture_0, F_texcoord);
}]]}}

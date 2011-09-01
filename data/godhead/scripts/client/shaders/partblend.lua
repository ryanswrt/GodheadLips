Shader{name = "partblend",

-- Low quality program.
-- No particles at all.
low = {
pass4_vertex = [[
void main()
{
	gl_Position = vec4(0.0);
}]],
pass4_fragment = [[
void main()
{
	discard;
}]]},

-- Medium quality program.
-- Hardware particles using geometry shaders, billboard point lighting.
medium = {
sort = true,
pass6_blend = true,
pass6_blend_src = "src_alpha",
pass6_blend_dst = "one_minus_src_alpha",
pass6_depth_write = false,
pass6_vertex = [[
out geomvar
{
	vec4 color;
	vec2 size;
} OUT;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	vec3 normal = vec3(0.0,0.0,1.0);
	vec3 halfvector[LOS_LIGHT_MAX];
	vec3 lightvector[LOS_LIGHT_MAX];
	]] .. Shader.los_lighting_vectors("lightvector", "halfvector", "tmp.xyz") .. [[
	]] .. Shader.los_lighting_default("tmp.xyz", "normal", "lightvector", "halfvector") .. [[
	OUT.color = vec4(LOS_normal,LOS_texcoord.x) * lighting;
	vec4 ref1 = LOS_matrix_projection * tmp;
	vec4 ref2 = LOS_matrix_projection * (tmp - vec4(LOS_texcoord.yy, 0.0, 0.0));
	OUT.size = (ref2 - ref1).xy;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass6_geometry = [[
layout(triangles) in;
layout(triangle_strip, max_vertices=4) out;
in geomvar
{
	vec4 color;
	vec2 size;
} IN[3];
out fragvar
{
	vec4 color;
	vec2 texcoord;
} OUT;
void main()
{
	vec4 ctr = gl_PositionIn[0];
	vec3 size = vec3(IN[0].size.xy, 0.0);
	OUT.color = IN[0].color;
	OUT.texcoord = vec2(0.0, 0.0);
	gl_Position = ctr - size.xyzz;
	EmitVertex();
	OUT.texcoord = vec2(1.0, 0.0);
	gl_Position = ctr + size.xzzz - size.zyzz;
	EmitVertex();
	OUT.texcoord = vec2(0.0, 1.0);
	gl_Position = ctr - size.xzzz + size.zyzz;
	EmitVertex();
	OUT.texcoord = vec2(1.0, 1.0);
	gl_Position = ctr + size.xyzz;
	EmitVertex();
	EndPrimitive();
}]],
pass6_fragment = [[
in fragvar
{
	vec4 color;
	vec2 texcoord;
} IN;
void main()
{
	LOS_output_0 = IN.color * texture(LOS_diffuse_texture_0, IN.texcoord);
}]]}}

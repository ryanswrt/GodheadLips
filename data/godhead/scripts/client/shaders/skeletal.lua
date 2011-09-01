Shader{name = "skeletal",

-- Low quality program.
-- No tangent calculation.
low = {
transform_feedback = true,
pass1_depth_test = false,
pass1_depth_write = false,
pass1_vertex = [[
out vec3 LOS_output_coord;
out vec3 LOS_output_normal;
out vec3 LOS_output_tangent;
out vec2 LOS_output_texcoord;
vec3 los_quat_xform(in vec4 q, in vec3 v)
{
	vec4 a = vec4(
		 (q.w * v.x) + (q.y * v.z) - (q.z * v.y),
		 (q.w * v.y) - (q.x * v.z) + (q.z * v.x),
		 (q.w * v.z) + (q.x * v.y) - (q.y * v.x),
		-(q.x * v.x) - (q.y * v.y) - (q.z * v.z));
	return vec3(
		(a.w * -q.x) + (a.x *  q.w) + (a.y * -q.z) - (a.z * -q.y),
		(a.w * -q.y) - (a.x * -q.z) + (a.y *  q.w) + (a.z * -q.x),
		(a.w * -q.z) + (a.x * -q.y) - (a.y * -q.x) + (a.z *  q.w));
}
void main()
{
	int i;
	int bone[8] = int[](
		int(LOS_bones1.x), int(LOS_bones1.y), int(LOS_bones1.z), int(LOS_bones1.w),
		int(LOS_bones2.x), int(LOS_bones2.y), int(LOS_bones2.z), int(LOS_bones2.w));
	float weight[8] = float[](
		LOS_weights1.x, LOS_weights1.y, LOS_weights1.z, LOS_weights1.w,
		LOS_weights2.x, LOS_weights2.y, LOS_weights2.z, LOS_weights2.w);
	vec3 vtx = vec3(0.0);
	vec3 nml = vec3(0.0);
	for (i = 0 ; i < 8 ; i++)
	{
		int offset = 3 * bone[i];
		vec3 restpos = texelFetch(LOS_buffer_texture, offset).xyz;
		vec4 posepos = texelFetch(LOS_buffer_texture, offset + 1);
		vec4 poserot = texelFetch(LOS_buffer_texture, offset + 2);
		vtx += weight[i] * (los_quat_xform(poserot, (LOS_coord - restpos) * posepos.w) + posepos.xyz);
		nml += weight[i] * (los_quat_xform(poserot, LOS_normal));
	}
	LOS_output_coord = vtx;
	LOS_output_normal = normalize(nml) * length(LOS_normal);
	LOS_output_texcoord = LOS_texcoord;
	LOS_output_tangent = vec3(0.0);
	gl_Position = vec4(LOS_coord,1.0);
}]],
pass1_fragment = [[
void main()
{
}]]},

-- Medium quality program.
medium = {
transform_feedback = true,
pass1_depth_test = false,
pass1_depth_write = false,
pass1_vertex = [[
out vec3 G_coord;
out vec3 G_normal;
out vec2 G_texcoord;
vec3 los_quat_xform(in vec4 q, in vec3 v)
{
	vec4 a = vec4(
		 (q.w * v.x) + (q.y * v.z) - (q.z * v.y),
		 (q.w * v.y) - (q.x * v.z) + (q.z * v.x),
		 (q.w * v.z) + (q.x * v.y) - (q.y * v.x),
		-(q.x * v.x) - (q.y * v.y) - (q.z * v.z));
	return vec3(
		(a.w * -q.x) + (a.x *  q.w) + (a.y * -q.z) - (a.z * -q.y),
		(a.w * -q.y) - (a.x * -q.z) + (a.y *  q.w) + (a.z * -q.x),
		(a.w * -q.z) + (a.x * -q.y) - (a.y * -q.x) + (a.z *  q.w));
}
void main()
{
	int i;
	int bone[8] = int[](
		int(LOS_bones1.x), int(LOS_bones1.y), int(LOS_bones1.z), int(LOS_bones1.w),
		int(LOS_bones2.x), int(LOS_bones2.y), int(LOS_bones2.z), int(LOS_bones2.w));
	float weight[8] = float[](
		LOS_weights1.x, LOS_weights1.y, LOS_weights1.z, LOS_weights1.w,
		LOS_weights2.x, LOS_weights2.y, LOS_weights2.z, LOS_weights2.w);
	vec3 vtx = vec3(0.0);
	vec3 nml = vec3(0.0);
	for (i = 0 ; i < 8 ; i++)
	{
		int offset = 3 * bone[i];
		vec3 restpos = texelFetch(LOS_buffer_texture, offset).xyz;
		vec4 posepos = texelFetch(LOS_buffer_texture, offset + 1);
		vec4 poserot = texelFetch(LOS_buffer_texture, offset + 2);
		vtx += weight[i] * (los_quat_xform(poserot, (LOS_coord - restpos) * posepos.w) + posepos.xyz);
		nml += weight[i] * (los_quat_xform(poserot, LOS_normal));
	}
	G_coord = vtx;
	G_normal = normalize(nml) * length(LOS_normal);
	G_texcoord = LOS_texcoord;
	gl_Position = vec4(LOS_coord,1.0);
}]],
pass1_geometry = [[
layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;
in vec3 G_coord[3];
in vec3 G_normal[3];
in vec2 G_texcoord[3];
out vec3 LOS_output_coord;
out vec3 LOS_output_normal;
out vec3 LOS_output_tangent;
out vec2 LOS_output_texcoord;
void main()
{
	int i;
	vec3 ed0 = G_coord[1] - G_coord[0];
	vec3 ed1 = G_coord[2] - G_coord[0];
	vec2 uv0 = G_texcoord[1] - G_texcoord[0];
	vec2 uv1 = G_texcoord[2] - G_texcoord[0];
	float sign = uv0.x * uv1.y - uv1.x * uv0.y;
	LOS_output_tangent = normalize(sign * (ed1 * uv0.y - ed0 * uv1.y));
	for(i = 0 ; i < gl_VerticesIn ; i++)
	{
		LOS_output_coord = G_coord[i];
		LOS_output_texcoord = G_texcoord[i];
		LOS_output_normal = G_normal[i];
		gl_Position = gl_PositionIn[i];
		EmitVertex();
	}
	EndPrimitive();
}]],
pass1_fragment = [[
void main()
{
}]]}}

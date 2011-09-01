local instfunc = Shader.new
Shader.dict_name = {}

--- Creates a new shader and enables it.<br/>
-- The shader isn't compiled at all when it's created. It's done later in the
-- options screen when the used shader quality is known. Until then we only
-- store the settings for the different quality levels.
-- @param clss Shader class.
-- @param args Arguments.<ul>
--   <li>high: High quality shader.</li>
--   <li>low: Low quality shader.</li>
--   <li>medium: Medium quality shader.</li>
--   <li>name: Shader name.</li></ul>
-- @return New shader.
Shader.new = function(clss, args)
	local self = instfunc(clss, args)
	if self then
		for k,v in pairs(args) do self[k] = v end
		self:enable()
		return self
	end
end

--- Disables the shader.<br/>
-- The shader will be subject to normal garbage collection when disabled.
-- @param self Shader.
Shader.disable = function(self)
	Shader.dict_name[self.name] = nil
end

--- Enables the shader.<br/>
-- The shader will not be subject to garbage collection when enabled.
-- @param self Shader.
Shader.enable = function(self)
	Shader.dict_name[self.name] = self
end

--- Sets the quality level of the shader.<br/>
-- Older graphics cards can't deal with certain effects, such as geometry
-- amplification in the geometry shader, so multiple shader sets are used.
-- This function allows switching the shader quality be recompiling the
-- shader with different settings.
-- \param self Shader.
-- \param value Integer in the range of [1,3].
Shader.set_quality = function(self, value)
	local qualities = {self.low, self.medium, self.high}
	if self.quality ~= value then
		self.quality = value
		-- Try the requested level.
		local q = qualities[value]
		if q then return self:compile(q) end
		-- Fallback to lower setting.
		for i = value,1,-1 do
			local q = qualities[i]
			if q then return self:compile(q) end
		end
		-- Fallback to higher setting.
		for i = value,3 do
			local q = qualities[i]
			if q then return self:compile(q) end
		end
	end
end

--- Calculates skeletal animation for the vertex shader inputs.
Shader.los_animation_default = function()
	-- We'll leave it to the compiler to eliminate the unused outputs if
	-- the rest of the shader doesn't use them.
	return [[int anim_i;
	ivec4 anim_bones = 3 * LOS_bones1;
	vec4 anim_s1 = texelFetch(LOS_buffer_texture, anim_bones.x + 2);
	vec4 anim_s2 = texelFetch(LOS_buffer_texture, anim_bones.y + 2);
	vec4 anim_s3 = texelFetch(LOS_buffer_texture, anim_bones.z + 2);
	vec4 anim_s4 = texelFetch(LOS_buffer_texture, anim_bones.w + 2);
	vec3 anim_v =
		LOS_weights1.x * ((LOS_coord - anim_s1.xyz) * anim_s1.w + anim_s1.xyz) +
		LOS_weights1.y * ((LOS_coord - anim_s2.xyz) * anim_s2.w + anim_s2.xyz) +
		LOS_weights1.z * ((LOS_coord - anim_s3.xyz) * anim_s3.w + anim_s3.xyz) +
		LOS_weights1.w * ((LOS_coord - anim_s4.xyz) * anim_s4.w + anim_s4.xyz);
	mat2x4 anim_dq = mat2x4(
		LOS_weights1.x * texelFetch(LOS_buffer_texture, anim_bones.x) +
		LOS_weights1.y * texelFetch(LOS_buffer_texture, anim_bones.y) +
		LOS_weights1.z * texelFetch(LOS_buffer_texture, anim_bones.z) +
		LOS_weights1.w * texelFetch(LOS_buffer_texture, anim_bones.w),
		LOS_weights1.x * texelFetch(LOS_buffer_texture, anim_bones.x + 1) +
		LOS_weights1.y * texelFetch(LOS_buffer_texture, anim_bones.y + 1) +
		LOS_weights1.z * texelFetch(LOS_buffer_texture, anim_bones.z + 1) +
		LOS_weights1.w * texelFetch(LOS_buffer_texture, anim_bones.w + 1));
	anim_dq /= length(anim_dq[0]);
	vec3 anim_coord = anim_v + 2.0 * (
		cross(anim_dq[0].xyz, anim_v * anim_dq[0].w + cross(anim_dq[0].xyz, anim_v)) +
		cross(anim_dq[0].xyz, anim_dq[1].xyz) + anim_dq[1].xyz * anim_dq[0].w - anim_dq[0].xyz * anim_dq[1].w);
	vec3 anim_normal = LOS_normal + 2.0 * cross(anim_dq[0].xyz,
		cross(anim_dq[0].xyz, LOS_normal) + LOS_normal * anim_dq[0].w);
	vec3 anim_tangent = LOS_tangent + 2.0 * cross(anim_dq[0].xyz,
		cross(anim_dq[0].xyz, LOS_tangent) + LOS_tangent * anim_dq[0].w);]]
end

--- Calculates lighting and stores it to a variable named lighting.
-- @param co Variable name that contains the fragment coordinate.
-- @param no Variable name that contains the fragment normal.
-- @param lv Array name that contains the light vectors.
-- @param hv Array name that contains the light half vectors.
-- @param sp Variable name that contains additional specular color.
-- @param sh Variable name that contains shininess override value.
Shader.los_lighting_default = function(co, no, lv, hv, sp, sh)
	return string.format([[int lighting_index;
	vec4 lighting = vec4(0.0, 0.0, 0.0, 1.0);
	for(lighting_index = 0 ; lighting_index < LOS_LIGHT_MAX ; lighting_index++)
	{
		vec3 lv = %s[lighting_index];
		float fattn = 1.0 / dot(LOS_light[lighting_index].equation, vec3(1.0, length(lv), dot(lv, lv)));
		float fdiff = max(0.0, dot(%s, normalize(lv)));
		float coeff = max(0.0, dot(%s, normalize(%s[lighting_index])));
		float fspec = pow(max(0.0, coeff), %s);
		lighting.rgb += fattn * (LOS_light[lighting_index].ambient.rgb +
			fdiff * LOS_light[lighting_index].diffuse.rgb +
			fdiff * fspec * LOS_light[lighting_index].specular.rgb * %s.rgb);
	}]], lv, no, no, hv, sh or "LOS_material_shininess", sp or "LOS_material_specular")
end

--- Calculates lighting and stores it to a variable named lighting.
-- @param co Variable name that contains the fragment coordinate.
-- @param no Variable name that contains the fragment normal.
-- @param lv Array name that contains the light vectors.
-- @param hv Array name that contains the light half vectors.
-- @param sp Variable name that contains additional specular color.
-- @param sh Variable name that contains shininess override value.
Shader.los_lighting_skin = function(co, no, lv, hv, sp, sh)
	return string.format([[int lighting_index;
	vec4 lighting = vec4(0.0, 0.0, 0.0, 1.0);
	for(lighting_index = 0 ; lighting_index < LOS_LIGHT_MAX ; lighting_index++)
	{
		vec3 lv = %s[lighting_index];
		vec3 no = %s;
		vec3 co = normalize(%s);
		vec3 hv = %s[lighting_index];
		vec3 HV = normalize(hv);
		float sh = %s/128.0;
		float ndoth = dot(no, HV);
		float ndotl = dot(no, normalize(lv));
		/* Kelemen/Szirmay-Kalos specular. */
		float m = 0.5;
		float fexp = pow(1.0 - dot(co, HV), 5.0);
		float fres = fexp + 0.028 * (1.0 - fexp);
		vec4 beck = pow(2.0 * texture(LOS_diffuse_texture_3, vec2(ndoth, m)), vec4(10.0));
		vec4 spec = clamp(beck * fres / dot(hv, hv), vec4(0.0), vec4(1.0));
		float fspec = sh * ndotl * dot(spec, vec4(1.0, 0.625, 0.075, 0.005));
		/* Combine. */
		float fattn = 1.0 / dot(LOS_light[lighting_index].equation, vec3(1.0, length(lv), dot(lv, lv)));
		float fdiff = max(0.0, dot(no, normalize(lv)));
		lighting.rgb += fattn * (LOS_light[lighting_index].ambient.rgb +
			fdiff * LOS_light[lighting_index].diffuse.rgb +
			fspec * LOS_light[lighting_index].specular.rgb);
	}]], lv, no, co, hv, sh or "LOS_material_shininess")
end

--- Calculates lighting and stores it to a variable named lighting.
-- @param co Variable name that contains the fragment coordinate.
-- @param no Variable name that contains the fragment normal.
-- @param ta Variable name that contains the fragment tangent.
-- @param lv Array name that contains the light vectors.
-- @param hv Array name that contains the light half vectors.
Shader.los_lighting_hair = function(co, no, ta, lv, hv)
	return string.format([[int lighting_index;
	vec4 lighting = vec4(0.0, 0.0, 0.0, 1.0);
	for(lighting_index = 0 ; lighting_index < LOS_LIGHT_MAX ; lighting_index++)
	{
		vec3 lv = %s[lighting_index];
		float fattn = 1.0 / dot(LOS_light[lighting_index].equation, vec3(1.0, length(lv), dot(lv, lv)));
		float fdiff = max(0.0, 0.25 + 0.75 * dot(%s, normalize(lv)));
		float coeff1 = max(0.0, dot(%s, normalize(%s[lighting_index])));
		float tanref = max(0.0, dot(%s, reflect(-normalize(%s), %s)));
		float coeff2 = sqrt(1.0 - tanref * tanref);
		float coeff = mix(coeff1, coeff2, 0.6);
		float fspec = pow(coeff, LOS_material_shininess);
		lighting.rgb += fattn * (LOS_light[lighting_index].ambient.rgb +
			fdiff * LOS_light[lighting_index].diffuse.rgb +
			fdiff * fspec * LOS_light[lighting_index].specular.rgb * LOS_material_specular.rgb);
	}]], lv, no, no, hv, ta, co, no)
end

--- Calculates the light vectors and half vectors.
-- @param lv Array name where to return light vectors.
-- @param hv Array name where to return half vectors.
-- @oaran co Variable name that contains the transformed vertex coordinate.
Shader.los_lighting_vectors = function(lv, hv, co)
	return string.format([[int lighting_vindex;
	for(lighting_vindex = 0 ; lighting_vindex < LOS_LIGHT_MAX ; lighting_vindex++)
	{
		vec3 lighting_vector = LOS_light[lighting_vindex].position_premult - %s;
		%s[lighting_vindex] = lighting_vector;
		%s[lighting_vindex] = normalize(lighting_vector - %s);
	}]], co, lv, hv, co)
end

Shader.los_normal_mapping = [[
vec3 los_normal_mapping(in vec3 normal, in vec3 tangent, in vec2 texcoord, in sampler2D sampler)
{
	vec3 nml1 = normalize(normal);
	if(length(tangent) < 0.01) return nml1;
	vec3 tan1 = normalize(tangent);
	if(abs(dot(nml1, tan1)) > 0.9) return nml1;
	mat3 tangentspace = mat3(tan1, cross(tan1, nml1), nml1);
	vec3 n = tangentspace * (texture(sampler, texcoord).xyz * 2.0 - 1.0);
	if(length(n) < 0.01) return nml1;
	return normalize(n);
}]]

Shader.los_shadow_mapping = [[
float los_shadow_mapping(in vec4 lightcoord, in sampler2DShadow sampler)
{
	float esm_c = 80.0;
	float esm_d = lightcoord.z;
	float esm_z = lightcoord.w * textureProj(sampler, lightcoord);
	float esm_f = exp(-esm_c * esm_d + esm_c * esm_z);
	return clamp(esm_f, 0.0, 1.0);
}]]

Shader.los_shadow_mapping_pcf = [[
float los_shadow_mapping_pcf(in vec4 lightcoord, in sampler2DShadow sampler)
{
	vec2 pcf = vec2(clamp(0.004 * lightcoord.z, 0.02, 0.2), 0.0);
	float blend = los_shadow_mapping(lightcoord, sampler);
	blend += los_shadow_mapping(lightcoord - pcf.xyyy, sampler);
	blend += los_shadow_mapping(lightcoord + pcf.xyyy, sampler);
	blend += los_shadow_mapping(lightcoord - pcf.yxyy, sampler);
	blend += los_shadow_mapping(lightcoord + pcf.yxyy, sampler);
	blend *= 0.2;
	return blend;
}]]

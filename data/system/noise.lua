if not Los.program_load_extension("noise") then
	error("loading extension `noise' failed")
end

Noise = Class()
Noise.class_name = "Noise"

--- Generates Perlin noise.
-- @param self Noise class.
-- @param ... Arguments.<ul>
--   <li>1,point: Position vector.</li>
--   <li>2,scale: Coordinate scale factor.</li>
--   <li>3,frequency: Noise frequency.</li>
--   <li>4,octaves: Number of octaves.
--   <li>5,persistence: Noise persistence.</li></ul>
--   <li>6,seed: Noise seed.</li></ul>
-- @return Noise value.
Noise.perlin_noise = function(self, ...)
	local a,b,c,d,e,f,g = ...
	if a.class then
		return Los.noise_perlin_noise(a.handle, b and b.handle, c, d, e, f, g)
	else
		return Los.noise_perlin_noise(a.point.handle, a.scale and a.scale.handle,
			a.frequency, a.octaves, a.persistent, a.seed)
	end
end

--- Finds all noise coordinates whose values exceed a threshold and creates voxel terrain out of them.
-- @param self Noise class.
-- @param ... Arguments.<ul>
--   <li>1,min: Range start position vector.</li>
--   <li>2,max: Range end position vector.</li>
--   <li>3,tile: Tile type.</li>
--   <li>4,threshold: Threshold value.</li>
--   <li>5,scale: Coordinate scale factor.</li>
--   <li>6,frequency: Noise frequency.</li>
--   <li>7,octaves: Number of octaves.
--   <li>8,persistence: Noise persistence.</li>
--   <li>9,seed: Noise seed.</li></ul>
-- @return Table of vectors.
Noise.perlin_terrain = function(self, ...)
	local a,b,c,d,e,f,g,h,i = ...
	if a.class then
		return Los.noise_perlin_terrain(a.handle, b.handle, c, d, e and e.handle, f, g, h, i)
	else
		return Los.noise_perlin_terrain(a.min.handle, a.max.handle,
			a.tile, a.threshold, a.scale and a.scale.handle,
			a.frequency, a.octaves, a.persistent, a.seed)
	end
end

--- Finds all noise coordinates whose values exceed a threshold.
-- @param self Noise class.
-- @param ... Arguments.<ul>
--   <li>1,min: Range start position vector.</li>
--   <li>2,max: Range end position vector.</li>
--   <li>3,threshold: Threshold value.</li>
--   <li>4,scale: Coordinate scale factor.</li>
--   <li>5,frequency: Noise frequency.</li>
--   <li>6,octaves: Number of octaves.
--   <li>7,persistence: Noise persistence.</li>
--   <li>8,seed: Noise seed.</li></ul>
-- @return Table of vectors.
Noise.perlin_threshold = function(self, ...)
	local a,b,c,d,e,f,g,h = ...
	if a.class then
		return Los.noise_perlin_threshold(a.handle, b.handle, c, d and d.handle, f, g, h)
	else
		return Los.noise_perlin_threshold(a.min.handle, a.max.handle,
			a.threshold, a.scale and a.scale.handle,
			a.frequency, a.octaves, a.persistent, a.seed)
	end
end

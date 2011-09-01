Color = Class()

--- Converts an HSV color to RGB.
-- @param clss Color class.
-- @param hsv HSV color.
-- @return RGB color.
Color.hsv_to_rgb = function(clss, hsv)
	local h,s,v = hsv[1],hsv[2],hsv[3]
	local c = v * s
	local l = v - c
	local hh = h * 6
	local x = c * (1 - math.abs(math.mod(hh, 2) - 1));
	if 0 <= hh and hh < 1 then return {c + l, x + l, l} end
	if 1 <= hh and hh < 2 then return {x + l, c + l, l} end
	if 2 <= hh and hh < 3 then return {l, c + l, x + l} end
	if 3 <= hh and hh < 4 then return {l, x + l, c + l} end
	if 4 <= hh and hh < 5 then return {x + l, l, c + l} end
	return {c + l, l, x + l}
end

--- Converts an RGB color to HSV.
-- @param clss Color class.
-- @param rgb RGB color.
-- @return HSV color.
Color.rgb_to_hsv = function(clss, rgb)
	local eps = 0.00000001
	local r,g,b = rgb[1],rgb[2],rgb[3]
	local v = math.max(math.max(r, g), b)
	local m = math.min(math.min(r, g), b)
	local c = v - m
	if c < eps then h = 0
	elseif v == r then h = ((g - b) / c % 6) / 6
	elseif v == g then h = ((b - r) / c + 2) / 6
	elseif v == b then h = ((r - g) / c + 4) / 6 end
	if c < eps then s = 0
	else s = c / v end
	return {h, s, v}
end

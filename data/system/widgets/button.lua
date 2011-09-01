Widgets.Button = Class(Widget)
Widgets.Button.class_name = "Widgets.Button"

Widgets.Button.new = function(clss, args)
	local self = Widget.new(clss, args)
	if not self.font then self.font = "default" end
	if not self.style then self.style = "default" end
	if not self.text then self.text = "" end
	if self.enabled == nil then self.enabled = true end
	return self
end

Widgets.Button.pressed = function(self)
end

Widgets.Button.reshaped = function(self)
	-- Calculate the size.
	if self.style == "default" then
		self:set_request{
			font = self.font,
			internal = true,
			paddings = {3,6,6,3},
			text = self.text}
	else
		self:set_request{
			width = 16,
			height = 16}
	end
	local w = self.width
	local h = self.height
	-- Rebuild the canvas.
	local e = self.enabled
	local f = self.focused
	self:canvas_clear()
	if self.style == "default" then
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {w,h},
			source_image = "widgets1",
			source_position = self.focused and {350,415} or {350,375},
			source_tiling = {12,64,12,11,14,13}}
		self:canvas_text{
			dest_position = {0,0},
			dest_size = {w,h},
			text = self.text,
			text_alignment = {0.5,0.5},
			text_color = self.focused and {1,1,0,1} or {1,1,1,1},
			text_font = self.font}
	elseif self.style == "scroll-up" then
		local sx = self.enabled and 700 or 720
		local sy = self.focused and 100 or 130
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {21,21},
			source_image = "widgets1",
			source_position = {sx,sy},
			source_tiling = {0,21,0,0,21,0}}
	elseif self.style == "scroll-down" then
		local sx = self.enabled and 700 or 720
		local sy = self.focused and 160 or 190
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {21,21},
			source_image = "widgets1",
			source_position = {sx,sy},
			source_tiling = {0,21,0,0,21,0}}
	elseif self.style == "scroll-left" then
		local sx = self.enabled and 700 or 720
		local sy = self.focused and 100 or 130
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {21,21},
			rotation = 0.5 * math.pi,
			rotation_center = Vector(10,10),
			source_image = "widgets1",
			source_position = {sx,sy},
			source_tiling = {0,21,0,0,21,0}}
	elseif self.style == "scroll-right" then
		local sx = self.enabled and 700 or 720
		local sy = self.focused and 160 or 190
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {21,21},
			rotation = 0.5 * math.pi,
			rotation_center = Vector(10,10),
			source_image = "widgets1",
			source_position = {sx,sy},
			source_tiling = {0,21,0,0,21,0}}
	end
	self:canvas_compile()
end

Widgets.Button:add_getters{
	enabled = function(s) return rawget(s, "__enabled") end,
	focused = function(s) return rawget(s, "__focused") end,
	font = function(s) return rawget(s, "__font") end,
	style = function(s) return rawget(s, "__style") end,
	text = function(s) return rawget(s, "__text") end}

Widgets.Button:add_setters{
	enabled = function(s, v)
		if s.enabled == v then return end
		rawset(s, "__enabled", v)
		s:reshaped()
	end,
	focused = function(s, v)
		if s.focused == v then return end
		rawset(s, "__focused", v)
		s:reshaped()
	end,
	font = function(s, v)
		if s.font == v then return end
		rawset(s, "__font", v)
		s:reshaped()
	end,
	style = function(s, v)
		if s.style == v then return end
		rawset(s, "__style", v)
		s:reshaped()
	end,
	text = function(s, v)
		if s.text == v then return end
		rawset(s, "__text", v)
		s:reshaped()
	end}

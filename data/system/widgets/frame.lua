Widgets.Frame = Class(Widget)
Widgets.Frame.class_name = "Widgets.Frame"

--- Creates a new frame widget.
-- @param clss Frame widget class.
-- @param args Arguments.
-- @return Skills widget.
Widgets.Frame.new = function(clss, args)
	local self = Widget.new(clss, {spacings = {5,5}})
	self.style = "default"
	if args then
		for k,v in pairs(args) do self[k] = v end
		if args.text then
			self.cols = 1
			self.rows = 1
			self.label = Widgets.Label{font = "medium", text = args.text, valign = 0.5, halign = 0.5}
			self:append_row(self.label)
			self:set_expand{row = 1, col = 1}
		end
	end
	return self
end

Widgets.Frame.reshaped = function(self)
	local w = self.width
	local h = self.height
	self:canvas_clear()
	if type(self.style) == "table" then
		self:set_request{internal = true, width = self.style.width, height = self.style.height}
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {w,h},
			source_image = self.style.source_image or "widgets1",
			source_position = self.style.source_position or {0,0},
			source_tiling = self.style.source_tiling}
	elseif self.style == "default" then
		self:set_request{internal = true, width = 333, height = 150}
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {w,h},
			source_image = "widgets1",
			source_position = {0,0},
			source_tiling = {75,183,75,50,158,100}}
	elseif self.style == "equipment" then
		self:set_request{internal = true, width = 90, height = 143}
		self:canvas_image{
			dest_position = {20,0},
			dest_size = {90,165},
			source_image = "widgets1",
			source_position = {0,327},
			source_tiling = {0,300,0,0,185,0}}
	elseif self.style == "list" then
		self:set_request{internal = true, width = 64, height = 102}
	elseif self.style == "paper" then
		self:set_request{internal = true, width = 333, height = 220}
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {w,h},
			source_image = "widgets1",
			source_position = {0,0},
			source_tiling = {75,183,75,50,158,100}}
		self:canvas_image{
			dest_position = {10,5},
			dest_size = {w-16,h-17},
			source_image = "widgets1",
			source_position = {350,0},
			source_tiling = {50,217,50,100,163,100}}
	elseif self.style == "popup" then
		self:set_request{internal = true, width = 60, height = 40}
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {w,h},
			source_image = "widgets1",
			source_position = {350,455},
			source_tiling = {14,60,24,13,10,27}}
	elseif self.style == "quickbar" then
		self:set_request{internal = true, width = 200, height = 82}
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {w,h},
			source_image = "widgets1",
			source_position = {640,430},
			source_tiling = {256,70,58,0,82,0}}
	elseif self.style == "title" then
		self:set_request{internal = true, width = 333, height = 45}
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {w,h},
			source_image = "widgets1",
			source_position = {672,325},
			source_tiling = {100,73,100,0,45,0}}
	elseif self.style == "tooltip" then
		self:set_request{internal = true, width = 100, height = 20}
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {w,h},
			source_image = "widgets1",
			source_position = {720,0},
			source_tiling = {10,80,21,10,30,21}}
	end
	self:canvas_compile()
end

Widgets.Frame:add_getters{
	style = function(s) return rawget(s, "__style") end,
	focused = function(s) return rawget(s, "__focused") end,
	font = function(s) return rawget(s, "__font") end,
	text = function(s) return rawget(s, "__text") end}

Widgets.Frame:add_setters{
	style = function(s, v)
		if s.style == v then return end
		rawset(s, "__style", v)
		if type(v) == "table" then
			s.spacings = v.spacings or {0,0}
			s.margins = v.margins or {0,0,0,0}
		elseif v == "default" then
			s.margins = {10,7,10,20}
		elseif v == "equipment" then
			s.spacings = {0,0}
			s.margins = {10,7,0,0}
		elseif v == "list" then
			s.spacings = {0,0}
			s.margins = {0,0,0,0}
		elseif v == "paper" then
			s.margins = {30,30,40,30}
		elseif v == "popup" then
			s.margins = {10,17,5,17}
			s.spacings = {0,0}
		elseif v == "quickbar" then
			s.spacings = {0,16}
			s.margins = {77,0,10,0}
		elseif v == "title" then
			s.margins = {100,100,0,10}
		elseif v == "tooltip" then
			s.margins = {5,16,0,11}
		end
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
	text = function(s, v)
		if s.text == v then return end
		if s.label then s.label.text = v end
		rawset(s, "__text", v)
		s:reshaped()
	end}

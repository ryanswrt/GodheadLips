Widgets.Label = Class(Widget)
Widgets.Label.class_name = "Widgets.Label"

Widgets.Label.new = function(clss, args)
	local self = Widget.new(clss)
	local copy = function(f, d) self[f] = (args and args[f] ~= nil) and args[f] or d end
	copy("request")
	copy("width_request")
	copy("color")
	copy("font", "default")
	copy("halign", 0)
	copy("valign", 0.5)
	copy("text", "")
	if args then
		for k,v in pairs(args) do self[k] = v end
	end
	return self
end

Widgets.Label.reshaped = function(self)
	local wrap = self.width_request
	self:set_request{
		font = self.font,
		internal = true,
		paddings = {2,2,2,2},
		text = self.text,
		width = wrap and (wrap - 4)}
	local w = self.width
	local h = self.height
	local f = self.focused
	local p = self.pressed
	self:canvas_clear()
	self:canvas_text{
		dest_position = {0,0},
		dest_size = {w,h},
		text = self.text,
		text_alignment = {self.halign,self.valign},
		text_color = self.color or (f and p and {0.6,0.6,0,1} or p and {0.6,0.6,0.6,0.6} or {1,1,1,1}),
		text_font = self.font}
	self:canvas_compile()
end

Widgets.Label:add_getters{
	color = function(s) return rawget(s, "__color") end,
	focused = function(s) return rawget(s, "__focused") end,
	font = function(s) return rawget(s, "__font") end,
	text = function(s) return rawget(s, "__text") end}

Widgets.Label:add_setters{
	color = function(s, v)
		if s.color == v then return end
		rawset(s, "__color", v)
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
		rawset(s, "__text", v)
		s:reshaped()
	end}

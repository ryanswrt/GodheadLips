Widgets.Text = Class(Widgets.Frame)
Widgets.Text.class_name = "Widgets.Text"

Widgets.Text.new = function(clss, args)
	local arg = function(n, d) return args and args[n] or d end
	local self = Widgets.Frame.new(clss, {cols = 1, rows = 0, margins = {30,30,40,30}, style = "paper"})
	self.color = arg("color", {0,0,0,1})
	self.font = arg("font", "default")
	self.halign = arg("halign", 0)
	self.valign = arg("valign", 0.5)
	self.text = arg("text", {})
	self:build()
	return self
end

Widgets.Text.build = function(self)
	self.rows = 0
	if self.valign > 0 then
		self:append_row()
		self:set_expand{row = 1}
	end
	for k,v in ipairs(self.text) do
		local w = Widgets.Label{
			color = self.color,
			font = v[2] or self.font,
			text = v[1],
			valign = self.valign,
			width_request = 270}
		self:append_row(w)
	end
	if self.valign < 1 then
		self:append_row()
		self:set_expand{row = self.rows}
	end
end

Widgets.Text:add_getters{
	color = function(s) return rawget(s, "__color") end,
	font = function(s) return rawget(s, "__font") end,
	text = function(s) return rawget(s, "__text") end}

Widgets.Text:add_setters{
	color = function(s, v)
		if s.color == v then return end
		rawset(s, "__color", v)
		s:reshaped()
	end,
	font = function(s, v)
		if s.font == v then return end
		rawset(s, "__font", v)
		s:reshaped()
	end,
	text = function(s, v)
		if type(v) == "string" then
			rawset(s, "__text", {{v}})
		else
			rawset(s, "__text", v)
		end
	end}

Widgets.Binding = Class(Widget)

Widgets.Binding.new = function(clss, args)
	local self = Widget.new(clss, args)
	self:reshaped()
	return self
end

Widgets.Binding.pressed = function(self)
end

Widgets.Binding.reshaped = function(self)
	self:set_request{
		internal = true,
		height = 24,
		width = 400}
	local a = 1
	local f = self.focused
	local w = self.width
	local h = self.height
	self:canvas_clear()
	-- Name.
	if self.action then
		self:canvas_text{
			dest_position = {10,0},
			dest_size = {w-10,h},
			text = self.name,
			text_alignment = {0,0},
			text_color = f and {1,1,0,1} or {a,a,a,1},
			text_font = "bigger"}
	end
	-- Key 1.
	local key1 = self.action.key1
	self:canvas_text{
		dest_position = {150,0},
		dest_size = {w-150,h},
		text = Keycode[key1] or (key1 and tostring(key1) or "----"),
		text_alignment = {0,0},
		text_color = f and {1,1,0,1} or {a,a,a,1},
		text_font = "bigger"}
	-- Key 2.
	if self.action.mode == "analog" then
		local key2 = self.action.key2
		self:canvas_text{
			dest_position = {250,0},
			dest_size = {w-250,h},
			text = Keycode[key2] or (key2 and tostring(key2) or "----"),
			text_alignment = {0,0},
			text_color = f and {1,1,0,1} or {a,a,a,1},
			text_font = "bigger"}
	end
	self:canvas_compile()
end

Widgets.Binding:add_getters{
	action = function(s) return rawget(s, "__action") end,
	focused = function(s) return rawget(s, "__focused") end,
	name = function(s)
		local tmp = string.gsub(s.action.name, "_", " ")
		return string.gsub(tmp, "(.)(.*)", function(a,b) return string.upper(a) .. b end)
	end}

Widgets.Binding:add_setters{
	action = function(s, v)
		if s.active == v then return end
		rawset(s, "__action", v)
		s:reshaped()
	end,
	focused = function(s, v)
		if s.focused == v then return end
		rawset(s, "__focused", v)
		s:reshaped()
	end}

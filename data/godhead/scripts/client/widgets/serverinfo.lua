Widgets.Serverinfo = Class(Widget)

Widgets.Serverinfo.new = function(clss, args)
	local self = Widget.new(clss, args)
	self:reshaped()
	return self
end

Widgets.Serverinfo.pressed = function(self)
end

Widgets.Serverinfo.reshaped = function(self)
	self:set_request{
		internal = true,
		height = 36,
		width = 100}
	local a = 1
	local f = self.focused
	local w = self.width
	local h = self.height
	self:canvas_clear()
	-- Background.
	local sx = 350
	local sy = self.active and 415 or 375
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {w,h},
		source_image = "widgets1",
		source_position = {sx,sy},
		source_tiling = {12,64,12,11,14,13}}
	-- Name.
	if self.name then
		self:canvas_text{
			dest_position = {10,0},
			dest_size = {w-10,h},
			text = self.name,
			text_alignment = {0,self.compact and 0.5 or 0},
			text_color = f and {1,1,0,1} or {a,a,a,1},
			text_font = "bigger"}
	end
	-- Description.
	if self.desc and not self.compact then
		self:canvas_text{
			dest_position = {10,18},
			dest_size = {w-10,h},
			text = self.desc,
			text_alignment = {0,0},
			text_color = f and {1,1,0,1} or {a,a,a,1},
			text_font = "default"}
	end
	-- Players
	if self.players and not self.compact then
		self:canvas_text{
			dest_position = {0,3},
			dest_size = {w-5,h},
			text = tostring(self.players),
			text_alignment = {1,0},
			text_color = f and {1,1,0,1} or {a,a,a,1},
			text_font = "default"}
	end
	self:canvas_compile()
end

Widgets.Serverinfo:add_getters{
	active = function(s) return rawget(s, "__active") end,
	compact = function(s) return rawget(s, "__compact") end,
	effect = function(s) return rawget(s, "__effect") end,
	focused = function(s) return rawget(s, "__focused") end}

Widgets.Serverinfo:add_setters{
	active = function(s, v)
		if s.active == v then return end
		rawset(s, "__active", v)
		s:reshaped()
	end,
	compact = function(s, v)
		if s.compact == v then return end
		rawset(s, "__compact", v)
		s:reshaped()
	end,
	effect = function(s, v)
		if s.effect == v then return end
		s.icon = v and Iconspec:find{name = v.icon}
		rawset(s, "__effect", v)
		s:reshaped()
	end,
	focused = function(s, v)
		if s.focused == v then return end
		rawset(s, "__focused", v)
		s:reshaped()
	end}

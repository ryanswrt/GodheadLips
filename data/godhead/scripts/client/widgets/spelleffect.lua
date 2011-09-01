Widgets.Spelleffect = Class(Widget)

Widgets.Spelleffect.new = function(clss, args)
	local self = Widget.new(clss, args)
	self:reshaped()
	return self
end

Widgets.Spelleffect.pressed = function(self)
end

Widgets.Spelleffect.reshaped = function(self)
	self:set_request{
		internal = true,
		height = 36,
		width = 100}
	local a = self.active and 1 or 0.5
	local f = self.active and self.focused
	local w = self.width
	local h = self.height
	self:canvas_clear()
	-- Icon.
	local icon = self.icon
	if icon then
		self:canvas_image{
			dest_position = {0,(h-icon.size[2])/2},
			dest_size = {icon.size[1],icon.size[2]},
			color = {1,1,1,a},
			source_image = icon.image,
			source_position = icon.offset,
			source_tiling = {0,icon.size[1],0,0,icon.size[2],0}}
	end
	-- Name.
	if self.effect then
		local cap = string.gsub(self.effect.name, "(.)(.*)", function(a,b) return string.upper(a) .. b end)
		self:canvas_text{
			dest_position = {40,0},
			dest_size = {w-40,h},
			text = cap,
			text_alignment = {0,self.compact and 0.5 or 0},
			text_color = f and {1,1,0,1} or {a,a,a,1},
			text_font = "bigger"}
	end
	-- Description.
	if self.effect and not self.compact then
		self:canvas_text{
			dest_position = {40,18},
			dest_size = {w-40,h},
			text = self.effect.description,
			text_alignment = {0,0},
			text_color = f and {1,1,0,1} or {a,a,a,1},
			text_font = "default"}
	end
	self:canvas_compile()
end

Widgets.Spelleffect:add_getters{
	active = function(s) return rawget(s, "__active") end,
	compact = function(s) return rawget(s, "__compact") end,
	effect = function(s) return rawget(s, "__effect") end,
	focused = function(s) return rawget(s, "__focused") end}

Widgets.Spelleffect:add_setters{
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

Widgets.SkillControl = Class(Widget)

Widgets.SkillControl.new = function(clss, args)
	local self = Widget.new(clss, {cols = 3, rows = 2, margins = {left=100-15}})
	-- Buttons.
	self.button_minus = Widgets.Button{style = "scroll-left", pressed = function(w)
		if w.enabled then self:changed(math.max(0, self.cap - 1)) end
	end}
	self.button_plus = Widgets.Button{style = "scroll-right", pressed = function(w)
		if w.enabled then self:changed(math.min(self.max, self.cap + 1)) end
	end}
	self:set_expand{col = 2, row = 1}
	self:set_child(1, 1, self.button_minus)
	self:set_child(3, 1, self.button_plus)
	-- Tooltip.
	if args.name then
		self.tooltip = Widgets.Skilltooltip{desc = args.desc, name = args.name}
	end
	-- Configuration.
	for k,v in pairs(args) do self[k] = v end
	self.init = true
	self:reshaped()
	return self
end

Widgets.SkillControl.changed = function(self, value)
end

Widgets.SkillControl.get_value_at = function(self, point)
	local p = point - Vector(self.x, self.y)
	if not self.compact then
		p = p - Vector(100, 0)
		p.x = p.x / 2
	end
	if p.x >= 0 and p.x <= self.max then
		return math.ceil(p.x)
	end
end

Widgets.SkillControl.pressed = function(self)
	local v = self:get_value_at(Program.cursor_position)
	if not v then return end
	self:changed(v)
end

Widgets.SkillControl.reshaped = function(self)
	if not self.init then return end
	local xlbl = 2
	local xbar = self.compact and 0 or 100
	self:set_request{
		font = "tiny",
		internal = true,
		width = xbar + (self.compact and 100 or 208),
		height = 16}
	local w = self.compact and self.max or 2 * self.max
	local h = self.height
	local c = self.cap / self.max
	local v = self.value / self.max
	-- Background.
	self:canvas_clear()
	self:canvas_image{
		dest_position = {xbar,0},
		dest_size = {w,h},
		source_image = "widgets1",
		source_position = {450,375},
		source_tiling = {34,120,34,6,15,7}}
	-- Value.
	if v > 0 then
		self:canvas_image{
			dest_clip = {xbar,0,v*w,h},
			dest_position = {xbar,0},
			dest_size = {w,h},
			source_image = "widgets1",
			source_position = {450,405},
			source_tiling = {34,120,34,6,15,7}}
	end
	-- Cap.
	if c > v then
		self:canvas_image{
			dest_clip = {xbar+v*w,0,c*w-v*w,h},
			dest_position = {xbar,0},
			dest_size = {w,h},
			source_image = "widgets1",
			source_position = {450,465},
			source_tiling = {34,120,34,6,15,7}}
	end
	-- Name string.
	if not self.compact and self.text ~= "" then
		self:canvas_text{
			dest_position = {xlbl,0},
			dest_size = {xbar,h},
			text = self.text .. ":",
			text_alignment = {0,0.5},
			text_color = {1,1,1,1},
			text_font = "default"}
	end
	-- Value string.
	self:canvas_text{
		dest_position = {xbar,0},
		dest_size = {w,h},
		text = tostring(self.value),
		text_alignment = {0.5,0.5},
		text_color = {1,1,1,1},
		text_font = "tiny"}
	self:canvas_compile()
end

Widgets.SkillControl:add_getters{
	allowance = function(s) return rawget(s, "__allowance") or 100 end,
	cap = function(s) return rawget(s, "__cap") or 0 end,
	compact = function(s) return rawget(s, "__compact") end,
	icon = function(s) return rawget(s, "__icon") end,
	max = function(s) return rawget(s, "__max") or 100 end,
	text = function(s) return rawget(s, "__text") or "" end,
	value = function(s) return rawget(s, "__value") or 0 end}

Widgets.SkillControl:add_setters{
	allowance = function(s, v)
		if s.allowance == v then return end
		rawset(s, "__allowance", v)
		s.button_plus.enabled = (s.cap < s.max and s.allowance > 0)
		s:reshaped()
	end,
	cap = function(s, v)
		if s.cap == v then return end
		rawset(s, "__cap", v)
		s.button_minus.enabled = (s.cap > 0)
		s.button_plus.enabled = (s.cap < s.max and s.allowance > 0)
		s:reshaped()
	end,
	compact = function(s, v)
		if s.compact == v then return end
		rawset(s, "__compact", v)
		s.button_minus.visible = not v
		s.button_plus.visible = not v
		s:reshaped()
	end,
	icon = function(s, v)
		if s.icon == v then return end
		rawset(s, "__icon", v)
		s:reshaped()
	end,
	max = function(s, v)
		if s.max == v then return end
		rawset(s, "__max", v)
		s.button_plus.enabled = (s.cap < s.max and s.allowance > 0)
		s:reshaped()
	end,
	text = function(s, v)
		if s.text == v then return end
		rawset(s, "__text", v)
		s:reshaped()
	end,
	value = function(s, v)
		if s.value == v then return end
		rawset(s, "__value", v)
		s:reshaped()
	end}

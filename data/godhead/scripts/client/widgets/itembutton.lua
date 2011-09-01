Widgets.ItemButton = Class(Widget)

Widgets.ItemButton.new = function(clss, args)
	local self = Widget.new(clss)
	self.init = true
	for k,v in pairs(args) do self[k] = v end
	if self.enabled == nil then self.enabled = true end
	if self.font == nil then self.font = "tiny" end
	if self.icon == nil then self.icon = "" end
	if self.text == nil then self.text = "" end
	self.init = nil
	return self
end

Widgets.ItemButton.pressed = function(self, args)
end

Widgets.ItemButton.reshaped = function(self)
	if self.init then return end
	self:set_request{
		internal = true,
		height = 34,
		width = 34}
	local w = self.width
	local h = self.height
	local a = (self.drag or not self.enabled) and 0.2 or 1
	-- Background.
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {w,h},
		source_image = "widgets1",
		source_position = self.focused and {708,380} or {650,380},
		source_tiling = {12,29,12,12,8,12}}
	-- Icon.
	local icon = Iconspec:find{name = self.icon}
	if icon then
		self:canvas_image{
			dest_position = {(w-icon.size[1])/2,(h-icon.size[2])/2},
			dest_size = {icon.size[1],icon.size[2]},
			color = {1,1,1,a},
			source_image = icon.image,
			source_position = icon.offset,
			source_tiling = {0,icon.size[1],0,0,icon.size[2],0}}
	end
	-- Count.
	if self.count and self.count > 1 then
		self:canvas_text{
			dest_position = {4,0},
			dest_size = {w-4,h-2},
			text = tostring(self.count),
			text_alignment = {0,1},
			text_color = self.drag and {1,1,1,a} or self.focused and {1,1,0,a} or {1,1,1,a},
			text_font = self.font}
	end
	self:canvas_compile()
end

Widgets.ItemButton.update_tooltip = function(self, args)
	local spec = Itemspec:find{name = self.text}
	if not spec then
		self.tooltip = nil
	else
		self.tooltip = Widgets.Itemtooltip{count = self.count, spec = spec}
	end
end

Widgets.ItemButton:add_getters{
	count = function(self) return rawget(self, "__count") end,
	drag = function(self) return rawget(self, "__drag") end,
	enabled = function(self) return rawget(self, "__enabled") end,
	focused = function(self) return rawget(self, "__focused") end,
	font = function(self) return rawget(self, "__font") end,
	icon = function(self) return rawget(self, "__icon") end,
	text = function(self) return rawget(self, "__text") end}

Widgets.ItemButton:add_setters{
	count = function(self, value)
		if self.count == value then return end
		rawset(self, "__count", value)
		self:reshaped()
		self:update_tooltip()
	end,
	drag = function(self, value)
		if self.drag == value then return end
		rawset(self, "__drag", value)
		self:reshaped()
	end,
	enabled = function(self, value)
		if self.enabled == value then return end
		rawset(self, "__enabled", value)
		self:reshaped()
	end,
	focused = function(self, value)
		if self.focused == value then return end
		rawset(self, "__focused", value)
		self:reshaped()
	end,
	font = function(self, value)
		if self.font == value then return end
		rawset(self, "__font", value)
		self:reshaped()
	end,
	icon = function(self, value)
		if self.icon == value then return end
		rawset(self, "__icon", value)
		self:update_tooltip()
		self:reshaped()
	end,
	text = function(self, value)
		if self.text == value then return end
		rawset(self, "__text", value)
		self:update_tooltip()
	end}

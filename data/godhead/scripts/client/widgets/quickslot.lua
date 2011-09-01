Widgets.Quickslot = Class(Widget)

Widgets.Quickslot.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.margins = {3,2,5,4}
	return self
end

Widgets.Quickslot.reshaped = function(self)
	local icon = self.icon or self.DEFAULT
	self:set_request{
		internal = true,
		width = 34+5,
		height = 34}
	local w = self.width
	local h = self.height + 9
	-- Background.
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,-9},
		dest_size = {w, h},
		source_image = "widgets1",
		source_position = {810,379},
		source_tiling = {7,25,7,8,28,7}}
	-- Icon.
	if self.icon then
		self:canvas_image{
			dest_position = {(w-icon.size[1])/2+1,(h-icon.size[2])/2-9},
			dest_size = icon.size,
			source_image = icon.image,
			source_position = icon.offset,
			source_tiling = {0,icon.size[1],0,0,icon.size[1],0}}
	end
	-- Number.
	if self.index then
		self:canvas_text{
			dest_position = {6,-3},
			dest_size = {28,28},
			text = string.format("%d.", self.index),
			text_alignment = {0,0},
			text_color = {1,1,1,0.5},
			text_font = "tiny"}
	end
	-- Text.
	if self.text then
		self:canvas_text{
			dest_position = {6,0},
			dest_size = {28,28},
			text = self.text,
			text_alignment = {0,1},
			text_color = {1,1,1,1},
			text_font = "tiny"}
	end
	-- Glass.
	if not self.focused then
		self:canvas_image{
			dest_position = {0,-9},
			dest_size = {w, h},
			source_image = "widgets1",
			source_position = {860,379},
			source_tiling = {7,25,7,8,28,7}}
	end
	self:canvas_compile()
end

Widgets.Quickslot.pressed = function(self, args)
end

Widgets.Quickslot:add_getters{
	focused = function(self) return rawget(self, "__focused") end,
	icon = function(self) return rawget(self, "__icon") end,
	text = function(self) return rawget(self, "__text") end}

Widgets.Quickslot:add_setters{
	focused = function(self, value)
		if self.focused == value then return end
		rawset(self, "__focused", value)
		self:reshaped()
	end,
	icon = function(self, value)
		if self.icon == value then return end
		rawset(self, "__icon", value)
		self:reshaped()
	end,
	text = function(self, value)
		if self.text == value then return end
		rawset(self, "__text", value)
		self:reshaped()
	end}

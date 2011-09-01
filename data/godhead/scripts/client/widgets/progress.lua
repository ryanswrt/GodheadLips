Widgets.Progress = Class(Widget)

Widgets.Progress.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.min = self.min or 0
	self.max = self.max or 100
	self.text = self.text or ""
	self.value = self.value or 0
	self.init = true
	return self
end

Widgets.Progress.get_value_at = function(self, point)
	local p = point - Vector(self.x, self.y)
	local v = p.x / self.width * (self.max - self.min) + self.min
	return v
end

Widgets.Progress.reshaped = function(self)
	if not self.init then return end
	self:set_request{
		font = "tiny",
		internal = true,
		paddings = {0,5,5,0},
		text = self.text ~= "" and self.text or "\n"}
	local w = self.width
	local h = self.height
	local v = (self.value - self.min) / (self.max - self.min)
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {w,h},
		source_image = "widgets1",
		source_position = {450,375},
		source_tiling = {34,120,34,6,15,7}}
	if v > 0 then
		self:canvas_image{
			dest_clip = {0,0,v*w,h},
			dest_position = {0,0},
			dest_size = {w,h},
			source_image = "widgets1",
			source_position = {450,405},
			source_tiling = {34,120,34,6,15,7}}
	end
	self:canvas_text{
		dest_position = {0,0},
		dest_size = {w,h},
		text = self.text,
		text_alignment = {0.5,0.5},
		text_color = {1,1,1,1},
		text_font = "tiny"}
	self:canvas_compile()
end

Widgets.Progress:add_getters{
	max = function(self) return rawget(self, "__max") end,
	min = function(self) return rawget(self, "__min") end,
	text = function(self) return rawget(self, "__text") end,
	value = function(self) return rawget(self, "__value") end}

Widgets.Progress:add_setters{
	max = function(self, value)
		if self.max == value then return end
		rawset(self, "__max", value)
		self:reshaped()
	end,
	min = function(self, value)
		if self.min == value then return end
		rawset(self, "__min", value)
		self:reshaped()
	end,
	text = function(self, value)
		if self.text == value then return end
		rawset(self, "__text", value)
		self:reshaped()
	end,
	value = function(self, value)
		if self.value == value then return end
		rawset(self, "__value", value)
		self:reshaped()
	end}

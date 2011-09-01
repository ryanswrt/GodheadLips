Widgets.ColorSelector = Class(Widget)

Widgets.ColorSelector.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.red = self.red or 1
	self.green = self.green or 1
	self.blue = self.blue or 1
	self.init = true
	return self
end

Widgets.ColorSelector.get_value_at = function(self, point)
	local p = point - Vector(self.x, self.y)
	local v = p.x / self.width
	if p.y < self.height / 3 then return "red", v end
	if p.y < self.height * 2 / 3 then return "green", v end
	return "blue", v
end

Widgets.ColorSelector.pressed = function(self)
	local c,v = self:get_value_at(Program.cursor_position)
	self[c] = v
	return c,v
end

Widgets.ColorSelector.reshaped = function(self)
	if not self.init then return end
	self:set_request{width = 128, height = 40}
	local w = self.width
	local h = self.height
	local values = {self.red, self.green, self.blue}
	self:canvas_clear()
	for i=1,3 do
		local v = values[i]
		self:canvas_image{
			dest_position = {0,(i-1)*h/3},
			dest_size = {w,h/3},
			source_image = "widgets1",
			source_position = {450,376},
			source_tiling = {34,120,34,5,15,5}}
		if v > 0 then
			self:canvas_image{
				dest_clip = {0,0,v*w,h},
				dest_position = {0,(i-1)*h/3},
				dest_size = {w,h/3},
				source_image = "widgets1",
				source_position = {450,436},
				source_tiling = {34,120,34,5,15,5}}
		end
	end
	self:canvas_compile()
end

Widgets.ColorSelector:add_getters{
	blue = function(self) return rawget(self, "__blue") end,
	green = function(self) return rawget(self, "__green") end,
	red = function(self) return rawget(self, "__red") end}

Widgets.ColorSelector:add_setters{
	blue = function(self, value)
		if self.blue == value then return end
		rawset(self, "__blue", value)
		self:reshaped()
	end,
	green = function(self, value)
		if self.green == value then return end
		rawset(self, "__green", value)
		self:reshaped()
	end,
	red = function(self, value)
		if self.red == value then return end
		rawset(self, "__red", value)
		self:reshaped()
	end}

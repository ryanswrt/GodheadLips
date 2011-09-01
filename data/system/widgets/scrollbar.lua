Widgets.Scrollbar = Class(Widget)

Widgets.Scrollbar.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.max = 0
	self.page = 1
	self.scroll_offset = 1
	return self
end

Widgets.Scrollbar.changed = function(self, value)
end

Widgets.Scrollbar.get_value_at = function(self, point)
	if self.max == 0 then return end
	local p = point - Vector(self.x, self.y)
	local v = p.y / self.height * self.max
	return math.floor(v + 0.5)
end

Widgets.Scrollbar.pressed = function(self)
	if self.max <= self.page then return end
	local cursor = Program.cursor_position
	local c = cursor - Vector(self.x, self.y)
	if c.y < 21 then
		-- Scroll up.
		if self.scroll_offset >= 1 then
			self.scroll_offset = self.scroll_offset - 1
			self:reshaped()
			self:changed(self.scroll_offset)
		end
	elseif c.y >= self.height - 21 then
		-- Scroll down.
		if self.scroll_offset < self.max - self.page then
			self.scroll_offset = self.scroll_offset + 1
			self:reshaped()
			self:changed(self.scroll_offset)
		end
	else
		-- Scroll to cursor.
		local v = self:get_value_at(cursor)
		if not v then return end
		self.scroll_offset = math.max(0, math.min(v - math.floor(self.page/2), self.max - self.page))
		self:reshaped()
		self:changed(self.scroll_offset)
	end
end

Widgets.Scrollbar.reshaped = function(self)
	self:set_request{
		internal = true,
		width = 21,
		height = 64}
	local w = self.width
	local h = self.height
	-- Draw the background.
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {w,h},
		source_image = "widgets1",
		source_position = {670,0},
		source_tiling = {0,21,0,32,226,32}}
	-- Draw the slider.
	local sy,sh
	local alloc = h - 42
	if self.max > 0 then
		sy = alloc * self.scroll_offset / self.max
		sh = math.min(alloc, math.max(20, alloc * self.page / self.max))
	else
		sy = 0
		sh = alloc
	end
	self:canvas_image{
		dest_position = {0,21 + sy},
		dest_size = {w,sh},
		source_image = "widgets1",
		source_position = {700,0},
		source_tiling = {0,21,0,10,70,10}}
	-- Draw the buttons.
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {21,21},
		source_image = "widgets1",
		source_position = (self.scroll_offset > 0) and {700,100} or {700,130},
		source_tiling = {0,21,0,0,21,0}}
	self:canvas_image{
		dest_position = {0,h-22},
		dest_size = {21,21},
		source_image = "widgets1",
		source_position = (self.scroll_offset < self.max - self.page) and {700,160} or {700,190},
		source_tiling = {0,21,0,0,21,0}}
	self:canvas_compile()
end

Widgets.Scrollbar.scrolled = function(self, args)
	if args.button == 4 then
		-- Scroll up.
		if self.scroll_offset >= 1 then
			self.scroll_offset = self.scroll_offset - 1
			self:reshaped()
			self:changed(self.scroll_offset)
		end
	else
		-- Scroll down.
		if self.scroll_offset < self.max - self.page then
			self.scroll_offset = self.scroll_offset + 1
			self:reshaped()
			self:changed(self.scroll_offset)
		end
	end
	return true
end

Widgets.Scrollbar.set_range = function(self, max, scroll_offset, page)
	if scroll_offset then self.scroll_offset = scroll_offset end
	if page then self.page = page end
	self.max = max
	self.scroll_offset = math.max(0, math.min(self.scroll_offset, self.max - self.page))
	self:reshaped()
end

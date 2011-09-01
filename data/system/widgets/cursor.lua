Widgets.Cursor = Class(Widget)
Widgets.Cursor.class_name = "Widgets.Cursor"

Widgets.Cursor.new = function(clss, cursor)
	local self = Widget.new(clss, {cursor = cursor, cols = 1, rows = 1})
	return self
end

Widgets.Cursor.update = function(self)
	-- Update position.
	local c = Program.cursor_position
	self.floating = not Program.cursor_grabbed
	self.x = c.x + 1
	self.y = c.y + 1
	-- Show tooltips.
	if self.tooltip then
		self.tooltip.floating = false
		self.tooltip = nil
	end
	if self.widget then return end
	if self.floating then
		local w = Widgets:find_handler_widget("tooltip")
		if w and w.tooltip then
			self.tooltip = w.tooltip
			w.tooltip:popup(c + Vector(5,5))
		end
	end
end

Widgets.Cursor.reshaped = function(self)
	local icon = self.icon
	local cursor = self.cursor
	self:set_request{
		internal = true,
		height = cursor and cursor.size[2] or 16,
		width = cursor and cursor.size[1] or 16}
	self:canvas_clear()
	if self.cursor then
		self:canvas_image{
			dest_position = {0,0},
			dest_size = cursor.size,
			source_image = cursor.image,
			source_position = cursor.offset,
			source_tiling = {0,cursor.size[1],0,0,cursor.size[1],0}}
	end
	self:canvas_compile()
end

Widgets.Cursor:add_getters{
	cursor = function(s) return rawget(s, "__cursor") end,
	widget = function(s) return rawget(s, "__widget") end}

Widgets.Cursor:add_setters{
	cursor = function(s, v)
		if s.cursor == v then return end
		rawset(s, "__cursor", v)
		s:reshaped()
	end,
	widget = function(s, v)
		if s.widget == v then return end
		rawset(s, "__widget", v)
		s:set_child(1, 1, v)
	end}

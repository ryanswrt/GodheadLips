Widgets.Title = Class(Widget)
Widgets.Title.class_name = "Widgets.Title"

--- Creates a new title widget.
-- @param clss Title widget class.
-- @param args Arguments.
-- @return Title widget.
Widgets.Title.new = function(clss, args)
	local request = 30
	local self = Widget.new(clss, {cols = 5, rows = 1, margins = {5,5,0,10}, spacings = {2,0}})
	self.button_help = Widgets.Button{text = "?", pressed = function() self:help() end}
	self.button_help:set_request{width = request}
	self.widget_pad = Widget()
	self.widget_pad:set_request{width = request}
	self.label_text = Widgets.Label{font = "medium", valign = 0.5, halign = 0.5}
	self.button_back = Widgets.Button{text = "<", pressed = function() self:back() end}
	self.button_back:set_request{width = request}
	self.button_close = Widgets.Button{text = "X", pressed = function() self:close() end}
	self.button_close:set_request{width = request}
	self:set_expand{col = 3, row = 1}
	self:set_child(1, 1, self.button_back)
	self:set_child(2, 1, self.button_help)
	self:set_child(3, 1, self.label_text)
	self:set_child(4, 1, self.widget_pad)
	self:set_child(5, 1, self.button_close)
	for k,v in pairs(args) do self[k] = v end
	return self
end

Widgets.Title.back = function(self)
end

Widgets.Title.close = function(self)
end

Widgets.Title.help = function(self)
end

Widgets.Title.reshaped = function(self)
	self:canvas_clear()
	self:set_request{internal = true, width = 333, height = 45}
	local w = self.width
	local h = self.height
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {w,h},
		source_image = "widgets1",
		source_position = {672,325},
		source_tiling = {100,73,100,0,45,0}}
	self:canvas_compile()
end

Widgets.Title:add_getters{
	text = function(self) return rawget(self, "__text") end}

Widgets.Title:add_setters{
	text = function(self, v)
		if self.text == v then return end
		rawset(self, "__text", v)
		self.label_text.text = v
		self:reshaped()
	end}

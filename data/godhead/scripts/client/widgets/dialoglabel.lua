Widgets.DialogLabel = Class(Widgets.Label)

Widgets.DialogLabel.new = function(clss, args)
	local self = Widgets.Label.new(clss, args)
	self.font = "bigger"
	self:set_request{width = 500}
	return self
end

Widgets.DialogLabel.reshaped = function(self)
	-- Format the text.
	local color
	local text
	if self.index and self.text then
		local action = Action.dict_name[string.format("choice_%d", self.index)]
		local key = action and action.key1 and Keycode[action.key1] or "--"
		if self.choice then
			color = self.focused and {0.7,0.3,0.3,1} or {0,0.7,0,1}
			text = string.format("<%s> %s", key, self.text)
		else
			color = self.focused and {0.7,0.3,0.3,1} or {1,1,1,1}
			text = string.format("<%s> %s", key, self.text)
		end
	elseif self.text then
		color = self.focused and {0.7,0.3,0.3,1} or {1,1,1,1}
		text = string.format("%s:", self.text)
	end
	-- Calculate the size.
	local wrap = self:get_request() or 500
	self:set_request{
		font = self.font,
		internal = true,
		paddings = {2,2,2,2},
		text = text,
		width = wrap - 4}
	local w = self.width
	local h = self.height
	-- Populate the canvas.
	self:canvas_clear()
	if self.text then
		self:canvas_image{
			dest_position = {0,4},
			dest_size = {w,h},
			source_image = "widgets1",
			source_position = {722,63},
			source_tiling = {6,32,6,6,24,6}}
		self:canvas_text{
			dest_position = {0,0},
			dest_size = {w,h},
			text = text,
			text_alignment = {self.halign,self.valign},
			text_color = color,
			text_font = self.font}
	end
	self:canvas_compile()
end

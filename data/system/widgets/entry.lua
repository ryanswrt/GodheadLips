require "system/widgets/label"

Widgets.Entry = Class(Widgets.Label)
Widgets.Entry.class_name = "Widgets.Entry"

--- Creates a new text entry widget.
-- @param clss Entry class.
-- @param args Arguments.
-- @return Entry widget.
Widgets.Entry.new = function(clss, args)
	local self = Widgets.Label.new(clss, args)
	self.background = args and args.background
	self.background_focus = args and args.background_focus
	if not self.background then
		self.background = {
			dest_position = {0,0},
			source_image = "widgets1",
			source_position = {910,383},
			source_tiling = {15,25,15,10,14,10}}
		self.background_focus = {
			dest_position = {0,0},
			source_image = "widgets1",
			source_position = {967,383},
			source_tiling = {15,25,15,10,14,10}}
	end
	self.init = true
	return self
end

--- Clears the text of the entry.
-- @param self Entry widget.
Widgets.Entry.clear = function(self)
	self.text = ""
end

--- Handles input.
-- @param self Entry widget.
-- @param args Arguments.
-- @return True if absorbed.
Widgets.Entry.event = function(self, args)
	if args.type == "keypress" then
		if args.code == 8 then
			-- Backspace.
			if self.text and #self.text then
				self.text = string.sub(self.text, 1, math.max(0, #self.text - 1))
			end
		elseif args.code == 13 then
			-- Enter.
			if self.pressed then
				self:pressed()
			end
		elseif args.text then
			-- Typing.
			self.text = (self.text or "") .. args.text
		end
		return true
	end
end

--- Rebuilds the entry widget.
-- @param self Entry widget.
Widgets.Entry.reshaped = function(self)
	-- Format the text.
	-- In password mode, all characters are replaced with '*'.
	local text
	if self.password and self.text then
		text = ""
		for i=1,#self.text do text = text .. "*" end
	else
		text = self.text or ""
	end
	-- Calculate the size request.
	self:set_request{
		font = self.font,
		internal = true,
		paddings = {2,2,2,2},
		text = text .. "\n"}
	local w = self.width
	local h = self.height
	-- Pack the background.
	self:canvas_clear()
	local style = self.focus and self.background_focus or self.background
	if style then
		self:canvas_image{
			dest_position = style.dest_position,
			dest_size = {w,h},
			source_image = style.source_image,
			source_position = style.source_position,
			source_tiling = style.source_tiling}
	end
	-- Pack the text.
	self:canvas_text{
		dest_position = {2,0},
		dest_size = {w,h},
		text = text .. (self.focused and "|" or ""),
		text_alignment = {0,0.5},
		text_color = self.color or self.focused and {1,1,1,1} or {0.6,0.6,0.6,1},
		text_font = self.font}
	self:canvas_compile()
end

Widgets.Entry:add_getters{
	password = function(s) return rawget(s, "__password") end}

Widgets.Entry:add_setters{
	password = function(s, v)
		if s.password == v then return end
		rawset(s, "__password", v and true or nil)
		s:reshaped()
	end}

require "system/widgets/button"
require "system/widgets/frame"
require "system/widgets/menuitem"

Widgets.ComboBox = Class(Widgets.Button)
Widgets.ComboBox.class_name = "Widgets.ComboBox"

--- Creates a new combo box.
-- @param clss Combo box class.
-- @param args Arguments.
-- @return Combo box.
Widgets.ComboBox.new = function(clss, args)
	local self = Widgets.Button.new(clss)
	self.value = 1
	self.menu = Widgets.Frame{cols = 1, expand_col = 1, style = "popup", temporary = true}
	-- Copy arguments.
	for k,v in pairs(args or {}) do
		if type(k) ~= "number" then
			self[k] = v
		end
	end
	-- Create menu items.
	self:set_items(args or {})
	return self
end

--- Activates a row of the combo box menu.
-- @param self Combo box.
-- @param args Arguments.<ul>
--   <li>index: Row index.</li>
--   <li>press: False to not call the menu activation function.</li>
--   <li>text: Row text.</li></ul>
-- @return True on success.
Widgets.ComboBox.activate = function(self, args)
	-- Find the item.
	local widget = nil
	local index = nil
	if args.index then
		widget = self.menu:get_child(1, args.index)
		index = args.index
	elseif args.text then
		index = 1
		for index = 1,self.menu.rows do
			local tmp = self.menu:get_child(1, index)
			if not tmp then return end
			if tmp.text == args.text then
				widget = tmp
				break
			end
		end
	end
	-- Set the selection.
	if not widget then return end
	self.value = index
	if args.press ~= false then
		widget:pressed()
	else
		self.text = widget.text
	end
	-- Allow a user callback.
	self:activated()
	return true
end

--- Called when a row of the combo box menu is activated.
-- @param self Combo box.
Widgets.ComboBox.activated = function(self)
end

--- Appends a new item to the combo box.
-- @param self Combo box.
-- @param args Arguments.<ul>
--   <li>pressed: Selection handling function.</li>
--   <li>text: Text of the item.</li></ul>
Widgets.ComboBox.append = function(self, args)
	local row = self.menu.rows + 1
	local widget = Widgets.MenuItem{text = args.text, pressed = function()
		self.text = args.text
		self.value = row
		self.menu.visible = false
		if args.pressed then args.pressed(self, row) end
		self:activated()
	end}
	self.menu:append_row(widget)
end

--- Clears the choice list of the combo box.
-- @param self Combo box.
Widgets.ComboBox.clear = function(self)
	self.value = 1
	self.menu.rows = 0
	self.text = nil
	self:close()
end

--- Makes sure that the popup of the combo box is hidden.
-- @param self Combo box.
Widgets.ComboBox.close = function(self)
	if Widgets.ComboBox.active == self then
		Widgets.ComboBox.active = nil
		self.menu.visible = false
	end
end

--- Shows the popup of the combo box.
-- @param self Combo box.
Widgets.ComboBox.popup = function(self)
	if Widgets.ComboBox.active then
		Widgets.ComboBox.active:close()
	end
	self.menu:set_request{width = self.width + 9}
	self.menu:popup{x = self.x, y = self.y, width = self.width, height = self.height, dir = "down"}
	Widgets.ComboBox.active = self
end

--- Handles mouse clicks to the combo box.
-- @param self Combo box.
-- @param args Event arguments.
Widgets.ComboBox.pressed = function(self, args)
	if self.menu.rows == 0 then return end
	local p = Program.cursor_position
	if p.x - self.x < 15 then
		-- Previous item.
		if self.value > 1 then
			self:activate{index = self.value - 1}
		else
			self:activate{index = self.menu.rows} 
		end
	elseif p.x - self.x > self.width - 16 then
		-- Next item.
		if self.value < self.menu.rows then
			self:activate{index = self.value + 1}
		else
			self:activate{index = 1} 
		end
	else
		-- Popup menu.
		self:popup()
	end
end

--- Handles scroll wheel input of the combo box.
-- @param self Combo box.
-- @param args Event arguments.
Widgets.ComboBox.scrolled = function(self, args)
	if self.menu.rows == 0 then return end
	if args.button == 5 then
		-- Previous item.
		if self.value > 1 then
			self:activate{index = self.value - 1}
		else
			self:activate{index = self.menu.rows} 
		end
	else
		-- Next item.
		if self.value < self.menu.rows then
			self:activate{index = self.value + 1}
		else
			self:activate{index = 1} 
		end
	end
end

--- Sets the choice list of the combo box.
-- @param self Combo box.
-- @param items List of choices.
Widgets.ComboBox.set_items = function(self, items)
	self:clear()
	for k,v in ipairs(items) do
		if type(v) == "table" then
			self:append{text = v[1], pressed = v[2]}
		elseif type(v) == "string" then
			self:append{text = v}
		end
	end
end

--- Rebuilds the combo box.
-- @param self Combo box.
Widgets.ComboBox.reshaped = function(self)
	self:set_request{
		font = self.font,
		internal = true,
		paddings = {2,12,12,2},
		text = (self.text or "") .. " "}
	local w = self.width
	local h = self.height
	-- Background.
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {w,h},
		source_image = "widgets1",
		source_position = self.focused and {350,415} or {350,375},
		source_tiling = {12,64,12,11,14,13}}
	-- Arrows.
	self:canvas_image{
		dest_position = {5,(h-14)/2},
		dest_size = {7,14},
		source_image = "widgets1",
		source_position = {651,417},
		source_tiling = {0,7,0,0,14,0}}
	self:canvas_image{
		dest_position = {w-11,(h-14)/2},
		dest_size = {7,14},
		source_image = "widgets1",
		source_position = {667,417},
		source_tiling = {0,7,0,0,14,0}}
	-- Text.
	self:canvas_text{
		dest_position = {15,0},
		dest_size = {w,h},
		text = self.text,
		text_alignment = {0,0.5},
		text_color = self.focused and {1,1,0,1} or {1,1,1,1},
		text_font = self.font}
	self:canvas_compile()
end

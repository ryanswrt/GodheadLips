Views.Inventory = Class(Widget)

--- Creates a new inventory view.
-- @param clss Inventory view class.
-- @return Inventory view.
Views.Inventory.new = function(clss)
	local self = Widget.new(clss, {cols = 3, rows = 1, spacings = {0, 0}})
	-- Frame.
	self.title = Widgets.Title{text = "Inventory",
		back = function() self:back() end,
		close = function() Client:set_mode("game") end,
		help = function() Client.views.help:show("inventory") end}
	self.group = Widget{cols = 1, rows = 4, spacings = {0, 0}}
	self.group:set_child(1, 3, self.title)
	self.group:set_expand{col = 1, row = 1}
	-- Trading widget.
	self.trading = Widgets.Trading()
	self.trading.visible = false
	self.group:set_child(1, 2, self.trading)
	-- Container list.
	self.containers = Widget{rows = 3, spacings = {0, 0}}
	self.containers:set_expand{row = 1}
	-- Weight widget.
	-- This is injected to the owned inventory widget.
	self.group_weight = Widget{rows = 1, cols = 4, spacings = {0,0}, margins={5,0,0,0}}
	self.label_weight1 = Widgets.Label{text = "Weight:"}
	self.label_weight2 = Widgets.Label()
	self.label_weight3 = Widgets.Label{text = "/"}
	self.label_weight4 = Widgets.Label()
	self.group_weight:set_child(1, 1, self.label_weight1)
	self.group_weight:set_child(2, 1, self.label_weight2)
	self.group_weight:set_child(3, 1, self.label_weight3)
	self.group_weight:set_child(4, 1, self.label_weight4)
	-- Packing.
	self:set_request{width = 100}
	self:set_child(1, 1, self.group)
	self:set_child(2, 1, self.containers)
	return self
end

--- Adds a container to the inventory.
-- @param self Inventory view.
-- @param widget Container widget.
-- @param type Container type number.
-- @param own True if this is the main inventory.
Views.Inventory.add_container = function(self, widget, own)
	if own then
		widget.button_close.visible = false
		self.container = widget
		self.group:set_child{col = 1, row = 4, widget = widget}
		widget:set_extra_widget(self.group_weight)
	else
		local text = widget.crafting and "Workbench" or "Loot"
		widget.button_close.visible = true
		widget.closed = function(w) self:remove_container(w) end
		self.containers:append_col(Widget(), Widgets.Frame{style = "title", text = text}, widget)
		Client:set_mode("inventory")
	end
	Gui.need_rebuild = true
end

--- Removes a container from the inventory view.
-- @param self Inventory view.
-- @param widget Container widget.
Views.Inventory.remove_container = function(self, widget)
	-- Remove main inventory.
	if self.container == widget then
		self.container = nil
		self:set_child{col = 1, row = 2}
	end
	-- Remove looted containers.
	for i = 1,self.containers.cols do
		local w = self.containers:get_child(i, 3)
		if w == widget then
			self.containers:remove{col = i}
			break
		end
	end
	Gui.need_rebuild = true
end

Views.Inventory.back = function(self)
	Client:set_mode("menu")
end

--- Closes the inventory view.
-- @param self Inventory view.
Views.Inventory.close = function(self)
	-- Close containers.
	for i = self.containers.cols,1,-1 do
		local w = self.containers:get_child(i, 3)
		w:close()
	end
	-- Close trading.
	if self.trading.visible then
		self:set_trading()
		self.trading:close()
	end
end

--- Enters the inventory view.
-- @param self Inventory view.
-- @param from Name of the previous mode.
Views.Inventory.enter = function(self, from, level)
	Quickslots.mode = "items"
	Gui.menus:open{level = level, widget = self}
end

--- Gets the contents of an inventory or an equipment slot.
-- @param self Inventory view.
-- @param args Arguments.<ul>
--   <li>id: Container ID.</li></ul>
--   <li>slot: Slot name.</li></ul>
-- @return Item widget or nil.
Views.Inventory.get_item = function(self, args)
	local cont = Widgets.Container:find(args.id or self.id)
	if not cont then return end
	return cont:get_item(args)
end

--- Sets the contents of an equipment slot.
-- @param self Inventory view.
-- @param slot Slot name.
-- @param name Item name.
-- @param count Item count.
Views.Inventory.set_item = function(self, slot, name, count)
	local widget = self.equipment.dict_name[slot]
	if widget then
		local spec = Itemspec:find{name = name}
		widget.text = name or ""
		widget.icon = spec and spec.icon
		widget.count = count or 1
	end
end

--- Enables or disables trading.
-- @param self Inventory view.
-- @param value True to enable.
Views.Inventory.set_trading = function(self, value)
	self.trading.visible = value or false
	self.trading:clear()
end

--- Sets the displayed weight and burdening limit.
-- @param self Inventory view.
-- @param weight Weight in kilograms.
-- @param limit Burdening limit in kilograms.
Views.Inventory.set_weight = function(self, weight, limit)
	self.label_weight2.text = tostring(weight)
	self.label_weight4.text = tostring(limit)
	if weight <= limit then
		self.label_weight2.color = {1,1,1,1}
	else
		self.label_weight2.color = {1,0,0,1}
	end
end

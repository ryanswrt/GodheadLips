require "client/widgets/trading"

Widgets.Container = Class(Widget)
Widgets.Container.dict_id = {}

--- Finds a container widget by ID.
-- @param clss Container widget class.
-- @param id Container ID.
-- @return Container widget or nil.
Widgets.Container.find = function(clss, id)
	return clss.dict_id[id]
end

--- Creates a new container widget.
-- @param clss Container widget class.
-- @param args Arguments.<ul>
--   <li>closed: Close handler.</li>
--   <li>id: Container ID.</li>
--   <li>size: Number of slots in the container.</li>
--   <li>spec: Object spec of the owner.</li></ul>
Widgets.Container.new = function(clss, args)
	local self = Widget.new(clss, {cols = 1, spacings = {0, 0}})
	self.id = args.id
	self:set_request{width = 200}
	clss.dict_id[args.id] = self
	-- Item list.
	self.item_list = Widgets.ItemList{size = args.size, activated = function(w, r, a) self:activated(a, r) end}
	self.group = Widgets.Frame{cols = 3, rows = 3}
	self.group:set_expand{col = 1, row = 1}
	self.group:set_child{col = 1, row = 1, widget = self.item_list}
	self:append_row(self.group)
	self:set_expand{col = 1, row = 1}
	-- Equipment list.
	local slots = nil
	if args.spec and args.spec.equipment_slots then
		for k,v in pairs(args.spec.equipment_slots) do
			slots = true
			break
		end
	end
	if slots then
		self.equipment = Widgets.Equipment{spec = spec, pressed = function(widget, args, slot)
			self:activated(args, slot) end}
		self.group:set_child{col = 2, row = 1, widget = self.equipment}
	end
	-- Crafting list.
	self.show_uncraftable = true
	if args.spec and args.spec.inventory_type == "workbench" then
		self.button_hide_uncraftable = Widgets.Toggle{text = "Hide Uncraftable", pressed = function(w) w.active=not w.active self.show_uncraftable=w.active self:update() end}
		self.group:set_child{col = 1, row = 3, widget = self.button_hide_uncraftable}
		-- Crafting actions.
		local pressed = function(w)
			if not w.enabled then return end
			Network:send{packet = Packet(packets.CRAFTING, "uint32", w.id, "string", w.text)}
		end
		local scrolled = function(w, args)
			self.crafting:scrolled(args)
		end
		-- Build the crafting item buttons.
		local craftable = Crafting:get_craftable()
		table.sort(craftable)
		self.craftable = {}
		for k,v in ipairs(craftable) do
			local spec = Itemspec:find{name = v}
			--if Crafting:can_craft({spec=spec}) then print("can") end
			local widget = Widgets.ItemButton{enabled = false, id = self.id,
				index = k, icon = spec and spec.icon, spec = spec, text = v,
				pressed = pressed, scrolled = scrolled, visible= self.show_uncraftable}
			table.insert(self.craftable, widget)
		end
		-- Pack the crafting list.
		self.crafting = Widgets.List()
		local col = 1
		local row = Widget{cols = 8, rows = 1, spacings = {0,0}}
		for k,v in ipairs(self.craftable) do
			row:set_child(col, 1, v)
			if col == row.cols then
				self.crafting:append{widget = row}
				row = Widget{cols = row.cols, rows = 1, spacings = {0,0}}
				col = 1
			else
				col = col + 1
			end
		end
		if row.cols > 0 then
			self.crafting:append{widget = row}
		end
		-- Add the list to the container widget.
		self.group:set_child(1, 2, self.crafting)
	end
	-- Close button.
	self.closed = args.closed
	self.button_close = Widgets.Button{text = "Close", pressed = function() self:close() end}
	self.group:set_child{col = 2, row = 3, widget = self.button_close}
	return self
end

--- Called when a slot was pressed.
-- @param self Container widget.
-- @param args Event arguments.
-- @param slot Slot pressed.
Widgets.Container.activated = function(self, args, slot)
	if Target.active then
		Target:select_container(self.id, slot)
	elseif args and args.button == 1 then
		Drag:clicked_container(self.id, slot)
	elseif args and args.button == 3 then
		Commands:use(self.id, slot)
	end
end

--- Closes the container.
-- @param self Container widget.
-- @param silent True to not send a network event.
Widgets.Container.close = function(self, silent)
	if not silent then
		Network:send{packet = Packet(packets.INVENTORY_CLOSED, "uint32", self.id)}
	end
	self.dict_id[self.id] = nil
	self:closed()
end

--- Called when the container was closed.
-- @param self Container widget.
Widgets.Container.closed = function(self)
end

--- Gets an item widget in a slot.
-- @param self Container widget.
-- @param args Arguments.<ul>
--   <li>slot: Slot name or number.</li></ul>
Widgets.Container.get_item = function(self, args)
	if type(args.slot) == "number" then
		local widget = self.item_list.buttons[args.slot]
		if not widget then return end
		if not widget.text or #widget.text == 0 then return end
		return widget
	else
		if not self.equipment then return end
		local widget = self.equipment.dict_name[args.slot]
		if not widget then return end
		if not widget.text or #widget.text == 0 then return end
		return widget
	end
end

--- Finds an item by name and returns its slot.
-- @param self Container widget.
-- @param args Arguments.<ul>
--   <li>name: Item name.</li></ul>
-- @return Slot or nil.
Widgets.Container.find_item = function(self, args)
	-- Search from equipment slots.
	if self.equipment then
		for slot,item in pairs(self.equipment.dict_name) do
			if item.text == args.name then return slot end
		end
	end
	-- Search from inventory slots.
	for i = 1,self.item_list.size do
		local item = self.item_list:get_item{slot = i}
		if item and item.text == args.name then return i end
	end
end

--- Adds an extra child widget.
-- @param self Container widget.
-- @param widget Widget.
Widgets.Container.set_extra_widget = function(self, widget)
	self.group:set_child{col = 1, row = 3, widget = widget}
end

--- Sets the item of a slot.
-- @param self Container widget.
-- @param args Arguments.<ul>
--   <li>count: Item count.</li>
--   <li>icon: Item icon.</li>
--   <li>name: Item name.</li>
--   <li>slot: Slot name or number.</li></ul>
Widgets.Container.set_item = function(self, args)
	if type(args.slot) == "number" then
		local widget = self.item_list.buttons[args.slot]
		if not widget then return end
		widget.text = args.name
		widget.count = args.count
		widget.icon = args.icon
	else
		if not self.equipment then return end
		local widget = self.equipment.dict_name[args.slot]
		if not widget then return end
		widget.text = args.name
		widget.count = args.count
		widget.icon = args.icon
	end
	self:update()
end

--- Updates the crafting list.
-- @param self Container widget.
Widgets.Container.update = function(self)
	if not self.crafting then return end
	-- Get stored items.
	local items = {}
	for i = 1,self.item_list.size do
		local item = self.item_list:get_item{slot = i}
		if item then
			items[item.text] = item.count
		end
	end
	-- Update craftability flags.
	local args = {
		get_item = function(name) return items[name] end,
		get_skill = function(name) return Client.views.skills.skills:get_value(name) end}

	for k,v in pairs(self.craftable) do
		args.spec = v.spec
		v.enabled = Crafting:can_craft(args)
		if self.show_uncraftable then v.visible = true else v.visible = false v.visible = Crafting:can_craft(args) end 
	end
end

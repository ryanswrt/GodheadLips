Drag = Class()

--- Cancels the active drag and restores the UI to the original state.
-- @param self Drag.
Drag.cancel = function(self)
	self:clear()
end

--- Changes the count of dragged items, effectively allowing stack splitting.
-- @param self Drag.
-- @param delta Size of change.
Drag.change_count = function(self, delta)
	-- Make sure that the dragged item exists.
	local item = Client.views.inventory:get_item{id = self.drag[2], slot = self.drag[3]}
	if not item then return end
	-- Update the drag count.
	local new = self.drag[5] + delta
	new = math.max(new, 1)
	new = math.min(new, item.count)
	if new == self.drag[5] then return end
	self.drag[5] = new
	-- Update the cursor.
	local spec = Itemspec:find{name = item.text}
	Widgets.Cursor.inst.widget = Widgets.Itemtooltip{
		count = item.count, count_drag = self.drag[5], spec = spec}
end

--- Clears the drag state after finishing a drag.
-- @param self Drag.
Drag.clear = function(self)
	-- Restore items slots back to normal.
	if not self.drag then return end
	if self.drag[1] == "equ" or self.drag[1] == "inv" then
		local item = Client.views.inventory:get_item{id = self.drag[2], slot = self.drag[3]}
		if item then item.drag = nil end
	end
	-- Reset the cursor.
	Widgets.Cursor.inst.widget = nil
	-- Clear the drag.
	self.drag = nil
end

--- Called when a container is clicked with the purpose of starting or stopping a drag.
-- @param self Drag.
-- @param inv Inventory number of the container.
-- @param slot Slot number of the countainer.
Drag.clicked_container = function(self, inv, slot)
	-- Handle existing drags.
	-- If there's an item drag in progress, the item is dropped to the
	-- slot. Otherwise, the incompatible drag is cancelled.
	if self.drag then
		if self.drag[1] == "equ" or self.drag[1] == "inv" then
			if type(slot) == "number" then
				Equipment:move(self.drag[1], self.drag[2], self.drag[3], "inv", inv, slot, self.drag[5])
			else
				Equipment:move(self.drag[1], self.drag[2], self.drag[3], "equ", inv, slot, self.drag[5])
			end
			self:clear()
		else
			self:cancel()
		end
		return
	end
	-- Make sure that the dragged item exists.
	local item = Client.views.inventory:get_item{id = inv, slot = slot}
	if not item then return end
	-- Update the cursor.
	if type(slot) == "number" then
		self.drag = {"inv", inv, slot, item.text, item.count}
	else
		self.drag = {"equ", inv, slot, item.text, item.count}
	end
	local spec = Itemspec:find{name = item.text}
	Widgets.Cursor.inst.widget = Widgets.Itemtooltip{
		count = item.count, count_drag = item.count, spec = spec}
	-- Hide the dragged item.
	item.drag = true
end

--- Called when the sell list of trading widget is clicked with the purpose of stopping a drag.
-- @param self Drag.
-- @param index Trading slot index number.
-- @return True if the click was handled by the drag and drop system.
Drag.clicked_trading = function(self, slot)
	-- Handle existing drags.
	-- If there's an item drag in progress, the item is dropped to the
	-- slot. Otherwise, the incompatible drag is cancelled.
	if self.drag then
		if self.drag[1] == "equ" then
			Network:send{packet = Packet(packets.TRADING_ADD_SELL, "uint32", self.drag[2], "string", self.drag[3], "uint8", slot - 1, "uint32", self.drag[5])}
			self:clear()
		elseif self.drag[1] == "inv" then
			Network:send{packet = Packet(packets.TRADING_ADD_SELL, "uint32", self.drag[2], "string", tostring(self.drag[3] - 1), "uint8", slot - 1, "uint32", self.drag[5])}
			self:clear()
		else
			self:cancel()
		end
		return true
	end
end

--- Called when the scene is clicked with the purpose of stopping a drag.
-- @param self Drag.
-- @return True if the click was handled by the drag and drop system.
Drag.clicked_scene = function(self)
	-- Handle existing drags.
	-- If there's an item drag in progress, the item is dropped to the
	-- world. Otherwise, the incompatible drag is cancelled.
	if self.drag then
		if self.drag[1] == "equ" or self.drag[1] == "inv" then
			Equipment:move(self.drag[1], self.drag[2], self.drag[3], "obj")
			self:clear()
		else
			self:cancel()
		end
		return true
	end
	-- TODO: Dragging from the world to the inventory.
end

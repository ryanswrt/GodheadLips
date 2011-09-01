Widgets.Trading = Class(Widget)

--- Creates a new trading widget.
-- @param clss Trading widget class.
-- @return Trading widget.
Widgets.Trading.new = function(clss)
	local self = Widget.new(clss, {cols = 1, spacings = {0, 0}})
	-- Lists.
	self.sell_list = Widgets.ItemList{cols = 2, size = 8, activated = function(w, r, a) self:activated("sell", a, r) end}
	self.buy_list = Widgets.ItemList{cols = 2, size = 8, activated = function(w, r, a) self:activated("buy", a, r) end}
	self.shop_list = Widgets.ItemList{cols = 5, size = 20, activated = function(w, r, a) self:activated("shop", a, r) end}
	-- Frame.
	self.title = Widgets.Frame{style = "title", text = "Trading"}
	self.group = Widgets.Frame{cols = 3, rows = 3}
	self.group:set_child(1, 1, Widgets.Label{text = "Payment"})
	self.group:set_child(1, 2, self.sell_list)
	self.group:set_child(2, 1, Widgets.Label{text = "Goods"})
	self.group:set_child(2, 2, self.buy_list)
	self.group:set_child(3, 1, Widgets.Label{text = "Shop"})
	self.group:set_child(3, 2, self.shop_list)
	-- Buttons.
	self.label_decline = Widgets.Label{color = {1,0,0,1}, halign = 0.5, text = "Pay more!"}
	self.button_accept = Widgets.Button{text = "Accept", pressed = function() self:accept() end}
	self.button_close = Widgets.Button{text = "Close", pressed = function() self:close() end}
	self.group_buttons = Widget{cols = 2, rows = 1, homogeneous = true}
	self.group_buttons:set_expand{col = 1, row = 1}
	self.group_buttons:set_expand{col = 2}
	self.group_buttons:set_child(1, 1, self.button_close)
	self.group_buttons:set_child(2, 1, self.button_accept)
	self.group:set_child(3, 3, self.group_buttons)
	-- Packing.
	self:set_request{width = 200}
	self:set_expand{col = 1, row = 1}
	self:append_row(self.title)
	self:append_row(self.group)
	self.accepted = true
	return self
end

--- Called when a slot was pressed.
-- @param self Trading widget.
-- @param panel Panel name.
-- @param args Event arguments.
-- @param slot Slot pressed.
Widgets.Trading.activated = function(self, panel, args, slot)
	if panel == "sell" then
		if not Drag:clicked_trading(slot) then
			Network:send{packet = Packet(packets.TRADING_REMOVE_SELL, "uint8", slot - 1)}
		end
	elseif panel == "buy" then
		Network:send{packet = Packet(packets.TRADING_REMOVE_BUY, "uint8", slot - 1)}
	else
		Network:send{packet = Packet(packets.TRADING_ADD_BUY, "uint8", slot - 1)}
	end
end

--- Closes the trading widget.
-- @param self Trading widget.
Widgets.Trading.accept = function(self)
	Network:send{packet = Packet(packets.TRADING_ACCEPT)}
end

--- Clears the contents of the trading widget.
-- @param self Trading widget.
Widgets.Trading.clear = function(self)
	for i=1,self.sell_list.size do
		local w = self.sell_list.buttons[i]
		w.name = nil
		w.count = nil
		w.icon = nil
	end
	for i=1,self.buy_list.size do
		local w = self.buy_list.buttons[i]
		w.name = nil
		w.count = nil
		w.icon = nil
	end
	for i=1,self.shop_list.size do
		local w = self.shop_list.buttons[i]
		w.name = nil
		w.count = nil
		w.icon = nil
	end
	self.accepted = true
end

--- Closes the trading widget.
-- @param self Trading widget.
Widgets.Trading.close = function(self)
	Network:send{packet = Packet(packets.TRADING_CANCEL)}
end

Widgets.Trading:add_getters{
	accepted = function(s) return rawget(s, "__accepted") end}

Widgets.Trading:add_setters{
	accepted = function(s, v)
		rawset(s, "__accepted", v)
		if v then
			s.group_buttons:set_child(2, 1, s.button_accept)
		else
			s.group_buttons:set_child(2, 1, s.label_decline)
		end
	end}

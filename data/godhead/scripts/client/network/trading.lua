Protocol:add_handler{type = "TRADING_ACCEPT", func = function(event)
	-- Read deal status.
	local ok,deal = event.packet:read("bool")
	if not ok then return end
	-- Change accept button state.
	Client.views.inventory.trading.accepted = deal
end}

Protocol:add_handler{type = "TRADING_ADD_BUY", func = function(event)
	-- Read item info.
	local ok,slot,name,count = event.packet:read("uint8", "string", "uint32")
	if not ok then return end
	local spec = Itemspec:find{name = name}
	-- Update the trading list.
	local widget = Client.views.inventory.trading.buy_list.buttons[slot + 1]
	if not widget then return end
	widget.text = spec and spec.name
	widget.count = count
	widget.icon = spec and spec.icon
end}

Protocol:add_handler{type = "TRADING_ADD_SELL", func = function(event)
	-- Read item info.
	local ok,slot,name,count = event.packet:read("uint8", "string", "uint32")
	if not ok then return end
	local spec = Itemspec:find{name = name}
	-- Update the trading list.
	local widget = Client.views.inventory.trading.sell_list.buttons[slot + 1]
	if not widget then return end
	widget.text = spec and spec.name
	widget.count = count
	widget.icon = spec and spec.icon
end}

Protocol:add_handler{type = "TRADING_CANCEL", func = function(event)
	Client.views.inventory:set_trading()
end}

Protocol:add_handler{type = "TRADING_REMOVE_BUY", func = function(event)
	-- Read item info.
	local ok,slot = event.packet:read("uint8")
	if not ok then return end
	-- Update the trading list.
	local widget = Client.views.inventory.trading.buy_list.buttons[slot + 1]
	if not widget then return end
	widget.text = nil
	widget.count = nil
	widget.icon = nil
end}

Protocol:add_handler{type = "TRADING_REMOVE_SELL", func = function(event)
	-- Read item info.
	local ok,slot = event.packet:read("uint8")
	if not ok then return end
	-- Update the trading list.
	local widget = Client.views.inventory.trading.sell_list.buttons[slot + 1]
	if not widget then return end
	widget.text = nil
	widget.count = nil
	widget.icon = nil
end}

Protocol:add_handler{type = "TRADING_START", func = function(event)
	-- Activate the trading view.
	-- Needs to be done first because the item list are cleared here.
	Client:set_mode("inventory")
	Client.views.inventory:set_trading(true)
	-- Read the shop list length.
	local ok,count = event.packet:read("uint8")
	if not ok then return end
	count = math.min(count, Client.views.inventory.trading.shop_list.size)
	-- Read the shop list.
	for i=1,count do
		local ok,name = event.packet:resume("string")
		if not ok then break end
		local widget = Client.views.inventory.trading.shop_list.buttons[i]
		if widget then
			local spec = Itemspec:find{name = name}
			widget.text = spec and spec.name
			widget.count = 1
			widget.icon = spec and spec.icon
		end
	end
end}

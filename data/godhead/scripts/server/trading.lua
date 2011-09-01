Trading = Class()

Trading.accept = function(clss, player)
	-- Check for an acceptable deal.
	if not player.trading then return end
	if not clss:deal(player) then return end
	-- Give the bought items.
	for k,v in pairs(player.trading.buy) do
		local s = Itemspec:find{name = v[1]}
		local o = Item{spec = s, count = v[2]}
		player:give_item(o)
	end
	-- Close the trading screen.
	player.trading = nil
	player:send(Packet(packets.TRADING_CANCEL))
end

Trading.cancel = function(clss, player)
	if not player.trading then return end
	player:send(Packet(packets.TRADING_CANCEL))
	for k,v in pairs(player.trading.sell) do
		player:give_item(v)
	end
	player.trading = nil
end

Trading.deal = function(clss, player)
	-- Calculate the value of the items sold by the player.
	local sell = 0
	for k,v in pairs(player.trading.sell) do
		local spec = v.spec
		if spec then sell = sell + spec:get_trading_value() * v.count end
	end
	-- Calculate the value of the items bought by the player.
	local buy = 0
	for k,v in pairs(player.trading.buy) do
		local spec = Itemspec:find{name = v[1]}
		if spec then buy = buy + spec:get_trading_value() * v[2] end
	end
	-- Accept if the merchant profits.
	-- TODO: Should merchants be more favorable to the same race?
	-- TODO: Should there be other discriminative conditions?
	return 1.5 * sell >= buy
end

Trading.start = function(clss, player, merchant)
	-- Initialize the trading data.
	clss:cancel(player)
	player.trading = {sell = {}, buy = {}, shop = {}, merchant = merchant}
	-- Create the shop list.
	-- TODO: Should depend on the merchant.
	local count = 20
	local data = {}
	for i = 1,count do
		local s = Itemspec:random()
		table.insert(player.trading.shop, s.name)
		table.insert(data, "string")
		table.insert(data, s.name)
	end
	-- Send the trading start packet.
	player:send(Packet(packets.TRADING_START, "uint8", count, unpack(data)))
end

Trading.update = function(clss, player)
	player:send(Packet(packets.TRADING_ACCEPT, "bool", clss:deal(player)))
end

Protocol:add_handler{type = "TRADING_ACCEPT", func = function(args)
	-- Find the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	if not player.trading then return end
	-- Accept trading.
	Trading:accept(player)
end}

Protocol:add_handler{type = "TRADING_ADD_BUY", func = function(args)
	-- Find the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	if not player.trading then return end
	-- Read slot info.
	local ok,slot = args.packet:read("uint8")
	if not ok then return end
	-- Get the bought item.
	local name = player.trading.shop[slot + 1]
	if not name then return end
	-- Choose the destination slot.
	local dst
	for i=1,8 do
		if not player.trading.buy[i] then
			dst = i
			break
		end
	end
	if not dst then return end
	-- Add to the buy list.
	player.trading.buy[dst] = {name, 1}
	player:send(Packet(packets.TRADING_ADD_BUY, "uint8", dst - 1, "string", name, "uint32", 1))
	-- Update deal status.
	Trading:update(player)
end}

Protocol:add_handler{type = "TRADING_ADD_SELL", func = function(args)
	-- Find the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	if not player.trading then return end
	-- Read slots.
	local ok,srcid,srcslot,dst,count = args.packet:read("uint32", "string", "uint8", "uint32")
	if not ok then return end
	local srcslotnum = tonumber(srcslot)
	if srcslotnum then srcslot = srcslotnum + 1 end
	-- Validate slots.
	if dst >= 8 then return end
	local srcinv = player:find_open_inventory(srcid)
	if not srcinv then return end
	local srcobj = srcinv:get_object{slot = srcslot}
	if not srcobj then return end
	local dstobj = player.trading.sell[dst + 1]
	if dstobj then return end -- TODO: swap
	-- Validate count.
	if count == 0 or count > srcobj.count then return end
	-- Move to the trading list.
	if count < srcobj.count then
		local o = srcobj:split{count = count}
		player.trading.sell[dst + 1] = o
	else
		srcobj:detach()
		player.trading.sell[dst + 1] = srcobj
	end
	player:send(Packet(packets.TRADING_ADD_SELL, "uint8", dst, "string", srcobj.spec.name, "uint32", count))
	-- Update deal status.
	Trading:update(player)
end}

Protocol:add_handler{type = "TRADING_CANCEL", func = function(args)
	-- Find the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	if not player.trading then return end
	-- Cancel trading.
	Trading:cancel(player)
end}

Protocol:add_handler{type = "TRADING_REMOVE_BUY", func = function(args)
	-- Find the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	if not player.trading then return end
	-- Read slot info.
	local ok,slot = args.packet:read("uint8")
	if not ok then return end
	if not player.trading.buy[slot + 1] then return end
	-- Remove from the buy list.
	player.trading.buy[slot + 1] = nil
	player:send(Packet(packets.TRADING_REMOVE_BUY, "uint8", slot))
	-- Update deal status.
	Trading:update(player)
end}

Protocol:add_handler{type = "TRADING_REMOVE_SELL", func = function(args)
	-- Find the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	if not player.trading then return end
	-- Read slots.
	local ok,src = args.packet:read("uint8")
	if not ok then return end
	if src >= 8 then return end
	local srcobj = player.trading.sell[src + 1]
	if not srcobj then return end
	-- Move to the inventory.
	player:give_item(srcobj)
	player.trading.sell[src + 1] = nil
	player:send(Packet(packets.TRADING_REMOVE_SELL, "uint8", src))
	-- Update deal status.
	Trading:update(player)
end}

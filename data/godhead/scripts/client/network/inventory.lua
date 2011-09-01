-- Loots a container.
Protocol:add_handler{type = "INVENTORY_CREATED", func = function(event)
	-- Parse the packet.
	local ok,id,typ,nam,size,own = event.packet:read("uint32", "string", "string", "uint8", "bool")
	if not ok then return end
	-- Determine the container type.
	local mapping = {
		item = function() return Itemspec:find{name = nam} end,
		obstacle = function() return Obstaclespec:find{name = nam} end,
		species = function() return Species:find{name = nam} end}
	local func = mapping[typ]
	local spec = func and func()
	-- Create the container.
	local cont = Widgets.Container{id = id, size = size, spec = spec}
	Client.views.inventory:add_container(cont, own)
end}

-- Closes a container.
Protocol:add_handler{type = "INVENTORY_CLOSED", func = function(event)
	-- Parse the packet.
	local ok,id = event.packet:read("uint32")
	if not ok then return end
	-- Close the container widget if it's still open.
	local cont = Widgets.Container:find(id)
	if cont then cont:close(true) end
	-- Clear interrupted item drags.
	-- The container may close if another player picks it up or moves
	-- it out of reach. If that happens, any drags from that container
	-- need to be cancelled to avoid problems.
	if Drag.drag and Drag.drag[2] == id then
		Drag:cancel()
	end
end}

-- Adds an item to a container.
Protocol:add_handler{type = "INVENTORY_ITEM_ADDED", func = function(event)
	-- Parse the packet.
	local ok,id,islot,sslot,count,name = event.packet:read("uint32", "uint8", "string", "uint32", "string")
	if not ok then return end
	-- Update the container.
	local cont = Widgets.Container:find(id)
	if not cont then return end
	local spec = Itemspec:find{name = name}
	local icon = spec and spec.icon
	cont:set_item{slot = (sslot ~= "" and sslot or islot), icon = icon, name = name, count = count}
	-- Update quickslots.
	if cont == Client.views.inventory.container and sslot == "" then
		Quickslots:assign_item(islot, {icon = icon, name = name, count = count})
	end
	-- Clear interrupted item drags.
	-- When multiple players try to drag the same item, the fastest drag
	-- succeeds. Other drags need to be cancelled so that the drag icon
	-- disappears and the source slot isn't incorrectly left grayed out.
	if Drag.drag and Drag.drag[2] == id and Drag.drag[3] == slot then
		Drag:cancel()
	end
end}

-- Removes an item from a container.
Protocol:add_handler{type = "INVENTORY_ITEM_REMOVED", func = function(event)
	-- Parse the packet.
	local ok,id,islot,sslot = event.packet:read("uint32", "uint8", "string")
	if not ok then return end
	local cont = Widgets.Container:find(id)
	if not cont then return end
	-- Remove from the container.
	local slot = (sslot ~= "" and sslot or islot)
	cont:set_item{slot = slot}
	-- Update quickslots.
	if cont == Client.views.inventory.container and sslot == "" then
		Quickslots:assign_item(islot)
	end
	-- Clear interrupted item drags.
	-- When multiple players try to drag the same item, the fastest drag
	-- succeeds. Other drags need to be cancelled so that the drag icon
	-- disappears and the source slot isn't incorrectly left grayed out.
	if Drag.drag and Drag.drag[2] == id and Drag.drag[3] == slot then
		Drag:cancel()
	end
end}

-- Updates the weight and burdening limit of the player.
Protocol:add_handler{type = "INVENTORY_WEIGHT", func = function(event)
	-- Parse the packet.
	local ok,w,l = event.packet:read("uint16", "uint16")
	if not ok then return end
	-- Update the inventory view.
	Client.views.inventory:set_weight(w, l)
end}

Commands = {}

--- Initiates targeting for the examine command.
-- @param self Commands class.
Commands.examine = function(self)
	Target:start("What would you like to examine?", function(where, id, slot)
		if where == "inv" then
			Network:send{packet = Packet(packets.PLAYER_EXAMINE, "uint32", id, "uint32", slot)}
		elseif where == "obj" then
			Network:send{packet = Packet(packets.PLAYER_EXAMINE, "uint32", 0, "uint32", id)}
		end
	end)
end

--- Initiates targeting for the pick up command.
-- @param self Commands class.
Commands.pickup = function(self)
	Target:start("What would you like to pick up?", function(where, id, slot)
		local cont = Client.views.inventory.container
		if where == "obj" and cont then
			Network:send{packet = Packet(packets.MOVE_ITEM,
				"uint8", moveitem.WORLD, "uint8", moveitem.INVENTORY,
				"uint32", 0,
				"uint32", id, "uint32", cont.id, "uint32", 0)}
		end
	end)
end

--- Initiates targeting for the throw command.
-- @param self Commands class.
Commands.throw = function(self)
	Target:start("What would you like to throw?", function(where, id, slot)
		if where == "inv" then
			Network:send{packet = Packet(packets.THROW, "uint32", id, "uint32", slot)}
		end
	end)
end

--- Initiates targeting for the use command.
-- @param self Commands class.
-- @param inv Inventory number, zero for world, nil for prompt.
-- @param slot Inventory slot, object ID if using a world object, nil for prompt.
Commands.use = function(self, inv, slot)
	if not inv or not slot then
		-- Prompt for target.
		Target:start("What would you like to use?", function(where, id, slot)
			if where == "inv" then
				self:use(id, slot)
			elseif where == "obj" then
				self:use(0, id)
			end
		end)
	else
		-- Target known so use.
		if type(slot) == "number" then
			Network:send{packet = Packet(packets.PLAYER_USE, "uint32", inv, "uint8", 0, "uint32", slot)}
			if inv == 0 then
				Gui:set_dialog(slot)
			end
		else
			Network:send{packet = Packet(packets.PLAYER_USE, "uint32", inv, "uint8", 1, "string", slot)}
		end
	end
end

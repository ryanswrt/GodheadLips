Equipment = Class()

Equipment.move = function(self, src_type, src_id, src_slot, dst_type, dst_id, dst_slot, count)

	if not Player.object then return end
	local pos = Player.object.position
	Effect:play("item1")

	-- Equipment -> equipment.
	if src_type == "equ" and dst_type == "equ" then
		Network:send{packet = Packet(packets.MOVE_ITEM,
			"uint8", moveitem.EQUIPMENT, "uint8", moveitem.EQUIPMENT,
			"uint32", count,
			"uint32", src_id, "string", src_slot,
			"uint32", dst_id, "string", dst_slot)}

	-- Equipment -> inventory.
	elseif src_type == "equ" and dst_type == "inv" then
		Network:send{packet = Packet(packets.MOVE_ITEM,
			"uint8", moveitem.EQUIPMENT, "uint8", moveitem.INVENTORY,
			"uint32", count,
			"uint32", src_id, "string", src_slot,
			"uint32", dst_id, "uint8", dst_slot)}

	-- Equipment -> world.
	elseif src_type == "equ" and dst_type == "obj" then
		Network:send{packet = Packet(packets.MOVE_ITEM,
			"uint8", moveitem.EQUIPMENT, "uint8", moveitem.WORLD,
			"uint32", count,
			"uint32", src_id, "string", src_slot,
			"float", pos.x, "float", pos.y, "float", pos.z, "float", 0.0)}

	-- Inventory -> equipped.
	elseif src_type == "inv" and dst_type == "equ" then
		Network:send{packet = Packet(packets.MOVE_ITEM,
			"uint8", moveitem.INVENTORY, "uint8", moveitem.EQUIPMENT,
			"uint32", count,
			"uint32", src_id, "uint8", src_slot,
			"uint32", dst_id, "string", dst_slot)}

	-- Inventory -> inventory.
	elseif src_type == "inv" and dst_type == "inv" then
		Network:send{packet = Packet(packets.MOVE_ITEM,
			"uint8", moveitem.INVENTORY, "uint8", moveitem.INVENTORY,
			"uint32", count,
			"uint32", src_id, "uint8", src_slot,
			"uint32", dst_id, "uint8", dst_slot)}

	-- Inventory -> world.
	elseif src_type == "inv" and dst_type == "obj" then
		Network:send{packet = Packet(packets.MOVE_ITEM,
			"uint8", moveitem.INVENTORY, "uint8", moveitem.WORLD,
			"uint32", count,
			"uint32", src_id, "uint8", src_slot,
			"float", pos.x, "float", pos.y, "float", pos.z, "float", 0.0)}

	-- World -> equipment.
	elseif src_type == "obj" and dst_type == "equ" then
		Network:send{packet = Packet(packets.MOVE_ITEM,
			"uint8", moveitem.WORLD, "uint8", moveitem.EQUIPMENT,
			"uint32", count,
			"uint32", src_id,
			"uint32", dst_id, "string", dst_slot)}

	-- World -> inventory.
	elseif src_type == "obj" and dst_type == "inv" then
		Network:send{packet = Packet(packets.MOVE_ITEM,
			"uint8", moveitem.WORLD, "uint8", moveitem.EQUIPMENT,
			"uint32", count,
			"uint32", src_id,
			"uint32", dst_id, "uint8", dst_slot)}

	-- World -> world.
	elseif src_type == "obj" and dst_type == "obj" then
		Network:send{packet = Packet(packets.MOVE_ITEM,
			"uint8", moveitem.WORLD, "uint8", moveitem.WORLD,
			"uint32", count,
			"uint32", src_id,
			"float", pos.x, "float", pos.y, "float", pos.z, "float", 0.0)}
	end

end

-- Automatically equips an item.
Actionspec{name = "autoequip", type = "item", func = function(item, user)
	if not item.spec.equipment_slot then return end
	local inv = Inventory:find{object = item}
	if not inv then return end
	local _,srcslot = inv:find_object{object = item}
	if type(srcslot) == "string" then
		-- Unequip items in equipment slots.
		local dstslot = user.inventory:get_empty_slot()
		if not dstslot then return end
		Actions:move_from_inv_to_inv(user, inv.id, srcslot, user.id, dstslot)
	else
		-- Equip items in inventory slots.
		Actions:move_from_inv_to_inv(user, inv.id, srcslot, user.id, item.spec.equipment_slot)
	end
end}

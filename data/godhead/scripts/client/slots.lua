require "common/inventory"

Slots = Class(Inventory)

--- Sets an equipment item.
-- @param self Slots.
-- @param args Arguments.<ul>
--   <li>model: Model name.</li>
--   <li>slot: Slot name.</li></ul>
Slots.set_object = function(self, args)
	-- Delete old item.
	local object = self:get_object(args)
	if object then
		object:detach()
	end
	-- Create new item.
	if args.model then
		object = Object{collision_group = Physics.GROUP_EQUIPMENT, spec = args.spec}
		object:set_model()
		self.slots[args.slot] = object
	else
		self.slots[args.slot] = nil
	end
end

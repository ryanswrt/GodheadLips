--- Combined inventory and equipment item storage.<br/>
-- A generic inventory class that allows one to easily store and retrieve
-- items. Both objects and strings are supported as items so the class is
-- usable to both the client and the server.
-- @name Inventory
-- @class table
Inventory = Class()
Inventory.class_name = "Inventory"
Inventory.dict_object = {}
setmetatable(Inventory.dict_object, {__mode = "kv"})

--- Finds an inventory by owner or stored object.
-- @param clss Inventory class.
-- @param args Arguments.<ul>
--   <li>id: Inventory ID.</li>
--   <li>object: Stored object.</li></ul>
-- @return Inventory or nil.
Inventory.find = function(clss, args)
	if args.id then
		local owner = Object:find{id = args.id}
		return owner and owner.inventory
	elseif args.object then
		return clss.dict_object[args.object.id]
	end
end

--- Creates a new inventory.
-- @param clss Inventory class.
-- @param args Arguments.
-- @return New inventory.
Inventory.new = function(clss, args)
	local self = Class.new(clss, args)
	self.size = args and args.size or 10
	self.slots = {}
	self.listeners = setmetatable({}, {__mode = "k"})
	return self
end

--- Finds an object in the inventory.
-- @param self Inventory.
-- @param args Arguments.<ul>
--   <li>name: Item name to match.</li>
--   <li>object: Object to match.</li>
--   <li>type: Item category name to match.</li></ul>
-- @return Object and slot name or number, or nil if not found.
Inventory.find_object = function(self, args)
	if args.object then
		-- Search by object
		for k,v in pairs(self.slots) do
			if v == args.object then return v, k end
		end
	else
		-- Search by name and type.
		for k,v in pairs(self.slots) do
			if (not args.name or (v.name and v.name == args.name)) and
			   (not args.type or (v.spec.categories[args.type])) then
				return v, k
			end
		end
	end
end

--- Gets the first empty slot in the inventory.
-- @param self Inventory.
Inventory.get_empty_slot = function(self)
	for slot = 1,self.size do
		if not self.slots[slot] then return slot end
	end
end

--- Gets an object in a slot.
-- @param self Inventory.
-- @param args Arguments.<ul>
--   <li>slot: Slot index.</li></ul>
-- @return Object or nil.
Inventory.get_object = function(self, args)
	return args.slot and self.slots[args.slot]
end

--- Intelligently adds an object to the inventory.<br/>
-- If no slot number is specified explicitly, loops through all objects in the
-- inventory, trying to merge the object with each of them, returning after the
-- first successful merge. If no merge succeeds, tries to add the object to the
-- first empty slot. If there are no empty slots, returns without adding the
-- object.<br/>
-- If an explicit slot number is requested, checks if the slot is empty. If so,
-- the object is added. Otherwise, a merge is attempted. If the merge fails,
-- the function returns without adding the object.<br/>
-- Note that merging and thus this function as well only works for objects. If
-- the inserted object or the object in the requested slot is nil, nothing is done.
-- @param self Inventory.
-- @param args Arguments.<ul>
--   <li>equip: True to auto-equip if possible.</li>
--   <li>exclude: Slot number to exclude.</li>
--   <li>object: Object.</li>
--   <li>slot: Slot name or number if requiring a specific slot.</li></ul>
-- @return True if merged or added.
Inventory.merge_object = function(self, args)
	if type(args.object) ~= "table" then return end
	if type(args.slot) == "number" then
		-- Merge or insert to a specific inventory slot.
		local object = self:get_object{slot = args.slot}
		if not object then
			args.object:detach()
			self:set_object(args)
			return true
		elseif object:merge{object = args.object} then
			args.object:detach()
			self:update_slot(args.slot)
			return true
		end
	elseif type(args.slot) == "string" then
		-- Merge or insert to specific equipment slot.
		local object = self:get_object{slot = args.slot}
		if not object then
			args.object:detach()
			self:set_object(args)
			return true
		elseif object:merge{object = args.object} then
			args.object:detach()
			self:update_slot(args.slot)
			return true
		end
	else
		-- Merge to any equipment slot.
		if args.equip then
			for slot in pairs(self.slots) do
				if type(slot) == "string" and slot ~= args.exclude then
					local object = self:get_object{slot = slot}
					if object and object:merge{object = args.object} then
						args.object:detach()
						self:update_slot(slot)
						return true
					end
				end
			end
		end
		-- Merge or insert to any slot.
		for slot = 1,self.size do
			if slot ~= args.exclude then
				local object = self:get_object{slot = slot}
				if object and object:merge{object = args.object} then
					args.object:detach()
					self:update_slot(slot)
					return true
				end
			end
		end
		local slot = self:get_empty_slot()
		if not slot then return end
		args.object:detach()
		self:set_object{slot = slot, object = args.object}
		return true
	end
end

--- Subscribes an object to the inventory.
-- @param self Inventory.
-- @param args Arguments.<ul>
--   <li>callback: Callback.</li>
--   <li>object: Object to subscribe.</li></ul>
Inventory.subscribe = function(self, args)
	if args.object and args.callback and not self.listeners[args.object] then
		self.listeners[args.object] = args.callback
		args.callback{type = "inventory-subscribed", inventory = self}
		for k,v in pairs(self.slots) do
			args.callback{type = "inventory-changed", inventory = self, object = v, slot = k}
		end
	end
end

--- Checks if an object is subscribed to the inventory.
-- @param self Inventory.
-- @param args Arguments.<ul>
--   <li>object: Object.</li></ul>
-- @return True if subscribed.
Inventory.subscribed = function(self, args)
	return self.listeners[args.object] ~= nil
end

--- Unsubscribes an object from the inventory.
-- @param self Inventory.
-- @param args Arguments.<ul>
--   <li>object: Object to unsubscribe.</li></ul>
Inventory.unsubscribe = function(self, args)
	local listener = self.listeners[args.object]
	if listener then
		listener{type = "inventory-unsubscribed", inventory = self}
		self.listeners[args.object] = nil
	end
end

--- Sets the object of a slot.
-- @param self Inventory.
-- @param args Arguments.<ul>
--   <li>slot: Slot name or number.</li>
--   <li>object: Object.</li></ul>
Inventory.set_object = function(self, args)
	-- Check for changes.
	if not args.slot then return end
	local oldobj = self.slots[args.slot]
	if oldobj == args.object then return end
	-- Maintain dictionaries.
	if oldobj then
		Inventory.dict_object[oldobj.id] = nil
	end
	if type(args.object) == "table" then
		args.object:detach()
		Inventory.dict_object[args.object.id] = self
	end
	-- Add to slot.
	self.slots[args.slot] = args.object
	self:update_slot(args.slot)
end

--- Emits a slot update event.</br>
-- A private inventory-changed event is always sent to the subscibers.
-- If the slot was a string, a public vision event is also send to
-- nearby objects as if they saw an item being equipped or unequipped.</br>
-- If there are no subscribers and the Vision class hasn't been loaded,
-- the function does nothing, hence making the class usable to both the
-- client and the server.
-- @param self Inventory.
-- @param slot Slot number or name.
Inventory.update_slot = function(self, slot)
	local o = self:get_object{slot = slot}
	-- Always send inventory change events to subscribers.
	for k,v in pairs(self.listeners) do
		v{type = "inventory-changed", inventory = self, object = o, slot = slot}
	end
	-- Only send public slot change events when an equipment slot changed.
	if type(slot) == "string" and Vision then
		local owner = Object:find{id = self.id}
		Vision:event{type = "slot-changed", item = o, id = self.id, slot = slot}
	end
end

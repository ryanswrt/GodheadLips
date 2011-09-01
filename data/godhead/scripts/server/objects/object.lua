local oldanimate = Object.animate
local objspec = Spec{name = "object", type = "object"}

Object:add_getters{
	admin = function(s)
		local a = rawget(s, "account")
		return a and Config.inst.admins[a.login]
	end,
	count = function(s)
		return rawget(s, "__count") or 1
	end,
	spec = function(s)
		return rawget(s, "__spec") or objspec
	end}

Object:add_setters{
	admin = function(s, v)
		Config.inst.admins[s] = v and true or nil
		Config.inst:save()
	end,
	count = function(s, v)
		-- Store the new count.
		if s.count == v then return end
		rawset(s, "__count", v ~= 0 and v or nil)
		-- Update the inventory containing the object.
		local inventory = Inventory:find{object = s}
		if inventory then
			for k,v in pairs(inventory.slots) do
				if v == s then
					inventory:update_slot(k)
					break
				end
			end
		end
	end,
	spec = function(s, v)
		rawset(s, "__spec", v)
	end}

--- Handles physics contacts.
-- @param self Object.
-- @param result Contact result.
-- @return True if the hit was processed.
Object.contact_cb = function(self, result)
	if not self.contact_args then
		self.contact_args = nil
		self.contact_events = false
		return
	end
	if result.object == self.contact_args.owner then return end
	self.contact_args.feat:apply{
		attacker = self.contact_args.owner,
		charge = self.contact_args.charge,
		point = result.point,
		projectile = self,
		target = result.object,
		tile = result.tile,
		weapon = self.contact_args.weapon}
	self.contact_args = nil
	self.contact_events = false
	self.realized = false
	return true
end

--- Creates a new object from a data string or database entry.
-- @param clss Object class.
-- @param args Arguments.<ul>
--  <li>data: Data string.</li>
--  <li>id: Object ID to search from the database.</li></ul>
-- @return Object or nil.
Object.load = function(clss, args)
	if args.data then
		local func = assert(loadstring("return function()\n" .. args.data .. "\nend"))()
		if func then return func() end
	elseif args.id then
		local rows = Serialize.db:query("SELECT * FROM objects WHERE id=?;", {args.id})
		for k,v in ipairs(rows) do
			local func = assert(loadstring("return function()\n" .. v[3] .. "\nend"))()
			return func and func()
		end
	end
end

--- Default add enemy call.
-- @param self Object.
-- @param object Object to add to the list of enemies.
Object.add_enemy = function(self, object)
end

--- Merges or adds an item to the slots or inventory of the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>object: Object to add.</li>
--   <li>slot: Slot name.</li></ul>
-- @return True if succeeded.
Object.add_item = function(self, args)
	if not args.object then return end
	if not self.inventory then return end
	if self.inventory:merge_object(args) then return true end
end

--- Plays an animation.
-- @param self Object.
-- @param name Animation name.
-- @param force_temporary Forces the animation to be temporary in the server side.
-- @return True if started a new animation.
Object.animate = function(self, name, force_temporary)
	-- Prevent animation when dead.
	-- This is a simple way to ensure that dead creatures look like dead.
	if self.dead then return end
	-- Maintain channels.
	-- When objects enter the vision of a player, the player class enumerates
	-- through the persistent animations and sends them to the client. We need
	-- to store them so that newly seen objects don't appear unanimated.
	local anim = self.spec.animations[name]
	if anim and anim.channel then
		if anim.permanent and not force_temporary then
			if not self.animations then self.animations = {} end
			local prev = self.animations[anim.channel]
			if prev and prev[1] == name then return end
			self.animations[anim.channel] = {name, Program.time}
		else
			self.animations[anim.channel] = nil
		end
	end
	-- Emit a vision event.
	Vision:event({type = "object-animated", animation = name, object = self})
	return true
end

--- Calculates the weight of contained items.
-- @param self Object.
-- @return Weight in kilograms.
Object.calculate_carried_weight = function(self)
	if not self.inventory then return 0 end
	local w = 0
	for k,v in pairs(self.inventory.slots) do
		w = w + v.spec.mass_inventory * v.count + v:calculate_carried_weight()
	end
	return w
end

--- Clones the object.
-- @param self Object.
-- @return New object.
Object.clone = function(self)
	local data = string.gsub(self:save(), "id=[0-9]*,", "")
	return Object:load{data = data}
end

--- Checks if the object contains the given object in its inventory.<br/>
-- The check is recursive so all the containers contained by the object
-- are also tested. The object is also considered to contain itself.
-- @param self Object.
-- @param object Object.
-- @return Container object and slot if found, nil otherwise.
Object.contains_item = function(self, object)
	local p = object
	while p do
		if p == self then return true end
		local inv = Inventory:find{object = p}
		if not inv then return end
		p = inv.owner
	end
end

--- Causes the object to take damage.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>amount: Amount of damage.</li>
--   <li>point: Damage point.</li>
--   <li>type: Damage type.</li></ul>
Object.damaged = function(self, args)
end

--- Detaches the object from the world or any container.
-- @param self Object.
Object.detach = function(self)
	-- Detach from world.
	self.realized = false
	-- Detach from inventory.
	local inv = Inventory:find{object = self}
	if inv then
		for k,v in pairs(inv.slots) do
			if v == self then
				inv:set_object{slot = k}
				break
			end
		end
	end
end

--- Hides the object and purges it from the database.
-- @param self Object to kill.
Object.die = function(self)
	self:detach()
	self:purge()
end

Object.effect = function(self, args)
	if args.effect then
		Vision:event{type = "object-effect", object = self, effect = args.effect}
	end
end

--- Finds an open inventory.
-- @param self Object.
-- @param id Inventory ID.
-- @return Inventory or nil.
Object.find_open_inventory = function(self, id)
	local inv = Inventory:find{id = id}
	if not inv then return end
	if not inv:subscribed{object = self} then return end
	return inv
end

--- Finds a targeted object.
-- @param self Object.
-- @param where Inventory number or zero for world.
-- @param what Inventory slot number or object number for world.
-- @return Object or nil.
Object.find_target = function(self, where, what)
	if where == 0 then
		return Object:find{id = what, point = self.position, radius = 5}
	else
		local inventory = Inventory:find{id = where}
		if inventory and inventory:subscribed{object = self} then
			return inventory:get_object{slot = what}
		end
	end
	return nil
end

--- Fires or throws the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>charge: Charge time of the attack.</li>
--   <li>collision: Trigger at collision.</li>
--   <li>feat: Feat.</li>
--   <li>owner: Object firing the projectile.</li>
--   <li>speed: Initial speed.</li>
--   <li>speedline: True if the object should have a speed line effect.</li>
--   <li>timer: Trigger at timeout.</li>
--   <li>weapon: Used weapon.</ul>
-- @return True on success.
Object.fire = function(self, args)
	if not args.owner or not args.feat then return end
	-- Enable collision callback.
	if args.collision then
		self.contact_args = args
		self.contact_events = true
	end
	-- Enable destruction timer.
	if args.timer then
		Timer{delay = args.timer, func = function(timer)
			self:die()
			timer:disable()
		end}
	end
	-- Initialize speed lines.
	if args.speedline then
		self.flags = Bitwise:bor(self.flags or 0, Protocol.object_flags.SPEEDLINE)
	end
	-- Store attack charge.
	self.charge = args.charge
	-- Add the projectile to the world.
	local src,dst = args.owner:get_attack_ray()
	self:detach()
	self.owner = args.owner
	self.position = src
	if args.owner.tilt then
		self.rotation = args.owner.rotation * args.owner.tilt
	else
		self.rotation = args.owner.rotation
	end
	self.velocity = (dst - src):normalize() * (args.speed or 20)
	self.realized = true
end

--- Gets a free object ID.
-- @param clss Object class.
-- @return Free object ID.
Object.get_free_id = function(clss)
	while true do
		local id = math.random(0x0000001, 0x0FFFFFF)
		if not Object:find{id = id} then
			local rows = Serialize.db:query("SELECT id FROM objects WHERE id=?;", {id})
			if not rows[1] then
				return id
			end
		end
	end
end

--- Finds an item from the inventory.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>name: Item name.</li>
--   <li>slot: Slot name or number.</li></ul>
-- @return Object or nil.
Object.get_item = function(self, args)
	if not self.inventory then return end
	if args.slot then return self.inventory:get_object(args) end
	if args.name then return self.inventory:find_object(args) end
end

--- Gets a full name string for the object.
-- @param self Object.
-- @return String.
Object.get_name_with_count = function(self)
	local name = self.name or "unnamed object"
	local count = self.count
	if count > 1 then
		name = "" .. count .. "x " .. name
	end
	return name
end

--- Returns the range of tiles occupied by the object.
-- @param self Object.
-- @return Start vector, size vector.
Object.get_tile_range = function(self)
	-- TODO: Should depend on species.
	local size = Vector(1,self.spec.type == "creature" and 2 or 1,1)
	local src = self.position * Voxel.tile_scale
	src.x = math.floor(src.x)
	src.y = math.floor(src.y + 0.3)
	src.z = math.floor(src.z)
	return src, src + size - Vector(1,1,1)
end

--- Gives the item to the object or drops it to the floor if failed.
-- @param self Object.
-- @param object Object to give.
-- @return True if added to inventory, nil if dropped.
Object.give_item = function(self, object)
	if self:add_item{object = object} then return true end
	local p = Utils:find_drop_point{point = self.position}
	object.position = p or self.position
	object.realized = true
end

--- Inflicts a modifier on the object.
-- @param self Object.
-- @param name Modifier name.
-- @param strength Modifier strength.
-- @param args Args passed to modifier
Object.inflict_modifier = function(self, name, strength, args)
end

--- Loots the object.
-- @param self Object.
-- @param user Object doing the looting.
Object.loot = function(self, user)
	if self.inventory then
		self.inventory:subscribe{object = user, callback = function(args) user:inventory_cb(args) end}
		self:animate("loot")
		self.looted = true
	end
end

--- Merges the objects if they're similar.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>object: Object to merge to this one.</li></ul>
-- @return True if merged successfully.
Object.merge = function(self, args)
	local obj = args.object
	if self.spec == args.object.spec and self.spec.stacking then
		self.count = self.count + obj.count
		obj.realized = false
		return true
	end
end

Object.purge = function(self)
	Serialize.db:query("DELETE FROM objects WHERE id=?;", {self.id})
end

--- Saves the object to the database.
-- @param self Object.
Object.save = function(self)
	local data = self:write()
	Serialize.db:query("REPLACE INTO objects (id,sector,data) VALUES (?,?,?);", {self.id, self.sector, data})
end

--- Sends a chat message to all players near the object.
-- @param self Speaking object.
-- @param msg Message to send.
Object.say = function(self, msg)
	if msg then
		Vision:event{type = "object-speech", object = self, message = msg}
	end
end

--- Sends a packet to the client controlling the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>packet: Packet.</li>
--   <li>reliable: False for unreliable.</li></ul>
Object.send = function(self, args)
	if self.client then
		if type(args) == "string" then
			local packet = Packet(packets.MESSAGE, "string", args)
			Network:send{client = self.client, packet = packet, reliable = true}
		elseif args.class_name == "Packet" then
			Network:send{client = self.client, packet = args, reliable = true}
		else
			Network:send{client = self.client, packet = args.packet, reliable = args.reliable}
		end
	end
end

--- Sets the item in a specific inventory slot.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>object: Object to add.</li>
--   <li>slot: Slot name or number.</li></ul>
Object.set_item = function(self, args)
	if not args.object then return end
	if not self.inventory then return end
	self.inventory:set_object(args)
end

--- Splits items from the inventory of the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>count: Number of items to split.</li>
--   <li>name: Item name.</li></ul>
-- @return Object or nil.
Object.split_items = function(self, args)
	local inv = self.inventory
	if not inv then return end
	local obj = inv:find_object{name = args.name}
	if not obj then return end
	obj = obj:split(args)
	obj:detach()
	return obj
end

--- Fixes the position of a stuck object.
-- @param self Object.
-- @return True if fixing succeeded.
Object.stuck_fix = function(self)
	do return true end
	-- Get the tile position of the object.
	local src = self:get_tile_range()
	-- Find the closest empty tile.
	-- FIXME: The object doesn't necessarily fit inside one tile.
	local t,p = Voxel:find_tile{match = "empty", point = src * Voxel.tile_size, radius = 5 * Voxel.tile_size}
	if not t then return end
	-- Move the object to the empty tile.
	self.position = (p + Vector(0.5, 0.2, 0.5)) * Voxel.tile_size
	return true
end

--- Subtracts stacked objects.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>count: Count to subtract.</li></ul>
Object.subtract = function(self, args)
	local c = args and args.count or 1
	if self.count > c then
		self.count = self.count - c
	else
		self:detach()
		self:purge()
	end
end

--- Subtracts items from the inventory of the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>count: Count to subtract.</li>
--   <li>name: Name to match.</li>
--   <li>type: Type to match.</li></ul>
-- @return True if succeeded.
Object.subtract_items = function(self, args)
	local inv = self.inventory
	if not inv then return end
	local obj = inv:find_object(args)
	if not obj then return end
	obj:subtract(args)
	return true
end

--- Teleports the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>marker: Map marker name.</li>
--   <li>position: World position.</li>
--   <li>region: Region name.</li></ul>
-- @return True on success.
Object.teleport = function(self, args)
	-- Set the position.
	if args.marker then
		local marker = Marker:find{name = args.marker}
		if not marker or not marker.position then return end
		self.position = marker.position + Vector(0, 2, -1)
	elseif args.region then
		local reg = Regionspec:find{name = args.region}
		if not reg then return end
		self.position = reg.spawn_point_world
	elseif args.position then
		self.position = args.position
	else return end
	self.velocity = Vector()
	self.realized = true
	return true
end

--- Updates the environment of the object and tries to fix it if necessary.
-- @param self Object.
-- @param secs Seconds since the last update.
-- @return Boolean and environment statistics. The boolean is true if the object isn't permanently stuck.
Object.update_environment = function(self, secs)
	if not self.env_timer then self.env_timer = 0 end
	self.env_timer = self.env_timer + secs
	if self.env_timer < 2 then return true end
	-- Count tiles affecting us.
	if not self.realized then return true end
	local src,dst = self:get_tile_range()
	local res = Voxel:check_range(src, dst)
	-- Stuck handling.
--[[	if res.solid > 0 then
		self.stuck = (self.stuck or 0) + 2
		if self.stuck < 10 then
			self:stuck_fix()
		else
			print("Warning: " .. (self.spec.name or "an object") .. " was deleted because it was permanently stuck!")
			self.realized = false
			return nil, res
		end
	elseif self.stuck then
		if self.stuck > 1 then
			self.stuck = self.stuck -1
		else
			self.stuck = nil
		end
	end--]]
	return true, res
end

--- Called when the object is used.
-- @param self Object.
-- @param user User.
Object.use_cb = function(self, user)
	-- Try to start a dialog.
	if self.dialog then return end
	local dialog = Dialog{object = self, user = user}
	if dialog then
		self.dialog = dialog
		self.dialog:execute()
		return
	end
	-- Try to loot if no dialog.
	self:loot(user)
end

--- Serializes the object to a string.
-- @param self Object.
-- @return Data string.
Object.write = function(self)
	return string.format("local self=Object%s\n%s", serialize{
		angular = self.angular,
		id = self.id,
		mass = self.mass,
		name = self.name,
		model = self.model_name,
		position = self.position,
		rotation = self.rotation},
		"return self")
end

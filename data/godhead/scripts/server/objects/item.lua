require "server/objects/object"

Item = Class(Object)
Item.class_name = "Item"
Item.pickable = true

Item:add_setters{
	spec = function(self, value)
		local spec = type(value) == "string" and Itemspec:find{name = value} or value
		if not spec then return end
		rawset(self, "__spec", spec)
		self.mass = spec.mass
		self.model = spec.model
		self.name = spec.name
		self.friction_liquid = spec.water_friction
		self.gravity = spec.gravity
		self.gravity_liquid = spec.water_gravity
		-- Create the inventory.
		if spec.inventory_size and not self.inventory then
			self.inventory = Inventory{id = self.id, size = spec.inventory_size}
			for k,v in pairs(spec.inventory_items) do
				self:add_item{object = Item{spec = Itemspec:find{name = v}}}
			end
		end
		-- Create random loot.
		-- When the map generator or an admin command creates an object, the
		-- random field is set to indicate that random loot should be generated.
		-- The field isn't saved so loot is only created once as expected.
		if self.random and self.inventory and spec.loot_categories then
			local num_cat = #spec.loot_categories
			local num_item
			if spec.loot_count then
				num_item = math.random(spec.loot_count[1], spec.loot_count[2])
			else
				num_item = math.random(0, self.inventory.size)
			end
			for i = 1,num_item do
				local cat = spec.loot_categories[math.random(1, num_cat)]
				self:add_item{object = Item{spec = Itemspec:random{category = cat}}}
			end
		end
	end}


--- Creates an item.
-- @param clss Item class.
-- @param args Arguments.<ul>
--   <li>angular: Angular velocity.</li>
--   <li>id: Unique object ID or nil for a random free one.</li>
--   <li>physics: Physics mode.</li>
--   <li>position: Position vector of the item.</li>
--   <li>rotation: Rotation quaternion of the item.</li>
--   <li>realized: True to add the object to the simulation.</li></ul>
-- @return New item.
Item.new = function(clss, args)
	local self = Object.new(clss, {id = args.id})
	local copy = function(n, d)
		if args[n] ~= nil or d then
			self[n] = (args[n] ~= nil) and args[n] or d
		end
	end
	copy("angular")
	copy("count")
	copy("physics", "rigid")
	copy("position")
	copy("random")
	copy("rotation")
	copy("spec")
	copy("looted")
	copy("realized")
	copy("variables")
	self.update_timer = 0.3 * math.random()
	if self.looted then self:animate("looted") end
	return self
end

--- Creates a copy of the item.
-- @param self Object.
-- @return Object.
Item.clone = function(self)
	return Item{
		spec = self.spec,
		angular = self.angular,
		position = self.position,
		rotation = self.rotation}
end

--- Handles physics contacts.
-- @param self Object.
-- @param result Contact result.
Item.contact_cb = function(self, result)
	-- Multiple contact points might be generated during one frame so we
	-- might have to skip some of the calls.
	if not self.contact_args then return end
	if self.spec.categories["boomerang"] then
		-- Boomerang mode.
		if result.object == self.contact_args.owner then
			-- Owner catch.
			local o = self.contact_args.owner:get_weapon()
			if not o then
				self.contact_args.owner:set_weapon(self)
			elseif not o:merge{object = self} then
				self.contact_args.owner:add_item{object = proj}
			end
		else
			-- Damage target.
			self.contact_args.feat:apply{
				attacker = self.contact_args.owner,
				charge = self.contact_args.charge,
				point = result.point,
				target = result.object,
				tile = result.tile,
				weapon = self}
		end
		-- Disable boomerang mode.
		if self.timer then
			self.timer:disable()
			self.timer = nil
		end
		self.contact_events = false
		self.contact_args = nil
		self.gravity = self.spec.gravity
		self:animate("fly stop")
	else
		-- Projectile mode.
		if Object.contact_cb(self, result) then
			self.gravity = self.spec.gravity
		end
	end
end

--- Gets the armor class of the item.
-- @param self Object.
-- @param user Creature.
-- @return Armor rating.
Item.get_armor_class = function(self, user)
	return self.spec.armor_class
end

--- Gets the weapon damage types of the item.
-- @param self Object.
-- @param user Creature.
-- @return Array of influences.
Item.get_weapon_influences = function(self, user)
	if not self.spec.influences_base then return {} end
	-- Calculate the damage multiplier.
	local mult = 1
	local bonuses = self.spec.influences_bonus
	if bonuses and user.skills then
		for k,v in pairs(bonuses) do
			local s = user.skills:get_value{skill = k}
			if s then mult = mult * (1 + v * s) end
		end
	end
	-- Calculate influences.
	local influences = {}
	for k,v in pairs(self.spec.influences_base) do
		influences[k] = mult * v
	end
	return influences
end

--- Causes the object to take damage.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>amount: Amount of damage.</li>
--   <li>point: Damage point.</li>
--   <li>type: Damage type.</li></ul>
-- @return True if not broken yet.
Item.damaged = function(self, args)
	-- Items whose spec has no health are indestructible.
	if not self.spec.health then return true end
	-- Subtract health.
	local h = self.health or self.spec.health
	self.health = math.max(0, h - args.amount)
	-- Destroy when out of health.
	if self.health == 0 then
		self:die()
		return
	end
	return true
end

--- Destroys the item.
-- @param self Object.
Item.die = function(self)
	-- Mark as dead.
	if self.dead then return end
	self.dead = true
	-- Execute destruction actions.
	for k,v in ipairs(self.spec.destroy_actions) do
		if v == "explode" then
			Utils:explosion(self.position)
		end
	end
	-- Remove from the world.
	Object.die(self)
end

--- Splits items from the stack.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>count: Number of items to split.</li></ul>
-- @return Object.
Item.split = function(self, args)
	local c = args and args.count or 1
	if c < self.count then
		local o = self:clone()
		self:subtract{count = c}
		o.count = c
		return o
	end
	return self
end

--- Fires or throws the item.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>collision: Trigger at collision.</li>
--   <li>feat: Feat.</li>
--   <li>owner: Object firing the projectile.</li>
--   <li>speed: Initial speed.</li>
--   <li>timer: Trigger at timeout.</li></ul>
-- @return The split and fired item.
Item.fire = function(self, args)
	-- Split a projectile from the stack and fire it.
	if not args.owner or not args.feat then return end
	local proj = self:split()
	Object.fire(proj, args)
	proj.gravity = self.spec.gravity_projectile
	-- Special handling for boomerangs.
	if proj.spec.categories["boomerang"] then
		-- Work around an initial collision with the user.
		proj.position = proj.position + proj.rotation * Vector(0,0,-0.7)
		-- Enable boomerang return.
		proj.state = 0
		proj.rotation = Quaternion{axis = Vector(0,0,1), angle = -0.5 * math.pi}
		proj:animate("fly start")
		proj.timer = Timer{delay = 1, func = function(timer, secs)
			if proj.state == 0 then
				proj.velocity = proj.velocity * -2
				proj.state = 1
			elseif proj.state == 1 then
				proj.velocity = proj.velocity * 2
				proj.state = 2
			else
				proj.timer = nil
				proj.gravity = Config.gravity
				timer:disable()
			end
		end}
	end
	-- Return the fired projectile.
	return proj
end

--- Called when the object is examined.
-- @param self Object.
-- @param user User.
Item.examine_cb = function(self, user)
	user:send{packet = Packet(packets.MESSAGE, "string", self.spec.name)}
end

--- Called when the object is used.
-- @param self Object.
-- @param user User.
Item.use_cb = function(self, user)
	local playeffect = function()
		if self.spec.effect_use then
			Effect:play{effect = self.spec.effect_use, object = user}
		end
	end
	local rundialog = function()
		if not self.dialog then
			local dialog = Dialog{object = self, user = user}
			if dialog then
				self.dialog = dialog
				self.dialog:execute()
			end
		end
	end
	-- Find the use action of the item.
	-- Non-item actions shouldn't be used but we reject them just in case.
	local action = Actionspec:find{name = self.spec.action_use}
	if action and action.type ~= "item" then action = nil end
	-- Actions that take preference over picking up.
	-- Containers are looted instead of being picked up since the player
	-- usually doesn't want to pick up heavy chests. Books are read since
	-- they aren't horribly useful after being read once.
	if action and (action.name == "loot" or action.name == "read") then
		action.func(self, user)
		playeffect()
		rundialog()
		return
	end
	-- Pick up items not yet in the inventory of the user.
	-- This has the side-effect of not allowing items to be used before
	-- taken out of containers.
	local inv = Inventory:find{object = self}
	if not inv then
		return user:pick_up(self.id, user.id, 0)
	end
	local _,slot = inv:find_object{object = self}
	if inv.id ~= user.id then
		return Actions:move_from_inv_to_inv(user, inv.id, slot, user.id, 0)
	end
	-- Perform type specific actions.
	if action then action.func(self, user) end
	playeffect()
	rundialog()
end

--- Called when the item is being equipped.
-- @param self Object.
-- @param user User object.
-- @param slot Slot name.
Item.equipped = function(self, user, slot)
end

--- Called when the item is being unequipped.
-- @param self Object.
-- @param user User object.
-- @param slot Slot name.
Item.unequipped = function(self, user, slot)
end

--- Serializes the object to a string.
-- @param self Object.
-- @return Data string.
Item.write = function(self)
	return string.format("local self=Item%s\n%s%s", serialize{
		angular = self.angular,
		count = self.count,
		id = self.id,
		looted = self.looted,
		spec = self.spec.name,
		position = self.position,
		rotation = self.rotation,
		variables = self.variables},
		Serialize:encode_inventory(self.inventory),
		"return self")
end

require "server/trading"

Dialog = Class()
Dialog.dict_id = {}
Dialog.flags = {}

--- Creates a new dialog.
-- @param clss Quest class.
-- @param args Arguments.<ul>
--   <li>name: Dialog name.</li>
--   <li>object: Object controlled by the dialog.</li>
--   <li>spec: Dialog spec.</li>
--   <li>user: Object that started the dialog.</li></ul>
-- @return New dialog.
Dialog.new = function(clss, args)
	-- Find the dialog spec.
	local name = args.name or (args.spec and args.spec.name) or args.object.spec.dialog
	local spec = args.spec or Dialogspec:find{name = name}
	if not spec then return end
	-- Allocate self.
	local self = Class.new(clss, args)
	self.id = args.object.id
	self.name = name
	self.object = args.object
	self.spec = spec
	self.user = args.user
	-- Add to dictionaries.
	clss.dict_id[self.id] = self
	-- Attach to the object.
	self.object.dialog = self
	if self.object.spec.type == "creature" then
		self.object:update_ai_state()
	end
	-- Initialize the virtual machine.
	self.vm = {{exe = self.spec.commands, off = 0, pos = 1, len = #self.spec.commands}}
	return self
end

--- Answers a choice or finishes the shown message.<br/>
-- Continues a paused dialog until the next choice or message is encountered.
-- @param self Dialog.
-- @param user Object answering.
-- @param answer Answer string.
Dialog.answer = function(self, user, answer)
	if type(self.choices) == "table" then
		-- Choice.
		local sel = self.choices[answer]
		if not sel then return end
		self.user = user
		self.choices = nil
		table.insert(self.vm, 1, {exe = sel, off = 2, pos = 1, len = #sel - 2})
		self:execute()
	elseif self.choices == "info" or self.choices == "line" then
		-- Info or say.
		self.vm[1].pos = self.vm[1].pos + 1
		self.user = user
		self.choices = nil
		self:execute()
	end
end

--- Executes the dialog.<br/>
-- Continues the dialog until the next choice or message is encountered or
-- the dialog ends.
-- @param self Dialog.
Dialog.execute = function(self)
	-- Utility functions.
	local check_cond = function(c)
		-- Backward compatibility.
		if c.cond and not Dialog.flags[c.cond] then return end
		if c.cond_dead and not self.object.dead then return end
		if c.cond_not and Dialog.flags[c.cond_not] then return end
		-- New condition string.
		if not c.check then return true end
		for _,str in ipairs(string.split(c.check, "&")) do
			local list = string.split(str, ":")
			local type,name = list[1],list[2]
			if type == "dead" then
				if not self.object.dead then return end
			elseif type == "!dead" then
				if self.object.dead then return end
			elseif type == "flag" then
				if not Dialog.flags[name] then return end
			elseif type == "!flag" then
				if Dialog.flags[name] then return end
			elseif type == "var" then
				if not self.object.variables or not self.object.variables[name] then return end
			elseif type == "!var" then
				if self.object.variables and self.object.variables[name] then return end
			end
		end
		return true
	end
	local select_spawn_position = function(c)
		if c.position_absolute then return c.position_absolute end
		local pos = self.object.position
		if c.position_marker then
			local m = Marker:find{name = c.position_marker}
			if m then pos = m.position end
		end
		if c.position_relative then
			pos = pos + c.position_relative
		end
		local min = c.distance_min or 0
		local max = c.distance_max or 0
		if min < max then
			local x = min + (min-max)*math.random()
			local y = min + (min-max)*math.random()
			pos = pos + Vector(x,y)
		end
		return pos
	end
	-- Command handlers of the virtual machine.
	-- Handlers increment stack pointers and push and pop command arrays to the stack.
	local commands = {
		branch = function(vm, c)
			vm[1].pos = vm[1].pos + 1
			if check_cond(c) then
				table.insert(vm, 1, {exe = c, off = 1, pos = 1, len = #c - 1})
			end
		end,
		["break"] = function(vm, c)
			local num = c[2] or 1
			for i = 1,num do
				if not vm[1] then break end
				table.remove(vm, 1)
			end
		end,
		choice = function(vm, c)
			-- Construct the list of choices.
			local cmd = c
			local cmds = {}
			local choices = {}
			repeat
				if check_cond(cmd) then
					table.insert(choices, cmd[2])
					cmds[cmd[2]] = cmd
				end
				vm[1].pos = vm[1].pos + 1
				cmd = vm[1].exe[vm[1].pos + vm[1].off]
			until not cmd or cmd[1] ~= "choice"
			-- Publish the choices.
			local event = {type = "object-dialog", object = self.object, choices = choices}
			self.event = event
			Vision:event(event)
			-- Break until answered.
			self.choices = cmds
			self.user = nil
			return true
		end,
		["default death check"] = function(vm, c)
			if self.object.dead then
				self.object:loot(self.user)
				for i = #vm,1,-1 do vm[i] = nil end
			else
				vm[1].pos = vm[1].pos + 1
			end
		end,
		exit = function(vm, c)
			for i = #vm,1,-1 do vm[i] = nil end
		end,
		effect = function(vm, c)
			Effect:play{effect = c[2], object = self.object}
			vm[1].pos = vm[1].pos + 1
		end,
		["effect player"] = function(vm, c)
			Effect:play{effect = c[2], object = self.user}
			vm[1].pos = vm[1].pos + 1
		end,
		feat = function(vm, c)
			Feat:unlock(c)
			vm[1].pos = vm[1].pos + 1
		end,
		flag = function(vm, c)
			Dialog.flags[c[2]] = "true"
			Serialize:save_quests()
			vm[1].pos = vm[1].pos + 1
		end,
		func = function(vm, c)
			local f = c[2]
			vm[1].pos = vm[1].pos + 1
			f(self)
		end,
		["give player item"] = function(vm, c)
			vm[1].pos = vm[1].pos + 1
			local s = Itemspec:find{name = c[2]}
			if not s then return end
			local o = Item{spec = s, count = c.count}
			if self.user:give_item(o) then
				self.user:send("Received " .. c[2])
			else
				self.user:send("Received " .. c[2] .. " but couldn't carry it")
			end
		end,
		info = function(vm, c)
			-- Publish the info.
			self.event = {type = "object-dialog", object = self.object, message = string.format("(%s)", c[2])}
			Vision:event(self.event)
			-- Break until answered.
			self.choices = "info"
			self.user = nil
			return true
		end,
		loop = function(vm, c)
			vm[1].pos = 1
		end,
		loot = function(vm, c)
			self.object:loot(self.user)
			for i = #vm,1,-1 do vm[i] = nil end
		end,
		notification = function(vm, c)
			vm[1].pos = vm[1].pos + 1
			self.user:send{packet = Packet(packets.MESSAGE_NOTIFICATION, "string", c[2])}
		end,
		quest = function(vm, c)
			local q = Quest:find{name = c[2]}
			if q then q:update(c) end
			vm[1].pos = vm[1].pos + 1
		end,
		random = function(vm, c)
			local o = math.random(2, #c)
			vm[1].pos = vm[1].pos + 1
			table.insert(vm, 1, {exe = c, off = o - 1, pos = 1, len = 1})
		end,
		["remove player item"] = function(vm, c)
			vm[1].pos = vm[1].pos + 1
			local o = self.user:get_item{name = c[2]}
			if o then
				self.user:send("Lost " .. c[2])
				o:detach()
				table.insert(vm, 1, {exe = c, off = 2, pos = 1, len = 1})
			elseif #c >= 3 then
				table.insert(vm, 1, {exe = c, off = 3, pos = 1, len = 1})
			end
		end,
		["require player item"] = function(vm, c)
			vm[1].pos = vm[1].pos + 1
			local o = self.user:get_item{name = c[2]}
			if o then
				table.insert(vm, 1, {exe = c, off = 2, pos = 1, len = 1})
			elseif #c >= 3 then
				table.insert(vm, 1, {exe = c, off = 3, pos = 1, len = 1})
			end
		end,
		say = function(vm, c)
			-- Publish the line.
			self.event = {type = "object-dialog", object = self.object, character = c[2], message = c[3]}
			Vision:event(self.event)
			-- Break until answered.
			self.choices = "line"
			self.user = nil
			return true
		end,
		["spawn object"] = function(vm, c)
			-- Spawn the object.
			local spec1 = Species:find{name = c[2]}
			local spec2 = Itemspec:find{name = c[2]}
			local spec3 = Obstaclespec:find{name = c[2]}
			local object
			if spec1 then
				object = Creature{spec = spec1, random = true}
			elseif spec2 then
				object = Item{spec = spec2, random = true}
			elseif spec3 then
				object = Obstacle{spec = spec3, random = true}
			end
			-- Set the position.
			if object then
				object.position = select_spawn_position(c)
				if type(c.rotation) == "number" then
					object.rotation = Quaternion{axis = Vector(0,1), angle = c.rotation / math.pi * 180}
				elseif type(c.rotation) == "table" then
					object.rotation = c.rotation
				end
				object.realized = true
			end
			-- Create the marker.
			if object and c.assign_marker then
				local name = c.assign_marker
				local marker
				local index = 1
				while Marker:find{name = name} do
					name = string.format("%s(%d)", c.assign_marker, index)
					index = index + 1
				end
				object.marker = Marker{name = name, position = object.position, target = object.id}
				object.marker:unlock()
			end
			vm[1].pos = vm[1].pos + 1
		end,
		["spawn pattern"] = function(vm, c)
			local pat = Pattern:find{name = c[2]}
			if pat then
				local pos = select_spawn_position(c) * Voxel.tile_scale - pat.size * 0.5
				pos = pos:round()
				if c.erase_tiles then
					Voxel:fill_region{point = pos, size = pat.size}
				end
				Voxel:place_pattern{name = c[2], point = pos, rotation = c.rotation}
			end
			vm[1].pos = vm[1].pos + 1
		end,
		teleport = function(vm, c)
			self.user:teleport(c)
			vm[1].pos = vm[1].pos + 1
		end,
		trade = function(vm, c)
			Trading:start(self.user, self.object)
			for i = #vm,1,-1 do vm[i] = nil end
		end,
		["unlock marker"] = function(vm, c)
			local m = Marker:find{name = c[2]}
			if m and not m.unlocked then
				m:unlock()
			end
			vm[1].pos = vm[1].pos + 1
		end,
		var = function(vm, c)
			if not self.object.variables then self.object.variables = {} end
			self.object.variables[c[2]] = "true"
			vm[1].pos = vm[1].pos + 1
		end}
	-- Execute commands until break or end.
	local vm = self.vm
	while vm[1] do
		if vm[1].pos > vm[1].len then
			table.remove(vm, 1)
		else
			local cmd = vm[1].exe[vm[1].pos + vm[1].off]
			local fun = commands[cmd[1]]
			local brk = fun(vm, cmd)
			if brk then return end
		end
	end
	-- Reset at end.
	Vision:event{type = "object-dialog", object = self.object}
	Dialog.dict_id[self.id] = nil
	self.object.dialog = nil
	self.object = nil
	self.user = nil
end

------------------------------------------------------------------------------

Protocol:add_handler{type = "DIALOG_ANSWER", func = function(event)
	local ok,id,msg = event.packet:read("uint32", "string")
	if not ok then return end
	local dialog = Dialog.dict_id[id]
	local player = Player:find{client = event.client}
	if not dialog or not dialog.choices or not player.vision.objects[dialog.object] then return end
	dialog:answer(player, msg)
end}

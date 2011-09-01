require "client/widgets/modifier"

Widgets.Modifiers = Class(Widget)

--- Creates a new modifiers widget.
-- @param clss Modifiers class.
-- @return Modifiers widget.
Widgets.Modifiers.new = function(clss)
	local self = Widget.new(clss, {rows = 1})
	self.dict_id = {}
	self.dict_name = {}
	clss.inst = self
	return self
end

-- Adds a new modifier.
-- @param self Modifiers widget.
-- @param name Modifier name.
-- @param time Modifier duration.
Widgets.Modifiers.add = function(self, name, time)
	-- Special handling for light and plague.
	if name == "light" then Player:set_light(true) end
	if name == "plague" and not self.dict_name[name] then Effect:play("disease1") end
	-- Add the icon.
	local icon = self.dict_name[name]
	if icon then
		icon.time = time
	else
		local id = self.cols + 1
		icon = Widgets.Modifier{id = id, icon = Iconspec:find{name = "modifier-" .. name}, name = name, time = time}
		self.dict_id[id] = icon
		self.dict_name[name] = icon
		self:append_col(icon)
	end
end

--- Removes a modifier.
-- @param self Modifiers widget.
-- @param name Modifier name.
Widgets.Modifiers.remove = function(self, name)
	local icon = self.dict_name[name]
	if not icon then return end
	-- Remove from dictionaries.
	Widget.remove(self, {col = icon.id})
	self.dict_name[icon.name] = nil
	table.remove(self.dict_id, icon.id)
	-- Update indices of modifiers after the removed index.
	for id = icon.id,#self.dict_id do
		self.dict_id[id].id = id
	end
	-- Special handling for light.
	if name == "light" then Player:set_light(false) end
end

--- Updates the modifiers.
-- @param self Modifiers widget.
-- @param secs Seconds since the last update.
Widgets.Modifiers.update = function(self, secs)
	for k,v in pairs(self.dict_id) do
		if v.time < 10000 then
			v.time = math.max(0, v.time - secs)
		end
	end
end

Eventhandler{type = "tick", func = function(self, args)
	if Widgets.Modifiers.inst then
		Widgets.Modifiers.inst:update(args.secs)
	end
end}

Protocol:add_handler{type = "MODIFIER_ADD", func = function(event)
	local ok,n,t = event.packet:read("string", "float")
	if ok and Widgets.Modifiers.inst then
		Widgets.Modifiers.inst:add(n, t)
	end
end}

Protocol:add_handler{type = "MODIFIER_REMOVE", func = function(event)
	local ok,n = event.packet:read("string")
	if ok and Widgets.Modifiers.inst then
		Widgets.Modifiers.inst:remove(n)
	end
end}

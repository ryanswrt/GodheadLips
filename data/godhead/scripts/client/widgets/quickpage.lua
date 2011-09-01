require "client/widgets/quickslot"

Widgets.Quickpage = Class(Widget)
Widgets.Quickpage.class_name = "Widgets.Quickpage"

--- Creates a new quickslots page.
-- @param clss Quickpage class.
-- @param args Arguments.<ul>
--   <li>type: Page type.</li></ul>
-- @return Quickpage.
Widgets.Quickpage.new = function(clss, args)
	-- Create self.
	local self = Widget.new(clss, {rows = 1, spacings = {0,0}})
	self.type = args and args.type or "feats"
	-- Create buttons.
	self.buttons = {}
	for i = 1,10 do
		self.buttons[i] = Widgets.Quickslot{index = i, pressed = function(w, a)
			if a.button == 3 then
				if self.type == "feats" then self:assign_none(i) end
			else
				self:activate(i)
			end
		end}
		self:append_col(self.buttons[i])
	end
	self:append_col(Widgets.Label{text=self.type:gsub("^%l", string.upper)})
	-- Load feats from the database.
	self:load()
	return self
end

--- Clears a quickslot assignment.
-- @param self Quickpage.
-- @param index Quickslot index number.
Widgets.Quickpage.assign_none = function(self, index)
	if index > #self.buttons then return end
	self.buttons[index].feat = nil
	self.buttons[index].icon = nil
	self.buttons[index].item = nil
	self.buttons[index].text = nil
	self.buttons[index].tooltip = nil
	self:save()
end

--- Assigns a feat to a quickslot.
-- @param self Quickpage.
-- @param index Quickslot index number.
-- @param feat Feat to assign.
Widgets.Quickpage.assign_feat = function(self, index, feat)
	if index > #self.buttons then return end
	local icon = feat:get_icon()
	self.buttons[index].feat = feat
	self.buttons[index].icon = icon
	self.buttons[index].item = nil
	self.buttons[index].text = nil
	self.buttons[index].tooltip = Widgets.Feattooltip{feat = feat}
	self:save()
end

--- Assigns an item to a quickslot.
-- @param self Quickpage class.
-- @param index Quickslot index number.
-- @param item Item to assign.
Widgets.Quickpage.assign_item = function(self, index, item)
	if index > #self.buttons then return end
	-- Find the icon.
	local spec = Itemspec:find{name = item.name}
	local icon = Iconspec:find{name = item.icon} or Iconspec:find{name = "missing1"}
	-- Assign the item.
	self.buttons[index].feat = nil
	self.buttons[index].icon = icon
	self.buttons[index].item = item.name
	self.buttons[index].text = item.count and item.count > 1 and tostring(item.count)
	self.buttons[index].tooltip = Widgets.Itemtooltip{spec = spec}
	self:save()
end

--- Called when the quickslot is clicked or accessed by a hotkey.<br/>
-- If a quickslot is activated while dragging an item, the item is assigned
-- to the slot. Otherwise, the item or feat in the slot is activated.
-- @param self Quickpage class.
-- @param index Quickslot index number.
Widgets.Quickpage.activate = function(self, index)
	local feat = self.buttons[index].feat
	local item = self.buttons[index].item
	if feat then
		-- Activate a feat.
		local names = {}
		local values = {}
		for i = 1,3 do
			names[i] = feat.effects[i] and feat.effects[i][1] or ""
			values[i] = feat.effects[i] and feat.effects[i][2] or 0
		end
		Network:send{packet = Packet(packets.FEAT,
			"string", feat.animation or "",
			"string", names[1], "float", values[1],
			"string", names[2], "float", values[2],
			"string", names[3], "float", values[3],
			"bool", true)}
		Network:send{packet = Packet(packets.FEAT,
			"string", feat.animation or "",
			"string", names[1], "float", values[1],
			"string", names[2], "float", values[2],
			"string", names[3], "float", values[3],
			"bool", false)}
	elseif item then
		-- Use an item.
		local cont = Client.views.inventory.container
		if not cont then return end
		local slot = cont:find_item{name = item}
		if not slot then return end
		Commands:use(cont.id, slot)
	end
end

--- Loads feats from the database.
-- @param self Quickpage.
Widgets.Quickpage.load = function(self)
	-- Only load feats.
	if self.type ~= "feats" then return end
	self.db = Client.db
	self.db:query("CREATE TABLE IF NOT EXISTS quickslots (slot INTEGER PRIMARY KEY,item TEXT,feat TEXT);")
	self.loading = true
	-- First set defaults.
	self:assign_feat(1, Feat{animation = "spell on self", effects = {{"restore health", 10}}})
	self:assign_feat(2, Feat{animation = "ranged spell", effects = {{"fire damage", 10}}})
	self:assign_feat(3, Feat{animation = "spell on self", effects = {{"light", 1}}})
	-- Load from the database.
	local rows = self.db:query("SELECT slot,item,feat FROM quickslots;")
	for k,v in ipairs(rows) do
		if v[1] >= 1 and v[1] <= #self.buttons then
			if v[2] and v[2] ~= "" then
				self:assign_item(v[1], v[2])
			elseif v[3] and v[3] ~= "" then
				self:assign_feat(v[1], Feat:load{data = v[3]})
			else
				self:assign_none(v[1])
			end
		end
	end
	self.loading = nil
end

--- Saves feats to the database.
-- @param self Quickpage.
Widgets.Quickpage.save = function(self)
	if self.type ~= "feats" then return end
	if self.loading then return end
	for k,v in ipairs(self.buttons) do
		local feat = v.feat and v.feat:write()
		local item = v.item
		self.db:query("REPLACE INTO quickslots (slot,item,feat) VALUES (?,?,?);", {k, item, feat})
	end
end

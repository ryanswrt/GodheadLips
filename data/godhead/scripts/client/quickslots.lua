require "client/widgets/quickpage"

Quickslots = Class()
Quickslots.class_name = "Quickslots"

Quickslots.init = function(clss)
	clss.group = Widget{rows = 1, cols = 1, spacings = {0,0}}
	clss.items = Widgets.Quickpage{type = "items"}
	clss.feats = Widgets.Quickpage{type = "feats"}
	clss.mode = "items"
end

--- Assigns a feat to a quickslot.
-- @param self Quickslots class.
-- @param index Quickslot index number.
-- @param feat Feat to assign or nil.
Quickslots.assign_feat = function(clss, index, feat)
	if feat then
		clss.feats:assign_feat(index, feat)
	else
		clss.feats:assign_none(index)
	end
	clss.mode = "feats"
end

--- Clears the assignment of the quickslot.
-- @param clss Quickslots class.
-- @param index Quickslot index number.
Quickslots.assign_none = function(clss, index)
	clss.items:assign_none(index)
end

--- Assigns an item to a quickslot.
-- @param clss Quickslots class.
-- @param index Quickslot index number.
-- @param item Item to assign or nil.
Quickslots.assign_item = function(clss, index, item)
	if item then
		clss.items:assign_item(index, item)
	else
		clss.items:assign_none(index)
	end
end

--- Called when the quickslot is clicked or accessed by a hotkey.
-- @param clss Quickslots class.
-- @param index Quickslot index number.
Quickslots.activate = function(clss, index)
	if clss.mode == "feats" then
		clss.feats:activate(index)
	else
		clss.items:activate(index)
	end
end

--- Resets the quickslots to the original state.
-- @param clss Quickslots class.
Quickslots.reset = function(clss)
	for i = 1,10 do
		clss.items:assign_none(i)
	end
end

--- Saves the quickslots.
-- @param clss Quickslots class.
Quickslots.save = function(clss)
	clss.feats:save()
end

Quickslots.class_getters = {
	mode = function(self) return rawget(self, "_mode") end}

Quickslots.class_setters = {
	mode = function(self, v)
		if self.mode == v then return end
		rawset(self, "_mode", v)
		if v == "feats" then
			self.group:set_child(1, 1, self.feats)
		else
			self.group:set_child(1, 1, self.items)
		end
	end}

Quickslots:init()

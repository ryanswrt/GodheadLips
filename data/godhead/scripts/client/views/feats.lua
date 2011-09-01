require "client/quickslots"
require "client/widgets/spelleffect"

Views.Feats = Class(Widget)

--- Creates a new feat editor.
-- @param clss Feats class.
-- @return Feats.
Views.Feats.new = function(clss)
	local self = Widget.new(clss, {cols = 2, rows = 1, spacings = {0,0}})
	-- Title.
	self.title = Widgets.Title{text = "Spells",
		back = function() self:back() end,
		close = function() Client:set_mode("game") end,
		help = function() Client.views.help:show("spells") end}
	-- Animation selector.
	local label = Widgets.Label{text = "Type"}
	label:set_request{width = 46}
	self.toggle_anim_none = Widgets.Toggle{text = "None", active = true, pressed = function() self.anim_type = nil end}
	self.toggle_anim_ranged = Widgets.Toggle{text = "Ranged", pressed = function() self.anim_type = "ranged spell" end}
	self.toggle_anim_self = Widgets.Toggle{text = "Self", pressed = function() self.anim_type = "spell on self" end}
	self.toggle_anim_touch = Widgets.Toggle{text = "Touch", pressed = function() self.anim_type = "spell on touch" end}
	self.group_anim_toggles = Widget{cols = 4, rows = 1, homogeneous = true}
	self.group_anim_toggles:set_expand{col = 1}
	self.group_anim_toggles:set_expand{col = 2}
	self.group_anim_toggles:set_expand{col = 3}
	self.group_anim_toggles:set_expand{col = 4}
	self.group_anim_toggles:set_child(1, 1, self.toggle_anim_none)
	self.group_anim_toggles:set_child(2, 1, self.toggle_anim_ranged)
	self.group_anim_toggles:set_child(3, 1, self.toggle_anim_self)
	self.group_anim_toggles:set_child(4, 1, self.toggle_anim_touch)
	self.group_anim = Widget{cols = 2, rows = 1}
	self.group_anim:set_expand{col = 2}
	self.group_anim:set_child(1, 1, label)
	self.group_anim:set_child(2, 1, self.group_anim_toggles)
	-- Effect selectors.
	self.spell_effect = {}
	self.scroll_effect = {}
	for i = 1,3 do
		self.spell_effect[i] = Widgets.Spelleffect{active = true, visible = false, compact = true,
			tooltip = Widgets.Tooltip{text = "Click to remove from the spell"}, pressed = function()
			self.spell_effect[i].effect = nil
			self.spell_effect[i].visible = false
			self.scroll_effect[i].visible = false
			self:changed()
		end}
		self.spell_effect[i]:set_request{width = 150}
		self.scroll_effect[i] = Widgets.Progress{min = 0, max = 100, visible = false, value = 0, pressed = function(w)
			w.value = w:get_value_at(Program.cursor_position)
			self:changed()
		end}
		self.scroll_effect[i]:set_request{width = 150}
	end
	self.group_effect = Widget{cols = 2, rows = 3}
	self.group_effect:set_request{width = 310, height = 36 * 3 + 10}
	self.group_effect:set_expand{col = 1}
	self.group_effect:set_expand{col = 2}
	self.group_effect:set_child{col = 1, row = 1, widget = self.spell_effect[1]}
	self.group_effect:set_child{col = 1, row = 2, widget = self.spell_effect[2]}
	self.group_effect:set_child{col = 1, row = 3, widget = self.spell_effect[3]}
	self.group_effect:set_child{col = 2, row = 1, widget = self.scroll_effect[1]}
	self.group_effect:set_child{col = 2, row = 2, widget = self.scroll_effect[2]}
	self.group_effect:set_child{col = 2, row = 3, widget = self.scroll_effect[3]}
	-- Information display.
	self.label_skills = Widgets.Label{valign = 0, color = {0,0,0,1}}
	self.label_reagents = Widgets.Label{valign = 0, color = {0,0,0,1}}
	self.group_req = Widget{rows = 2, cols = 2}
	self.group_req:set_expand{col = 1, row = 2}
	self.group_req:set_expand{col = 2}
	self.group_req:set_child{row = 1, col = 1, widget = Widgets.Label{font = "medium", text = "Skills", color = {0,0,0,1}}}
	self.group_req:set_child{row = 1, col = 2, widget = Widgets.Label{font = "medium", text = "Reagents", color = {0,0,0,1}}}
	self.group_req:set_child{row = 2, col = 1, widget = self.label_skills}
	self.group_req:set_child{row = 2, col = 2, widget = self.label_reagents}
	self.group_info = Widgets.Frame{rows = 3, cols = 1, style = "paper"}
	self.group_info:set_request{height = 210}
	self.group_info:set_expand{col = 1, row = 1}
	self.group_info:set_child{row = 1, col = 1, widget = self.group_req}
	-- Quickslot selector.
	self.toggle_slot = {}
	self.group_slots = Widget{cols = 10, rows = 1, spacings = {0,0}}
	for i = 1,10 do
		self.toggle_slot[i] = Widgets.Toggle{text = tostring(i), pressed = function() self.active_slot = i end}
		self.group_slots:set_expand{col = i}
		self.group_slots:set_child(i, 1, self.toggle_slot[i])
	end
	local label1 = Widgets.Label{text = "Slot"}
	label1:set_request{width = 46}
	self.group_slot = Widget{cols = 2, rows = 1}
	self.group_slot:set_expand{col = 2}
	self.group_slot:set_child(1, 1, label1)
	self.group_slot:set_child(2, 1, self.group_slots)
	-- Assign button.
	self.button_assign = Widgets.Button{text = "Assign to the quickslot"}
	self.button_assign.pressed = function() self:assign() end
	-- Packing the spell pane.
	self.group = Widgets.Frame{cols = 1}
	self.group:append_row(self.group_slot)
	self.group:append_row(self.group_anim)
	self.group:append_row(self.group_effect)
	self.group:append_row(self.button_assign)
	self.group_spells = Widget{cols = 1, rows = 3, spacings = {0,0}}
	self.group_spells:set_child(1, 1, self.title)
	self.group_spells:set_child(1, 2, self.group)
	self.group_spells:set_child(1, 3, self.group_info)
	-- Effect pane.
	self.list_effects = Widgets.List{page_size = 10}
	self.title_effects = Widgets.Frame{style = "title", text = "Effects"}
	self.frame_effects = Widgets.Frame{style = "default", cols = 1, rows = 1}
	self.frame_effects:set_child(1, 1, self.list_effects)
	self.frame_effects:set_expand{col = 1, row = 1}
	self.group_effects = Widget{cols = 1, rows = 2, spacings = {0,0}}
	self.group_effects:set_expand{col = 1, row = 2}
	self.group_effects:set_child(1, 1, self.title_effects)
	self.group_effects:set_child(1, 2, self.frame_effects)
	-- Packing the dialog.
	self:set_child(1, 1, self.group_spells)
	self:set_child(2, 1, self.group_effects)
	-- Show the first feat.
	self:show(1)
	return self
end

Views.Feats.add_effect = function(self, effect)
	for j = 1,3 do
		local w = self.spell_effect[j]
		if not w.effect then
			w.effect = effect
			w.visible = true
			self.scroll_effect[j].visible = true
			self:changed()
			return
		end
	end
end

Views.Feats.assign = function(self)
	-- Get effects and their magnitudes.
	local effects = {}
	local values = {}
	for i = 1,3 do
		local e = self.spell_effect[i].effect
		if e then
			table.insert(effects, e.name)
			table.insert(values, self.scroll_effect[i].value)
		end
	end
	-- Create a feat from the animation and the effects.
	local anim = self.anim_type
	if anim and #effects > 0 then
		local feat = Feat{animation = anim, effects = {}}
		for i = 1,3 do
			feat.effects[i] = {effects[i], values[i]}
		end
		Quickslots:assign_feat(self.active_slot, feat)
	else
		Quickslots:assign_feat(self.active_slot)
	end
end

Views.Feats.back = function(self)
	Client:set_mode("menu")
end

--- Recalculates the skill and reagent requirements of the currently shown feat.
-- @param self Feats.
Views.Feats.changed = function(self)
	if self.protect then return end
	-- Get effects and their magnitudes.
	local effects = {}
	local values = {}
	local both = {}
	for i = 1,3 do
		local e = self.spell_effect[i].effect
		if e then
			table.insert(effects, e.name)
			table.insert(values, self.scroll_effect[i].value)
			table.insert(both, {e.name, self.scroll_effect[i].value})
		end
	end
	-- Calculate skill and reagent requirements.
	local feat = Feat{animation = self.anim_type, effects = both}
	local info = feat:get_info()
	if not info then
		self.label_skills.text = ""
		self.label_reagents.text = ""
		return
	end
	local reagents = info.required_reagents
	local skills = info.required_skills
	-- Display skill requirements.
	local skill_list = {}
	for k,v in pairs(skills) do
		table.insert(skill_list, k .. ": " .. v)
	end
	table.sort(skill_list)
	local skill_str = ""
	for k,v in ipairs(skill_list) do
		skill_str = skill_str .. (skill_str ~= "" and "\n" or "") .. v
	end
	self.label_skills.text = skill_str
	-- Display reagent requirements.
	local reagent_list = {}
	for k,v in pairs(reagents) do
		table.insert(reagent_list, k .. ": " .. v)
	end
	table.sort(reagent_list)
	local reagent_str = ""
	for k,v in ipairs(reagent_list) do
		reagent_str = reagent_str .. (reagent_str ~= "" and "\n" or "") .. v
	end
	self.label_reagents.text = reagent_str
end

--- Executes the feat editor.
-- @param self Chargen.
Views.Feats.enter = function(self, from, level)
	Quickslots.mode = "feats"
	Gui.menus:open{level = level, widget = self}
end

--- Sets the feat animation.
-- @param self Feats.
-- @param name Animation name.
Views.Feats.set_anim = function(self, name)
	local spec = self.spec
	local anim = Featanimspec:find{name = name}
	-- Clear the effect slots.
	for i = 1,3 do
		self.spell_effect[i].effect = nil
		self.spell_effect[i].visible = false
		self.scroll_effect[i].visible = false
	end
	-- Create the list of effects.
	local effects = {}
	if spec then
		for k in pairs(spec.feat_effects) do
			local e = Feateffectspec:find{name = k}
			if e and e.description then
				table.insert(effects, k)
			end
		end
		table.sort(effects)
	end
	-- Update the effect list.
	self.list_effects:clear()
	for k,v in ipairs(effects) do
		local a = anim and anim.effects[v]
		local w = Widgets.Spelleffect{
			active = a,
			effect = Feateffectspec:find{name = v},
			tooltip = a and Widgets.Tooltip{text = "Click to add to the spell"}, 
			pressed = function(w)
				if w.active then self:add_effect(w.effect) end
			end}
		self.list_effects:append{widget = w}
	end
end

--- Sets the race of the character using the feat editor.
-- @param self Feats.
-- @param name Race name.
Views.Feats.set_race = function(self, name)
	self.spec = Species:find{name = name}
	self:set_anim(self.anim_type)
end

--- Shows the feat for the given quickslot.
-- @param self Feats.
-- @param index Quickslot index.
Views.Feats.show = function(self, index)
	local feat = Quickslots.feats.buttons[index].feat or Feat()
	self.protect = true
	self.active_slot = index
	self.anim_type = feat.animation
	for j = 1,3 do
		local e = feat.effects[j]
		local s = e and Feateffectspec:find{name = e[1]}
		self.spell_effect[j].effect = s
		self.spell_effect[j].visible = s and true or false
		self.scroll_effect[j].value = s and e[2] or 0
		self.scroll_effect[j].visible = s and true or false
	end
	self.protect = nil
	self:changed()
end

Views.Feats:add_getters{
	active_slot = function(self)
		return rawget(self, "_active_slot")
	end,
	anim_type = function(self)
		if self.toggle_anim_self.active then
			return "spell on self"
		elseif self.toggle_anim_ranged.active then
			return "ranged spell"
		elseif self.toggle_anim_touch.active then
			return "spell on touch"
		end
	end}

Views.Feats:add_setters{
	active_slot = function(self, v)
		rawset(self, "_active_slot", v)
		for i = 1,10 do
			self.toggle_slot[i].active = (i == v)
		end
		if not self.protect then self:show(v) end
	end,
	anim_type = function(self, v)
		self.toggle_anim_none.active = (v == nil)
		self.toggle_anim_ranged.active = (v == "ranged spell")
		self.toggle_anim_self.active = (v == "spell on self")
		self.toggle_anim_touch.active = (v == "spell on touch")
		self:set_anim(v)
	end}

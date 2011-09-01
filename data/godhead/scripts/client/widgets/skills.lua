Widgets.Skills = Class(Widget)

--- Creates a new skills widget.
-- @param clss Skills widget class.
-- @param args Arguments.
-- @return Skills widget.
Widgets.Skills.new = function(clss, args)
	local self = Widget.new(clss, {cols = 1})
	self.changed = args and args.changed
	self.dict_id = {}
	self.dict_row = {}
	self:add("dexterity", "Dexterity", "Determines how fast you can move and how effective your ranged attacks are.")
	self:add("health", "Health", "Determines how much damage your can withstand.")
	self:add("intelligence", "Intelligence", "Determines what items you can craft and how effectively you can handle technology based items.")
	self:add("perception", "Perception", "Determines how far you can see and how well you can handle precision weapons and items.")
	self:add("strength", "Strength", "Determines your physical power and the effectiveness of your melee attacks.")
	self:add("willpower", "Willpower", "Determines what spells you can cast and how effective they are.")
	self.quota_text = Widgets.Label()
	self:append_row(self.quota_text)
	return self
end

--- Adds a skill.
-- @param self Skills widget.
-- @param id Skill id.
-- @param name Skill name.
-- @param desc Skill description.
Widgets.Skills.add = function(self, id, name, desc)
	local index = #self.dict_row + 1
	local skill = Widgets.SkillControl{
		cap = 0, desc = desc, id = id, name = name,
		index = index, max = 100, text = name, value = 0,
		changed = function(w, v)
			if self.species then v = math.min(v, self.species.skill_quota + w.value - self.total) end
			w.cap = v
			self:update_points()
			self:changed(w)
		end}
	self.dict_id[id] = skill
	self.dict_row[index] = skill
	self:append_row(skill)
end

--- Called when skill has changed.
-- @param self Skills widget.
-- @param widget Skill control widget.
Widgets.Skills.changed = function(self, widget)
end

--- Gets the value of a skill.
-- @param self Skills widget.
-- @param id Skill identifier string.
-- @return Skill value.
Widgets.Skills.get_value = function(self, id)
	return self.dict_id[id].value
end

--- Sets the species for which the skills are.
-- @param self Skills widget.
-- @param value Species.
Widgets.Skills.set_species = function(self, value)
	self.species = value
	for k,v in pairs(value.skills) do
		local w = self.dict_id[k]
		if w then
			w.max = v.max
			w.cap = v.val
			w.value = v.val
		end
	end
	self:update_points()
end

--- Updates the skill point display.
-- @param self Skills widget.
Widgets.Skills.update_points = function(self, value)
	if self.species then
		local t = math.ceil(self.total)
		local q = math.ceil(self.species.skill_quota)
		if t < q then
			self.quota_text.text = string.format(" Points assigned: %d/%d (%+d)", t, q, q - t)
			for k,v in pairs(self.dict_id) do v.allowance = q - t end
		else
			self.quota_text.text = string.format(" Points assigned: %d/%d", t, q)
			for k,v in pairs(self.dict_id) do v.allowance = 0 end
		end
	end
end

Widgets.Skills:add_getters{
	total = function(s)
		local t = 0
		for k,v in pairs(s.dict_id) do t = t + v.cap end
		return t
	end}

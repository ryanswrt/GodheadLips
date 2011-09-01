Views.Skills = Class(Widget)

--- Creates a new skills widget.
-- @param clss skills class.
-- @return Skills widget.
Views.Skills.new = function(clss)
	local self = Widget.new(clss, {cols = 1, rows = 2, spacings = {0,0}})
	-- Create widgets.
	self.title = Widgets.Title{text = "Skills",
		back = function() self:back() end,
		close = function() Client:set_mode("game") end,
		help = function() Client.views.help:show("skills") end}
	self.skills = Widgets.Skills()
	self.skills.changed = function(widget, skill)
		Network:send{packet = Packet(packets.PLAYER_SKILLS, "string", skill.id, "float", skill.cap)}
	end
	self.group = Widgets.Frame{cols = 1, rows = 1}
	self.group:set_expand{col = 1, row = 1}
	self.group:set_child{col = 1, row = 1, widget = self.skills}
	self:set_child{col = 1, row = 1, widget = self.title}
	self:set_child{col = 1, row = 2, widget = self.group}
	return self
end

Views.Skills.back = function(self)
	Client:set_mode("menu")
end

Views.Skills.enter = function(self, from, level)
	Gui.menus:open{level = level, widget = self}
end

--- Sets the species for which the skills are.
-- @param self Skills.
-- @param value Species.
Views.Skills.set_species = function(self, value)
	self.skills:set_species(value)
end

--- Updates a skill.
-- @param self Skills.
-- @param id Skill id.
-- @param value Current value.
-- @param cap Custom cap.
Views.Skills.update = function(self, id, value, cap)
	local skill = self.skills.dict_id[id]
	if skill then
		local species = Species:find{name = Player.species}
		local spec = species and species.skills[skill.id]
		skill.value = value
		skill.cap = cap
		skill.max = spec and spec.max or 100
		if id == "health" then
			Gui.skill_health.cap = cap
			Gui.skill_health.value = value
		elseif id == "willpower" then
			Gui.skill_mana.cap = cap
			Gui.skill_mana.value = value
		end
		self.skills:update_points()
	end
end

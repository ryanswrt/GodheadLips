require "client/widgets/text"

Widgets.QuestInfo = Class(Widgets.Text)

Widgets.QuestInfo.new = function(clss, args)
	local self = Widgets.Text.new(clss, args)
	self.text = {{"No quests"}}
	return self
end

Widgets.QuestInfo:add_setters{
	quest = function(self, value)
		self.text = {
			{value.name, "medium"},
			{value.status == "completed" and "Status: Completed" or "Status: Active"},
			{value.text}}
		self:build()
	end}

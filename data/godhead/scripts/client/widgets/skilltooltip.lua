require "client/widgets/tooltip"

Widgets.Skilltooltip = Class(Widgets.Tooltip)

-- Creates a skill tooltip.
-- @param clss Skilltooltip class.
-- @param args Arguments.<ul>
--   <li>desc: Skill description.</li>
--   <li>name: Skill name.</li></ul>
-- @return Skill tooltip.
Widgets.Skilltooltip.new = function(clss, args)
	local self = Widgets.Tooltip.new(clss)
	-- Icon and name.
	local title = Widget{cols = 2, rows = 1, spacings = {5,0}}
	local icon = nil -- TODO
	if icon then title:set_child{col = 1, row = 1, widget = Widgets.Icon{icon = icon}} end
	title:set_child{col = 2, row = 1, widget = Widgets.Label{font = "bigger", text = args.name}}
	self:append(title)
	-- Description.
	self:append(args.desc)
	-- Usage.
	self:append("Click to add or remove points from this skill.")
	return self
end

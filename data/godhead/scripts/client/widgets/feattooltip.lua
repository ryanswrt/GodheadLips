require "client/widgets/tooltip"

Widgets.Feattooltip = Class(Widgets.Tooltip)

Widgets.Feattooltip.new = function(clss, args)
	local self = Widgets.Tooltip.new(clss)
	local feat = args.feat
	if not feat then return self end
	local info = feat:get_info()
	if not info then return self end
	-- Icon and name.
	local title = Widget{cols = 2, rows = 1, spacings = {5,0}}
	local icon = feat:get_icon()
	if icon then title:set_child{col = 1, row = 1, widget = Widgets.Icon{icon = icon}} end
	title:set_child{col = 2, row = 1, widget = Widgets.Label{font = "bigger", text = feat.animation}}
	self:append(title)
	-- Influences.
	if info.influences then
		local first = true
		for k,v in pairs(info.influences) do
			if first then
				self:append(string.format("Powers:"))
				first = false
			end
			local units = {
				plague = {"bearers"},
				fire = {"healing", "damage"},
				physical = {"healing", "damage"},
				sanctuary = {"seconds"}}
			local unit = units[k] and (v > 0 and units[k][1] or units[k][2])
			self:append(string.format("  %s: %d %s", k, math.abs(v), unit or ""))
		end
	end
	-- Reagents.
	if info.required_reagents then
		local first = true
		for k,v in pairs(info.required_reagents) do
			if first then
				self:append(string.format("Reagents required:"))
				first = false
			end
			self:append(string.format("  %s: %d", k, v))
		end
	end
	return self
end

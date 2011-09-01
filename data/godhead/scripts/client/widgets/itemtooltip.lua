require "client/widgets/tooltip"

Widgets.Itemtooltip = Class(Widgets.Tooltip)

Widgets.Itemtooltip.new = function(clss, args)
	local self = Widgets.Tooltip.new(clss)
	local spec = args.spec
	if not spec then return self end
	-- Icon and name.
	local title = Widget{cols = 2, rows = 1, spacings = {5,0}}
	local icon = Iconspec:find{name = spec.icon}
	if icon then title:set_child{col = 1, row = 1, widget = Widgets.Icon{icon = icon}} end
	title:set_child{col = 2, row = 1, widget = Widgets.Label{font = "bigger", text = spec.name}}
	self:append(title)
	-- Armor.
	local count = args.count or 1
	if spec.armor_class > 0 then
		self:append("Armor: " .. tostring(100 * spec.armor_class))
	end
	-- Count.
	if spec.stacking then
		if args.count_drag then
			self:append(string.format("Count: %d/%d", args.count_drag, count))
		else
			self:append(string.format("Count: %d", count))
		end
	end
	-- Influences.
	if spec.influences_base then
		local first = true
		for k,v in pairs(spec.influences_base) do
			if first then
				self:append(string.format("Powers:"))
				first = false
			end
			if v < 0 then
				self:append(string.format("  %s: +%d damage", k, -v))
			else
				self:append(string.format("  %s: +%d healing", k, v))
			end
		end
	end
	-- Materials.
	if spec.crafting_materials then
		local first = true
		for k,v in pairs(spec.crafting_materials) do
			if first then
				self:append(string.format("Crafting materials:"))
				first = false
			end
			self:append(string.format("  %s: %d", k, v))
		end
	end
	-- Weight.
	self:append("Weight: " .. tostring(spec.mass_inventory * count))
	-- Description.
	if spec.description then
		self:append(spec.description)
	end
	return self
end

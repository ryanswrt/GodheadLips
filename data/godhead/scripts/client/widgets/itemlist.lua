Widgets.ItemList = Class(Widget)

--- Creates an item list widget.
-- @param clss ItemList class.
-- @param args Arguments.<ul>
--   <li>activated: Activation callback.</li>
--   <li>cols: Number of colums.</li>
--   <li>size: Total number of slots.</li></ul>
-- @return Item list widget.
Widgets.ItemList.new = function(clss, args)
	local self = Widget.new(clss)
	local copy = function(f, d) self[f] = (args and args[f] ~= nil) and args[f] or d end
	self.rows = 0
	self.spacings = {0, 0}
	copy("activated")
	copy("cols", 5)
	copy("size", 10)
	return self
end

Widgets.ItemList.activated = function(self, row, args)
end

Widgets.ItemList.get_item = function(self, args)
	local widget = self.buttons[args.slot]
	if not widget then return end
	if not widget.text then return end
	return widget
end

Widgets.ItemList.set_item = function(self, args)
	local widget = self.buttons[args.slot]
	if not widget then return end
	widget.text = args.name
	widget.count = args.count
	widget.icon = args.icon
end

Widgets.ItemList:add_getters{
	size = function(s) return rawget(s, "__size") end}

Widgets.ItemList:add_setters{
	size = function(s, v)
		rawset(s, "__size", v)
		s.rows = math.ceil(v / s.cols)
		s.buttons = {}
		for i = 1,v do
			s.buttons[i] = Widgets.ItemButton{pressed = function(w, a) s:activated(i, a) end}
			s:set_child(
				1 + (i - 1) % s.cols,
				1 + math.floor((i - 1) / s.cols),
				s.buttons[i])
		end
	end}

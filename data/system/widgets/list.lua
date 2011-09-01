require "system/widgets/scrollbar"

Widgets.List = Class(Widgets.Frame)

--- Creates a new list widget.
-- @param clss List widget class.
-- @param args Arguments.
Widgets.List.new = function(clss, args)
	local self = Widgets.Frame.new(clss, {rows = 1, cols = 2, spacings = {0,0}, style = "list"})
	self.table = {}
	self.list = Widget{cols = 1, spacings = {0,0}}
	self.list:set_expand{col = 1}
	self.scroll = Widgets.Scrollbar{changed = function(w, p) self:set_offset(p) end}
	self.scroll:set_range(0, 1, 3)
	if args and args.page_size then self.page_size = args.page_size end
	self:set_child(1, 1, self.list)
	self:set_child(2, 1, self.scroll)
	self:set_expand{col = 1, row = 1}
	return self
end

--- Appends a widget to the list.
-- @param self List widget.
-- @param args Arguments.<ul>
--   <li>widget: Widget to append.</li></ul>
Widgets.List.append = function(self, args)
	local row = #self.table + 1
	table.insert(self.table, args.widget)
	if row <= self.scroll.page then
		self.list:append_row(args.widget)
	end
	self.scroll:set_range(#self.table)
end

--- Clear all widgets from the list.
-- @param self List widget.
Widgets.List.clear = function(self)
	self.table = {}
	self.list.rows = 0
	self.scroll:set_range(#self.table)
end

--- Gets the widget in the specific row.
-- @param self List widget.
Widgets.List.get_row = function(self, args)
	local child = self.table[args.row]
	if not child then return end
	return child.text
end

Widgets.List.pressed = function(self, row)
end

--- Gets the scroll offset of the widget.
-- @param self List widget.
-- @return Scroll offset in rows.
Widgets.List.get_offset = function(self)
	return self.scroll.offset
end

Widgets.List.scrolled = function(self, args)
	return self.scroll:scrolled(args)
end

--- Scrolls the list widget.
-- @param self List widget.
-- @param offset Scroll offset in rows.
Widgets.List.set_offset = function(self, offset)
	-- Update the scrollbar.
	if #self.table <= self.scroll.page then return end
	self.scroll:set_range(#self.table, offset)
	-- Repack the children.
	for i = 1,self.scroll.max do
		self.list:set_child(1, i, self.table[offset + i])
	end
end

Widgets.List:add_getters{
	page_size = function(self) return self.scroll.page end}

Widgets.List:add_setters{
	page_size = function(self, value)
		self.scroll:set_range(#self.table, nil, value)
	end}

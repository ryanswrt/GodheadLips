require "system/class"

if not Los.program_load_extension("widgets") then
	error("loading extension `widgets' failed")
end

------------------------------------------------------------------------------

Widget = Class()
Widget.class_name = "Widget"

--- Creates a new widget.
-- @param clss Widget class.
-- @param args Arguments.
-- @return New widget.
Widget.new = function(clss, args)
	local self = Class.new(clss)
	self.handle = Los.widget_new()
	self.__children = {}
	__userdata_lookup[self.handle] = self
	if args then
		for k,v in pairs(args) do self[k] = v end
	end
	return self
end

--- Adds a child widget.
-- @param self Widget.
-- @param widget Widget.
Widget.add_child = function(self, widget)
	Los.widget_add_child(self.handle, widget.handle)
	self.__children[widget] = widget
end

--- Appends a column to the widget.
-- @param self Widget.
-- @param args List of widgets.
Widget.append_col = function(self, ...)
	-- Append a list of handles.
	local h = {...}
	for k,v in pairs(h) do h[k] = h[k] and h[k].handle end
	Los.widget_append_col(self.handle, h)
	-- Reference children.
	local w = {...}
	local x = self.cols
	for y=1,self.rows do
		self.__children[x + y * 1000] = w[y]
	end
end

--- Appends a row to the widget.
-- @param self Widget.
-- @param args List of widgets.
Widget.append_row = function(self, ...)
	-- Append a list of handles.
	local h = {...}
	for k,v in pairs(h) do h[k] = h[k] and h[k].handle end
	Los.widget_append_row(self.handle, h)
	-- Reference children.
	local w = {...}
	local y = self.rows
	for x=1,self.cols do
		self.__children[x + y * 1000] = w[x]
	end
end

--- Clears the canvas of the widget.
-- @param self Widget.
Widget.canvas_clear = function(self)
	Los.widget_canvas_clear(self.handle)
end

--- Compiles the canvas of the widget.
-- @param self Widget.
Widget.canvas_compile = function(self)
	Los.widget_canvas_compile(self.handle)
end

--- Packs an image to the canvas of the widget.
-- @param self Widget.
-- @param args Arguments.<ul>
--   <li>color: RGBA color or nil.</li>
--   <li>dest_clip: {x,y,w,h} or nil.</li>
--   <li>dest_position: {x,y} or nil.</li>
--   <li>dest_size: {w,h} or nil.</li>
--   <li>rotation: Rotation angle in radians or nil.</li>
--   <li>rotation_center: Rotation center vector or nil.</li>
--   <li>source_position: {x,y} or nil.</li>
--   <li>source_tiling: {x1,x2,x3,y1,y2,y3} or nil.</li></ul>
Widget.canvas_image = function(self, args)
	local a = {}
	for k,v in pairs(args) do a[k] = v end
	if a.rotation_center then a.rotation_center = a.rotation_center.handle end
	Los.widget_canvas_image(self.handle, a)
end

--- Packs text to the canvas of the widget.
-- @param self Widget.
-- @param args Arguments.<ul>
--   <li>dest_clip: {x,y,w,h} or nil.</li>
--   <li>dest_position: {x,y} or nil.</li>
--   <li>dest_size: {w,h} or nil.</li>
--   <li>rotation: Rotation angle in radians or nil.</li>
--   <li>rotation_center: Rotation center vector or nil.</li>
--   <li>text: String.</li>
--   <li>text_alignment: {x,y} or nil.</li>
--   <li>text_color: {r,g,b,a} or nil.</li>
--   <li>text_font: Font name or nil.</li></ul>
Widget.canvas_text = function(self, args)
	local a = {}
	for k,v in pairs(args) do a[k] = v end
	if a.rotation_center then a.rotation_center = a.rotation_center.handle end
	Los.widget_canvas_text(self.handle, a)
end

--- Removes the widget from its parent.
-- @param self Widget.
Widget.detach = function(self)
	-- Remove from the script parent.
	local p = self.parent
	if p then
		for k,v in pairs(p.__children) do
			if v == self then
				p[k] = nil
				break
			end
		end
	end
	-- Remove from the internal parent.
	Los.widget_detach(self.handle)
end

--- Gets a child widget.
-- @param self Widget.
-- @param ... Arguments.<ul>
--   <li>1,row: Row number.</li>
--   <li>2,col: Column number.</li></ul>
-- @return Widget or nil.
Widget.get_child = function(self, ...)
	local a,b = ...
	if type(a) == "table" then
		local x = a[1] or a.col
		local y = a[2] or a.row
		return self.__children[x + y * 1000]
	else
		return self.__children[a + b * 1000]
	end
end

--- Gets the size request of the widget.
-- @param self Widget.
-- @return Request width, request height.
Widget.get_request = function(self)
	return Los.widget_get_request(self.handle)
end

--- Inserts a column to the widget.
-- @param self Widget.
-- @param ... Arguments.<ul>
--   <li>1,col: Column index or nil.</li>
--   <li>2,widget: Widget or nil.</li></ul>
Widget.insert_col = function(self, ...)
	local a = {...}
	for k,v in pairs(a) do
		if v ~= nil then a[k] = v.handle end
	end
	Los.widget_insert_col(self.handle, a)
end

--- Inserts a row to the widget.
-- @param self Widget.
-- @param ... Arguments.<ul>
--   <li>1,row: Row index or nil.</li>
--   <li>2,widget: Widget or nil.</li></ul>
Widget.insert_row = function(self, ...)
	local a = {...}
	for k,v in pairs(a) do
		if v ~= nil then a[k] = v.handle end
	end
	Los.widget_insert_row(self.handle, a)
end

--- Pops up the widget.
-- @param self Widget.
-- @param args Arguments.<ul>
--   <li>x: X coordinate.</li>
--   <li>y: Y coordinate.</li>
--   <li>width: Width allocation.</li>
--   <li>height: Height allocation.</li>
--   <li>dir: Popup direction. ("left"/"right"/"up"/"down")</li></ul>
Widget.popup = function(self, args)
	if Widgets.popup then
		Widgets.popup.visible = false
	end
	Widgets.popup = self
	return Los.widget_popup(self.handle, args)
end

--- Removes a row or a column from the widget.
-- @param self Widget.
-- @param args Arguments.<ul>
--   <li>col: Column index.</li>
--   <li>row: Row index.</li></ul>
Widget.remove = function(self, args)
	if args.col then
		local w,h = self.cols,self.rows
		for x = args.col,w do
			for y = 1,h do
				local c = self.__children[(x + 1) + y * 1000]
				self.__children[x + y * 1000] = c
			end
		end
		for y = 1,h do
			self.__children[w + y * 1000] = nil
		end
	end
	if args.row then
		local w,h = self.cols,self.rows
		for x = 1,w do
			for y = args.row,h do
				local c = self.__children[x + (y + 1) * 1000]
				self.__children[x + y * 1000] = c
			end
		end
		for x = 1,w do
			self.__children[x + h * 1000] = nil
		end
	end
	Los.widget_remove(self.handle, args)
end

--- Sets a child widget.
-- @param self Widget.
-- @param ... Arguments.<ul>
--   <li>1,col: Column number.</li>
--   <li>2,row: Row number.</li>
--   <li>3,widget: Widget.</li></ul>
Widget.set_child = function(self, ...)
	local a,b,c = ...
	if type(a) == "table" then
		local x = a[1] or a.col
		local y = a[2] or a.row
		local w = a[3] or a.widget
		self.__children[x + y * 1000] = w
		Los.widget_set_child(self.handle, x, y, w and w.handle)
	else
		self.__children[a + b * 1000] = c
		Los.widget_set_child(self.handle, a, b, c and c.handle)
	end
end

--- Enables or disables row or column expansion.
-- @param self Widget.
-- @param ... Arguments.<ul>
--   <li>1,row: Row number.</li>
--   <li>2,col: Column number.</li>
--   <li>3,expand: Boolean.</li></ul>
Widget.set_expand = function(self, ...)
	Los.widget_set_expand(self.handle, ...)
end

--- Sets the size request of the widget.
-- @param self Widget.
-- @param args Arguments.<ul>
--   <li>1,width: Width request.</li>
--   <li>2,height: Height request.</li>
--   <li>font: Font to use when calculating from text.</li>
--   <li>internal: Set the internal request instead of the user request.</li>
--   <li>paddings: Additional paddings to add or nil.</li>
--   <li>text: Text to use when calculating from text.</li></ul>
Widget.set_request = function(self, ...)
	Los.widget_set_request(self.handle, ...)
end

--- Behind flag.
-- @name Widget.behind
-- @class table

--- Number of columns in the widget.
-- @name Widget.cols
-- @class table

--- Floating flag.
-- @name Widget.floating
-- @class table

--- Fullscreen flag.
-- @name Widget.fullscreen
-- @class table

--- Height of the widget.
-- @name Widget.height
-- @class table

--- Margin widths.
-- @name Widget.margins
-- @class table

--- Pixel offset relative to the parent.
-- @name Widget.offset
-- @class table

--- The parent of this widget.
-- @name Widget.parent
-- @class table

--- Number of rows in the widget.
-- @name Widget.rows
-- @class table

--- Child spacings.
-- @name Widget.margins
-- @class table

--- Visibility flag.
-- @name Widget.visible
-- @class table

--- Width of the widget
-- @name Widget.width
-- @class table

--- Left edge position.
-- @name Widget.x
-- @class table

--- Top edge position.
-- @name Widget.y
-- @class table

Widget:add_getters{
	behind = function(s) return Los.widget_get_behind(s.handle) end,
	cols = function(s) return Los.widget_get_cols(s.handle) end,
	floating = function(s) return Los.widget_get_floating(s.handle) end,
	fullscreen = function(s) return Los.widget_get_fullscreen(s.handle) end,
	height = function(s) return Los.widget_get_height(s.handle) end,
	height_request = function(s)
		local a,b = Los.widget_get_request(s.handle)
		return b
	end,
	margins = function(s) return Los.widget_get_margins(s.handle) end,
	offset = function(s) return Class.new(Vector, {handle = Los.widget_get_offset(s.handle)}) end,
	parent = function(s)
		local handle = Los.widget_get_parent(s.handle)
		if not handle then return end
		return __userdata_lookup[handle]
	end,
	request = function(s)
		local a,b = Los.widget_get_request(s.handle)
		return {a, b}
	end,
	rows = function(s) return Los.widget_get_rows(s.handle) end,
	spacings = function(s) return Los.widget_get_spacings(s.handle) end,
	visible = function(s) return Los.widget_get_visible(s.handle) end,
	width = function(s) return Los.widget_get_width(s.handle) end,
	width_request = function(s)
		local a,b = Los.widget_get_request(s.handle)
		return a
	end,
	x = function(s) return Los.widget_get_x(s.handle) end,
	y = function(s) return Los.widget_get_y(s.handle) end}

Widget:add_setters{
	behind = function(s, v) Los.widget_set_behind(s.handle, v) end,
	cols = function(s, v)
		local w,h = s.cols,s.rows
		if v < w then
			for x = v+1,w do
				for y = 1,h do
					s.__children[x + y * 1000] = nil
				end
			end
		end
		Los.widget_set_cols(s.handle, v)
	end,
	expand_col = function(s, v) Los.widget_set_expand(s.handle, v) end,
	expand_row = function(s, v) Los.widget_set_expand(s.handle, nil, v) end,
	floating = function(s, v)
		Los.widget_set_floating(s.handle, v)
		if not v and s == Widgets.popup then Widgets.popup = nil end
	end,
	fullscreen = function(s, v) Los.widget_set_fullscreen(s.handle, v) end,
	height_request = function(s, v) Los.widget_set_request(s.handle, s.width_request, v) end,
	margins = function(s, v) Los.widget_set_margins(s.handle, v) end,
	offset = function(s, v) Los.widget_set_offset(s.handle, v.handle) end,
	request = function(s, v)
		if v and v.class_name == "Vector" then
			Los.widget_set_request(s.handle, v.handle)
		else
			Los.widget_set_request(s.handle, v)
		end
	end,
	rows = function(s, v)
		local w,h = s.cols,s.rows
		if v < h then
			for x = 1,w do
				for y = v+1,h do
					s.__children[x + y * 1000] = nil
				end
			end
		end
		Los.widget_set_rows(s.handle, v)
	end,
	spacings = function(s, v) Los.widget_set_spacings(s.handle, v) end,
	visible = function(s, v)
		Los.widget_set_visible(s.handle, v)
		if not v and s == Widgets.popup then Widgets.popup = nil end
	end,
	width_request = function(s, v) Los.widget_set_request(s.handle, v, s.height_request) end,
	x = function(s, v) Los.widget_set_x(s.handle, v) end,
	y = function(s, v) Los.widget_set_y(s.handle, v) end}

Widget.unittest = function()
	-- Initialization.
	local w = Widget{cols = 1, rows = 1}
	assert(w.cols == 1)
	assert(w.rows == 1)
	-- Getters.
	assert(type(w.height) == "number")
	assert(type(w.width) == "number")
	-- Child packing.
	local c = Widget()
	w:set_child(1, 1, c)
	collectgarbage()
	assert(w:get_child(1, 1) == c)
	assert(Los.widget_get_child(w.handle, 1, 1) == c.handle)
	local c1 = Widget()
	local c2 = Widget()
	local c3 = Widget()
	w:append_col(c1)
	w:append_row(c2, c3)
	collectgarbage()
	assert(w:get_child(2, 1) == c1)
	assert(w:get_child(1, 2) == c2)
	assert(w:get_child(2, 2) == c3)
	-- Row and column removal.
	w:remove{col = 1}
	assert(w.cols == 1)
	assert(w:get_child(1, 1) == c1)
	assert(w:get_child(1, 2) == c3)
	w:remove{col = 1}
	assert(w.cols == 0)
	w:remove{row = 1}
	assert(w.rows == 1)
	w:remove{row = 1}
	assert(w.rows == 0)
	-- Shrinking.
	do
		local w1 = Widget{cols = 2, rows = 2}
		local weak = setmetatable({}, {__mode = "kv"})
		do
			weak[1] = Widget()
			weak[2] = Widget()
			weak[3] = Widget()
			weak[4] = Widget()
			w1:set_child(1, 1, weak[1])
			w1:set_child(2, 1, weak[2])
			w1:set_child(1, 2, weak[3])
			w1:set_child(2, 2, weak[4])
			collectgarbage()
			assert(weak[1])
		end
		w1.rows = 1
		assert(w1:get_child(1, 1))
		assert(w1:get_child(2, 1))
		assert(w1:get_child(1, 2) == nil)
		assert(w1:get_child(2, 2) == nil)
		collectgarbage()
		assert(weak[1])
		assert(weak[2])
		assert(weak[3] == nil)
		assert(weak[4] == nil)
		w1.cols = 1
		assert(w1:get_child(1, 1))
		assert(w1:get_child(2, 2) == nil)
		collectgarbage()
		assert(weak[1])
		assert(weak[2] == nil)
		w1:remove{col = 1}
		assert(w1.cols == 0)
		assert(w1:get_child(1, 1) == nil)
		collectgarbage()
		assert(weak[1] == nil)
	end
	-- Visibility.
	assert(w.visible)
	assert(not w.floating)
	w.floating = true
	assert(w.floating)
	assert(w.visible)
	w.visible = false
	assert(not w.floating)
	assert(not w.visible)
	-- Inheritance of new classes.
	local Label = Class(Widget)
	Label.class_name = "Label"
	Label.new = function(clss, args)
		local self = Widget.new(clss, args)
		self.font = self.font or "default"
		self.text = self.text or ""
		self.halign = self.halign or 0
		self.valign = self.valign or 0.5
		return self
	end
	Label:add_getters{text = function(s) return rawget(s, "__text") end}
	Label:add_setters{text = function(s, v) rawset(s, "__text", v) end}
	-- Instantiating inherited classes.
	local lab = Label{text = "test"}
	assert(lab.text == "test")
	assert(rawget(lab, "__text"))
	assert(not rawget(lab, "text"))
	assert(lab.visible)
	lab.visible = false
	assert(not lab.visible)
	-- Inheritance of accessors.
	local Button = Class(Label)
	Button:add_getters{text = function(s) return Label.getters.text(s) .. "Y" end}
	Button:add_setters{text = function(s, v) return Label.setters.text(s, v .. "X") end}
	local but = Button{text = "test"}
	assert(but.text == "testXYXY") -- Called twice in initializers.
	assert(rawget(but, "__text"))
	assert(not rawget(but, "text"))
end

-- Userdata don't have members but the engine needs to be able to call the
-- render functions of widgets. This is done with a global variable.
__widget_render = function(handle)
	local w = __userdata_lookup[handle]
	if w and w.render then w:render() end
end
__widget_reshape = function(handle)
	local w = __userdata_lookup[handle]
	if w and w.reshaped then w:reshaped() end
end

------------------------------------------------------------------------------

Widgets = Class()
Widgets.class_name = "Widgets"

--- Adds a font style.
-- @param clss Widgets class.
-- @param ... Arguments.<ul>
--   <li>1,name: Font name.</li>
--   <li>2,file: Font file.</li>
--   <li>3,size: Font size.</li></ul>
Widgets.add_font_style = function(clss, ...)
	Los.widgets_add_font_style(...)
end

--- Draws the user interface.
-- @param clss Widgets class.
Widgets.draw = function(clss)
	Los.widgets_draw()
end

--- Finds a widget by screen position.
-- @param clss Widgets class.
-- @param args Arguments.<ul>
--   <li>1,point: Screen position.</li></ul>
Widgets.find_widget = function(clss, args)
	local handle = Los.widgets_find_widget(args)
	if not handle then return end
	return __userdata_lookup[handle]
end

--- Currently focused widget.
-- @name Widgets.focused_widget
-- @class table
Widgets.class_getters = {
	focused_widget = function(s, k)
		local handle = Los.widgets_get_focused_widget()
		if not handle then return end
		return __userdata_lookup[handle]
	end}

------------------------------------------------------------------------------

require "system/widgets/button"
require "system/widgets/combo"
require "system/widgets/cursor"
require "system/widgets/entry"
require "system/widgets/frame"
require "system/widgets/list"
require "system/widgets/menuitem"
require "system/widgets/scrollbar"
require "system/widgets/title"
require "system/widgets/toggle"
require "system/widgets/widgets"

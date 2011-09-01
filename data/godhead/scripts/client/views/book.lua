Views.Book = Class(Widgets.Frame)
Views.Book.class_name = "Views.Book"

--- Initializes the book UI.
-- @param self Book view class.
-- @return Book view.
Views.Book.new = function(clss)
	local self = Widgets.Frame.new(clss, {cols = 2, rows = 4, style = {
		width = 800, height = 600, margins = {65,65,40,40}, spacings = {50,0},
		source_image = "book1", source_tiling = {0,800,0,0,600,0}}})
	-- Text.
	self.title = Widgets.Label{color = {0,0,0,1}, font = "bigger"}
	self.text1 = Widgets.Label{color = {0,0,0,1}, valign = 0, width_request = 305}
	self.text2 = Widgets.Label{color = {0,0,0,1}, valign = 0, width_request = 305}
	-- Page numbers.
	self.button_close = Widgets.Button{text = "X", pressed = function() self:back() end}
	self.label_page1 = Widgets.Label{color = {0,0,0,1}, halign = 0.5}
	self.label_page2 = Widgets.Label{color = {0,0,0,1}, halign = 0.55}
	self.button_prev = Widgets.Button{text = "<", pressed = function() self:show_page(self.page - 2) end}
	self.button_next = Widgets.Button{text = ">", pressed = function() self:show_page(self.page + 2) end}
	-- Packing.
	self.group1 = Widget{cols = 2, rows = 1}
	self.group1:set_expand{col = 2}
	self.group1:set_child(1, 1, self.button_prev)
	self.group1:set_child(2, 1, self.label_page1)
	self.group2 = Widget{cols = 3, rows = 1}
	self.group2:set_expand{col = 1}
	self.group2:set_child(1, 1, self.label_page2)
	self.group2:set_child(2, 1, self.button_close)
	self.group2:set_child(3, 1, self.button_next)
	self:set_expand{row = 3}
	self:set_child(1, 1, self.title)
	self:set_child(1, 2, self.text1)
	self:set_child(2, 2, self.text2)
	self:set_child(1, 4, self.group1)
	self:set_child(2, 4, self.group2)
	return self
end

--- Closes the book view.
-- @param self Book view.
Views.Book.back = function(self)
	Client:set_mode("game")
end

--- Closes the book view.
-- @param self Book view.
Views.Book.close = function(self)
end

Views.Book.enter = function(self, from, level)
	Gui.menus:open{level = level, widget = self}
end

--- Shows the book UI.
-- @param self Book view.
-- @param args Arguments.<ul>
--   <li>text: Content string.</li>
--   <li>title: Title string.</li></ul>
Views.Book.show = function(self, args)
	local split = function(str, pat)
		local t = {}
		local fpat = "(.-)" .. pat
		local last_end = 1
		local s,e,cap = str:find(fpat, 1)
		while s do
			if s ~= 1 or cap ~= "" then
				table.insert(t,cap)
			end
			last_end = e+1
			s,e,cap = str:find(fpat, last_end)
		end
		if last_end <= #str then
			cap = str:sub(last_end)
			table.insert(t, cap)
		end
		return t
	end
	self.page = 1
	self.pages = split(args.text, "\n\n\n")
	self.title.text = args.title
	self.text1.text = ""
	self.text2.text = ""
	self:show_page(self.page)
end

--- Shows a page of the book.
-- @param self Book view.
-- @param page Page number.
Views.Book.show_page = function(self, page)
	if page < 1 then return end
	if page > #self.pages then return end
	-- Update text.
	self.page = page
	self.text1.text = self.pages[page]
	self.text2.text = self.pages[page + 1] or ""
	-- Update page numbers.
	local n = #self.pages + (#self.pages % 2)
	self.label_page1.text = string.format("%d/%d", self.page, n)
	self.label_page2.text = string.format("%d/%d", self.page + 1, n)
end

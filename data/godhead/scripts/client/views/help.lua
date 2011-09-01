Views.Help = Class(Widgets.Frame)
Views.Help.class_name = "Views.Help"
Views.Help.index = {}

-- Load the manual pages.
for k,v in pairs(File:scan_directory("scripts/client/help")) do
	require("client/help/" .. string.gsub(v, "([^.]*).*", "%1"))
end

--- Initializes the help UI.
-- @param self Help view class.
-- @return Help view.
Views.Help.new = function(clss)
	local self = Widgets.Frame.new(clss, {cols = 2, rows = 4, style = {
		width = 800, height = 600, margins = {65,65,40,40}, spacings = {50,0},
		source_image = "book1", source_tiling = {0,800,0,0,600,0}}})
	-- Text.
	self.title = Widgets.Label{color = {0,0,0,1}, font = "bigger"}
	self.text1 = Widgets.Markup{width_request = 305}
	self.text2 = Widgets.Markup{width_request = 305}
	-- Page numbers.
	self.button_close = Widgets.Button{text = "X", pressed = function() self:back() end}
	self.button_index = Widgets.Button{text = "I", pressed = function() self:show("index") end}
	self.label_page1 = Widgets.Label{color = {0,0,0,1}, halign = 0.5}
	self.label_page2 = Widgets.Label{color = {0,0,0,1}, halign = 0.55}
	self.button_prev = Widgets.Button{text = "<", pressed = function() self:show_page(self.page - 2) end}
	self.button_next = Widgets.Button{text = ">", pressed = function() self:show_page(self.page + 2) end}
	-- Packing.
	self.group1 = Widget{cols = 3, rows = 1}
	self.group1:set_expand{col = 3}
	self.group1:set_child(1, 1, self.button_prev)
	self.group1:set_child(2, 1, self.button_index)
	self.group1:set_child(3, 1, self.label_page1)
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
-- @param self Help view.
Views.Help.back = function(self)
	Client:set_mode(self.return_mode or "game")
end

--- Closes the book view.
-- @param self Help view.
Views.Help.close = function(self)
	self.return_mode = nil
	self.mode = nil
end

Views.Help.enter = function(self, from, level)
	-- Store the source page.
	if from ~= "help" then
		self.return_mode = from
	end
	-- Open the desired pages.
	-- If entered in context sensitive help mode, show() has already been called
	-- to setup the contents. This is marked by the mode being set to "context".
	if self.mode ~= "context" then
		self.mode = "standalone"
		self:show("index")
	end
	-- Show the UI.
	Gui.menus:open{level = level, widget = self}
end

--- Shows the help UI.
-- @param self Help view.
-- @param topic Help topic string.
Views.Help.show = function(self, topic)
	-- Find the help text.
	local help = self.index[topic]
	local title = help and help.title or "Manual: Missing"
	local text = help and help.text or {"text", "Under construction"}
	-- Parse the help text.
	local t = {}
	self.pages = {}
	for k,v in ipairs(text) do
		if v[1] == "pagebreak" then
			table.insert(self.pages, t)
			t = {}
		else
			table.insert(t, v)
		end
	end
	if #t then
		table.insert(self.pages, t)
	end
	-- Show the first page.
	self.page = 1
	self.title.text = title
	self.text1.text = nil
	self.text2.text = nil
	self:show_page(self.page)
	-- Handle context sensitive initialization.
	-- We need to explicitly set the client to the help mode if called by the
	-- context sensitive help system since it only calls show().
	if self.mode ~= "standalone" then
		self.mode = "context"
		Client:set_mode("help")
	end
end

--- Shows a page of the active manual.
-- @param self Help view.
-- @param page Page number.
Views.Help.show_page = function(self, page)
	if page < 1 then return end
	if page > #self.pages then return end
	-- Update text.
	self.page = page
	self.text1.text = self.pages[page]
	self.text1:build()
	self.text2.text = self.pages[page + 1]
	self.text2:build()
	-- Update page numbers.
	local n = #self.pages + (#self.pages % 2)
	self.label_page1.text = string.format("%d/%d", self.page, n)
	self.label_page2.text = string.format("%d/%d", self.page + 1, n)
end

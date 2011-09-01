require "system/widgets/frame"

Widgets.Menu = Class(Widget)

Widgets.Menu.new = function(clss, args)
	local self = Widget.new(clss, {cols = 1, rows = 2, spacings = {0,0}})
	self.title = Widgets.Title{text = args and args.text,
		back = function() Gui.menus:back() end,
		close = function() Client:set_mode("game") end,
		help = function() Client.views.help:show("menu") end}
	self.frame = Widgets.Frame{style = "paper", cols = 1}
	self:set_child{col = 1, row = 1, widget = self.title}
	self:set_child{col = 1, row = 2, widget = self.frame}
	-- Copy arguments.
	for k,v in pairs(args or {}) do
		if type(k) ~= "number" then
			self[k] = v
		end
	end
	-- Create menu items.
	for k,v in ipairs(args or {}) do
		if type(v[2]) == "function" then
			self:append{text = v[1], pressed = v[2], arrow = v.arrow}
		else
			self:append{text = v[1], widget = v[2], arrow = true}
		end
	end
	return self
end

Widgets.Menu.append = function(self, args)
	local row = self.rows + 1
	local button = Widgets.MenuItem{font = "medium", text = args.text, arrow = args.arrow,
		color = {0,0,0,1}, color_focus = {0.7,0.3,0.3,1}, pressed = function()
		if args.widget then Gui.menus:open{level = self.level + 1, widget = args.widget} end
		if args.pressed then args.pressed(self, row) end
	end}
	self.frame:append_row(button)
end

Widgets.Menu.replace = function(self, row, args)
	local widget
	if args then
		widget = Widgets.MenuItem{font = "medium", text = args.text, arrow = args.arrow,
			color = {0,0,0,1}, color_focus = {0.7,0.3,0.3,1}, pressed = function()
			if args.widget then Gui.menus:open{level = self.level + 1, widget = args.widget} end
			if args.pressed then args.pressed(self, row) end
		end}
	end
	self.frame:set_child{row = row, col = 1, widget = widget}
end

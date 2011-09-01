require "client/widgets/menu"

Views.Menu = Class(Widgets.Menu)
Views.Menu.class_name = "Views.Menu"

--- Creates a new menu view.
-- @param clss Menu view class.
-- @return Menu view.
Views.Menu.new = function(clss)
	local admin_menu = Widgets.Menu{text = "Admin",
		{"Stats", function() Client:set_mode("admin", 2) end, arrow = true},
		{"Editor", Editing.dialog},
		{"Save", function() Network:send{packet = Packet(packets.ADMIN_SAVE)} end},
		{"Shutdown", function() Network:send{packet = Packet(packets.ADMIN_SHUTDOWN)} end}}
	local self = Widgets.Menu.new(clss, {text = "Menu",
		{"Inventory", function() Client:set_mode("inventory", 2) end, arrow = true},
		{"Quests", function() Client:set_mode("quests", 2) end, arrow = true},
		{"Skills", function() Client:set_mode("skills", 2) end, arrow = true},
		{"Spells", function() Client:set_mode("feats", 2) end, arrow = true},
		{"Map", function() Client:set_mode("map", 2) end, arrow = true},
		{"-----", function() end},
		{"Controls", function() Client:set_mode("controls", 2) end, arrow = true},
		{"Options", function() Client:set_mode("options", 2) end, arrow = true},
		{"Admin", admin_menu},
		{"Help", function() Client:set_mode("help", 2) end, arrow = true},
		{"Quit", function() Program.quit = true end}})
	self.admin_menu = admin_menu
	self._admin_enabled = true
	return self
end

Views.Menu.back = function(self)
	Client:set_mode("game")
end

Views.Menu.enter = function(self, from, level)
	Gui.menus:open{level = level, widget = self}
end

Views.Menu:add_getters{
	admin_enabled = function(self) return rawget(self, "_admin_enabled") end}

Views.Menu:add_setters{
	admin_enabled = function(self, v)
		if v == self.admin_enabled then return end
		rawset(self, "_admin_enabled", v)
		self:replace(9, v and {"Admin", self.admin_menu})
	end}

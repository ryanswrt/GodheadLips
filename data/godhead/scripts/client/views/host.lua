Views.Host = Class(Widgets.Background)

Views.Host.new = function(clss)
	local self = Widgets.Background.new(clss, {cols = 1, rows = 2, behind = true, fullscreen = true, image = "mainmenu1"})
	self.text = Widgets.Label()
	-- Save slot settings.
	self.combo_file = Widgets.ComboBox({
		{"Save #1", function() self:set_slot(1) end},
		{"Save #2", function() self:set_slot(2) end},
		{"Save #3", function() self:set_slot(3) end},
		{"Save #4", function() self:set_slot(4) end},
		{"Save #5", function() self:set_slot(5) end}})
	self.combo_file:activate{index = Client.config.host_saveslot}
	-- Account settings.
	self.entry_account = Widgets.Entry{text = Client.config.host_account}
	self.entry_password = Widgets.Entry{password = true}
	self.toggle_restart = Widgets.Toggle()
	self.toggle_admin = Widgets.Toggle()
	self.group_account = Widgets.Frame{style = "default", cols = 2, rows = 6}
	self.group_account:set_expand{col = 2, row = 4}
	self.group_account:set_child{col = 1, row = 1, widget = Widgets.Label{text = "Savefile:"}}
	self.group_account:set_child{col = 2, row = 1, widget = self.combo_file}
	self.group_account:set_child{col = 1, row = 2, widget = Widgets.Label{text = "Account:"}}
	self.group_account:set_child{col = 2, row = 2, widget = self.entry_account}
	self.group_account:set_child{col = 1, row = 3, widget = Widgets.Label{text = "Password:"}}
	self.group_account:set_child{col = 2, row = 3, widget = self.entry_password}
	self.group_account:set_child{col = 1, row = 5, widget = Widgets.Label{text = "Admin:"}}
	self.group_account:set_child{col = 2, row = 5, widget = self.toggle_admin}
	self.group_account:set_child{col = 1, row = 6, widget = Widgets.Label{text = "Restart:"}}
	self.group_account:set_child{col = 2, row = 6, widget = self.toggle_restart}
	-- Menu labels.
	self.button_play = Widgets.Label{font = "mainmenu", text = "Host!", pressed = function() self:play() end}
	self.button_back = Widgets.Label{font = "mainmenu", text = "Back", pressed = function() self:back() end}
	self.group_buttons = Widget{cols = 4, rows = 1, margins = {bottom = 30}, spacings = {horz = 40}}
	self.group_buttons:set_expand{col = 1}
	self.group_buttons:set_expand{col = 4}
	self.group_buttons:set_child(2, 1, self.button_play)
	self.group_buttons:set_child(3, 1, self.button_back)
	-- Packing.
	self.group = Widget{cols = 3, rows = 1}
	self.group:set_expand{col = 1, row = 1}
	self.group:set_expand{col = 3}
	self.group:set_child(2, 1, self.group_account)
	self.group1 = Widget{cols = 1, rows = 3}
	self.group1:set_expand{col = 1, row = 1}
	self.group1:set_child(1, 1, self.text)
	self.group1:set_child(1, 2, self.group)
	self.group1:set_child(1, 3, self.group_buttons)
	self:set_expand{col = 1, row = 1}
	self:set_child(1, 2, self.group1)
	return self
end

--- Returns back to the login screen.
-- @param self Host view.
Views.Host.back = function(self)
	Client:set_mode("login")
end

--- Closes the host view.
-- @param self Host view.
Views.Host.close = function(self)
	self.floating = false
end

--- Enters the host view.
-- @param self Host view.
Views.Host.enter = function(self)
	self.floating = true
end

--- Initiates hosting of a game.
-- @param self Host view.
Views.Host.play = function(self)
	-- Save config.
	local saveslot = self.combo_file.value or 1
	local account = self.entry_account.text
	local password = self.entry_password.text
	Client.config.host_saveslot = saveslot
	Client.config.host_account = account
	Client.config:save()
	-- Setup hosting.
	Settings.address = "localhost"
	Settings.file = saveslot
	Settings.admin = self.toggle_admin.active
	Settings.generate = self.toggle_restart.active
	Settings.host = true
	Settings.port = 10101
	if account and #account > 0 then
		Settings.account = account
	else
		Settings.account = "guest"
	end
	if password and #password > 0 then
		Settings.password = password
	else
		Settings.password = ""
	end
	Client:set_mode("startup")
end

---- Sets the active saveslot.
--- @param self Startup.
--- @param slot Saveslot number.
Views.Host.set_slot = function(self, slot)
end

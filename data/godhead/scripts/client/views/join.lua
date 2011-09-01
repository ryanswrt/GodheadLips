require "client/widgets/serverinfo"

Views.Join = Class(Widgets.Background)

--- Creates a new join view.
-- @param clss Join class.
-- @return Join view.
Views.Join.new = function(clss)
	local self = Widgets.Background.new(clss, {cols = 1, rows = 2, behind = true, fullscreen = true, image = "mainmenu1"})
	self.text = Widgets.Label()
	-- Server list.
	self.list_servers = Widgets.List{page_size = 7}
	self.list_servers:set_request{height = 255}
	self.button_servers = Widgets.Button{text = "Refresh", pressed = function() self:refresh() end}
	self.title_servers = Widgets.Frame{style = "title", text = "Server list"}
	self.frame_servers = Widgets.Frame{cols = 1, rows = 3}
	self.frame_servers:set_expand{col = 1, row = 1}
	self.frame_servers:set_child(1, 1, self.list_servers)
	self.frame_servers:set_child(1, 2, self.button_servers)
	self.group_servers = Widget{cols = 1, rows = 4, spacings = {0,0}}
	self.group_servers:set_expand{row = 1, col = 1}
	self.group_servers:set_expand{row = 4, col = 3}
	self.group_servers:set_child(1, 2, self.title_servers)
	self.group_servers:set_child(1, 3, self.frame_servers)
	-- Address settings.
	self.entry_address = Widgets.Entry{text = Client.config.join_address}
	self.entry_port = Widgets.Entry{text = Client.config.join_port}
	self.entry_port:set_request{width = 50}
	self.group_address = Widget{cols = 2, rows = 1}
	self.group_address:set_expand{col = 1}
	self.group_address:set_child(1, 1, self.entry_address)
	self.group_address:set_child(2, 1, self.entry_port)
	-- Account settings.
	self.entry_account = Widgets.Entry{text = Client.config.join_account}
	self.entry_password = Widgets.Entry{password = true}
	self.group_account = Widgets.Frame{style = "default", cols = 2, rows = 3}
	self.group_account:set_expand{col = 2}
	self.group_account:set_child{col = 1, row = 1, widget = Widgets.Label{text = "Address:"}}
	self.group_account:set_child{col = 2, row = 1, widget = self.group_address}
	self.group_account:set_child{col = 1, row = 2, widget = Widgets.Label{text = "Account:"}}
	self.group_account:set_child{col = 2, row = 2, widget = self.entry_account}
	self.group_account:set_child{col = 1, row = 3, widget = Widgets.Label{text = "Password:"}}
	self.group_account:set_child{col = 2, row = 3, widget = self.entry_password}
	-- Menu labels.
	self.button_play = Widgets.Label{font = "mainmenu", text = "Join!", pressed = function() self:play() end}
	self.button_back = Widgets.Label{font = "mainmenu", text = "Back", pressed = function() self:back() end}
	self.group_buttons = Widget{cols = 4, rows = 1, margins = {bottom = 30}, spacings = {horz = 40}}
	self.group_buttons:set_expand{col = 1}
	self.group_buttons:set_expand{col = 4}
	self.group_buttons:set_child(2, 1, self.button_play)
	self.group_buttons:set_child(3, 1, self.button_back)
	-- Packing.
	self.group = Widget{cols = 3, rows = 2}
	self.group:set_expand{col = 1, row = 1}
	self.group:set_expand{col = 3}
	self.group:set_child(2, 1, self.group_servers)
	self.group:set_child(2, 2, self.group_account)
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
-- @param self Join view.
Views.Join.back = function(self)
	Client:set_mode("login")
end

--- Closes the join view.
-- @param self Join view.
Views.Join.close = function(self)
	self.floating = false
end

--- Enters the join view.
-- @param self Join view.
Views.Join.enter = function(self)
	self.floating = true
	self:refresh()
end

--- Launches the selected game.
-- @param self Join view.
Views.Join.play = function(self)
	-- Save settings.
	local address = self.entry_address.text
	local port = self.entry_port.text
	local account = self.entry_account.text
	local password = self.entry_password.text
	Client.config.join_account = account
	Client.config.join_address = address
	Client.config.join_port = port
	Client.config:save()
	-- Setup joining.
	Settings.address = address
	Settings.host = false
	Settings.port = port
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

--- Downloads the server list from the master server.
-- @param self Join view.
Views.Join.refresh = function(self)
	-- Download servers.
	Lobby.master = Client.config.server_master
	self.servers = Lobby:download_server_list()
	-- Rebuild the list.
	self.list_servers:clear()
	if self.servers then
		table.insert(self.servers, 1,  {ip = "localhost", port = 10101, name = "Localhost",
			desc = "Connect to a locally hosted server"})
		for k,v in pairs(self.servers) do
			local w = Widgets.Serverinfo(v)
			w.pressed = function(s)
				for k,v in pairs(self.servers) do
					v.widget.active = (v.widget == s)
				end
				self.entry_address.text = s.ip
				self.entry_port.text = s.port
			end
			v.widget = w
			self.list_servers:append{widget = w}
		end
	end
end

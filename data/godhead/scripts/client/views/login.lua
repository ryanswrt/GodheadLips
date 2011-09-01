Views.Login = Class(Widgets.Background)

--- Creates a login view.
-- @param clss Login view class.
-- @return Login view.
Views.Login.new = function(clss)
	local self = Widgets.Background.new(clss, {cols = 1, rows = 2, behind = true, fullscreen = true, image = "mainmenu1"})
	self.label_hint = Widgets.Label{halign = 0.01, valign = 0.01, font = "medium"}
	self.text = Widgets.Label()
	-- Menu labels.
	self.button_intro = Widgets.Label{font = "mainmenu", text = "Intro", pressed = function() Client:set_mode("intro") end}
	self.button_play = Widgets.Label{font = "mainmenu", text = "Play Now!", pressed = function() self:play() end}
	self.button_host = Widgets.Label{font = "mainmenu", text = "Host", pressed = function() Client:set_mode("host") end}
	self.button_join = Widgets.Label{font = "mainmenu", text = "Join", pressed = function() Client:set_mode("join") end}
	self.button_controls = Widgets.Label{font = "mainmenu", text = "Controls", pressed = function() Client:set_mode("controls") end}
	self.button_options = Widgets.Label{font = "mainmenu", text = "Options", pressed = function() Client:set_mode("options") end}
	self.button_editor = Widgets.Label{font = "mainmenu", text = "Editor", pressed = function() Client:set_mode("editor") end}
	self.button_quit = Widgets.Label{font = "mainmenu", text = "Quit", pressed = function() Program:shutdown() end}
	self.toprow = Widget{cols = 1, rows = 2}
	self.group_buttons = Widget{cols = 9, rows = 1, margins = {bottom = 30}, spacings = {horz = 30}}
	self.group_buttons:set_expand{col = 1}
	self.group_buttons:set_expand{col = 9}
	--self.group_buttons:set_child{col = 2, row = 1, widget = self.button_intro}
	self.toprow:set_child{col = 1, row = 1, widget = self.button_play}
	self.group_buttons:set_child{col = 3, row = 1, widget = self.button_host}
	self.group_buttons:set_child{col = 4, row = 1, widget = self.button_join}
	self.group_buttons:set_child{col = 5, row = 1, widget = self.button_editor}
	self.group_buttons:set_child{col = 6, row = 1, widget = self.button_controls}
	self.group_buttons:set_child{col = 7, row = 1, widget = self.button_options}
	self.group_buttons:set_child{col = 8, row = 1, widget = self.button_quit}
	self.toprow:set_child{col=1,row=2,widget= self.group_buttons}
	-- Main.
	self:set_expand{col = 1, row = 1}
	
	self:set_child(1, 2, self.toprow)
	self:set_child(1, 1, self.label_hint)
	return self
end

--- Exits the game.
-- @param self Login view.
Views.Login.back = function(self)
	Program.quit = true
end

--- Closes the login view.
-- @param self Startup view.
Views.Login.close = function(self)
	self.floating = false
end

--- Initializes and shows the login view.
-- @param self Login view.
-- @param from Name of the previous mode.
Views.Login.enter = function(self, from)
	-- Cleanup if disconnected.
	if from then
		for k,v in pairs(Object.objects) do v:detach() end
		Player.object = nil
		Gui.scene.floating = false
	end
	if from ~= "controls" and from ~= "join" and from ~= "host" and from ~= "options" then
		Sound:switch_music_track("menu")
	end
	-- Show the UI.
	self.floating = true
	-- Show controls.
	self.label_hint.text = [[
Start a game by clicking Host and then Host!

Default Key map:
e = interact
space = jump
c = climb
y = swap between first and third person
tab = swap quickbar mode
esc = game menu
You can reconfigure controls in the game menu
]]
end

--- Initiates hosting of a game.
-- @param self Host view.
Views.Login.play = function(self)
	-- Save config.
	local saveslot = 1
	local account = "guest"
	local password = ""
	Client.config.host_saveslot = saveslot
	Client.config.host_account = account
	Client.config:save()
	-- Setup hosting.
	Settings.address = "localhost"
	Settings.file = saveslot
	Settings.admin = false
	Settings.generate = false
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
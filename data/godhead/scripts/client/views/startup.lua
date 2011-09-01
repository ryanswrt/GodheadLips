Views.Startup = Class(Widgets.Background)

--- Creates a startup view.
-- @param clss Startup class.
-- @return Startup view.
Views.Startup.new = function(clss)
	local self = Widgets.Background.new(clss, {cols = 1, rows = 3, behind = true, fullscreen = true, image = "mainmenu1"})
	self.text = Widgets.Label{halign = 0.5}
	self.button_back = Widgets.Label{font = "mainmenu", text = "Back", pressed = function() self:back() end}
	self.button_quit = Widgets.Label{font = "mainmenu", text = "Quit", pressed = function() Program:shutdown() end}
	self.group_buttons = Widget{cols = 4, rows = 1, margins = {bottom=30}, spacings = {horz=40}}
	self.group_buttons:set_expand{col = 1}
	self.group_buttons:set_expand{col = 4}
	self.group_buttons:set_child{col = 2, row = 1, widget = self.button_back}
	self.group_buttons:set_child{col = 3, row = 1, widget = self.button_quit}
	self.label_hint = Widgets.Label{halign = 0.1, valign = 0.1, font = "medium"}
	self:set_expand{col = 1, row = 1}
	self:set_child(1, 1, self.label_hint)
	self:set_child(1, 2, self.text)
	self:set_child(1, 3, self.group_buttons)
	return self
end

--- Returns to the launcher.
-- @param self Startup view.
Views.Startup.back = function(self)
	Client:set_mode("login")
end

--- Finishes the startup when connection has been established.
-- @param self Startup view.
Views.Startup.close = function(self)
	self.floating = false
	self.connecting = nil
	self.joined = true
end

--- Executes the startup command.
-- @param self Startup view.
-- @param from Name of the previous mode.
Views.Startup.enter = function(self, from)
	-- Cleanup if disconnected.
	if from then
		for k,v in pairs(Object.objects) do v:detach() end
		Player.object = nil
		Gui.scene.floating = false
	end
	-- Show the UI.
	self.floating = true
	self:retry()
	-- Show controls.
	self.label_hint.text = [[Default Key map:
e = interact
space = jump
c = climb
y = swap between first and third person
tab = swap quickbar mode
esc = game menu
You can reconfigure controls in the game menu
]]
end

--- Retries hosting or joining.
-- @param self Startup view.
Views.Startup.retry = function(self)
	if self.host_wait_timer then
		self.host_wait_timer:disable()
		self.host_wait_timer = nil
	end
	if Settings.host then
		-- Host a game.
		local opts = string.format("--file %s --server %s %d", Settings.file, Settings.address, Settings.port)
		if Settings.admin then opts = opts .. " -d" end
		if Settings.generate then opts = opts .. " -g" end
		Program:unload_world()
		Program:launch_server(opts)
		self:set_state("Starting the server on port " .. Settings.port .. "...")
		self.host_wait_timer = Timer{delay = 2, func = function(timer)
			if Network:join{host = "localhost", Settings.port} then
				self:set_state("Connecting to the server...")
				self.connecting = true
			else
				self:set_state("Failed to start the server!")
				self.connecting = nil
			end
			timer:disable()
		end}
	else
		-- Join a game.
		Program:unload_world()
		if Network:join{host = Settings.address, port = Settings.port} then
			self:set_state("Connecting to " .. Settings.address .. ":" .. Settings.port .. "...")
			self.connecting = true
		else
			self:set_state("Failed to connect to " .. Settings.address .. ":" .. Settings.port .. "!")
			self.connecting = nil
		end
	end
end

--- Shows or updates the waiting screen.
-- @param self Startup view.
-- @param text Waiting text.
Views.Startup.set_state = function(self, text)
	self.text.text = text
end

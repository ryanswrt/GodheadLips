Views.Game = Class(Widget)

--- Creates a new game view.
-- @param clss Game view class.
-- @return Game view.
Views.Game.new = function(clss)
	local self = Widget.new(clss)
	return self
end

Views.Game.back = function(self)
	Client:set_mode("menu")
end

--- Enters the game view.
-- @param self Game view.
-- @param from Name of the previous mode.
Views.Game.enter = function(self, from)
	-- Handle fresh startup.
	if from == "startup" or from == "chargen" then
		Gui.scene.floating = true
		Program:unload_world()
		Sound:switch_music_track("game")
	end
	-- Enter movement mode.
	Gui.menus:close()
	Program.cursor_grabbed = true
end

--- Closes the game view.
-- @param self Game view.
Views.Game.close = function(self)
	Program.cursor_grabbed = false
end

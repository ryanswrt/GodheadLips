print "INFO: Loading client scripts."

Program.window_title = "Lips of Suna"
Reload.enabled = true

require "client/client"

Views = {}
Physics.GROUP_OBJECT = 0x0001
Physics.GROUP_EQUIPMENT = 0x0002
Physics.GROUP_CROSSHAIR = 0x0004
Physics.GROUP_PLAYER = 0x0008
Physics.GROUP_STATIC = 0x4000
Physics.GROUP_TILES = 0x8000
Physics.MASK_CAMERA = 0xF001
Physics.MASK_PICK = 0xF001

for k,v in pairs(File:scan_directory("scripts/client/widgets")) do
	require("client/widgets/" .. string.gsub(v, "([^.]*).*", "%1"))
end
require "client/action"
require "client/audio"
require "client/event"
require "client/theme"
require "client/drag"
require "client/commands"
require "client/controls"
require "client/effect"
require "client/equipment"
require "client/quickslots"
require "client/gui"
require "client/player"
require "client/slots"
require "client/target"
for k,v in pairs(File:scan_directory("scripts/client/network")) do
	require("client/network/" .. string.gsub(v, "([^.]*).*", "%1"))
end
for k,v in pairs(File:scan_directory("scripts/client/objects")) do
	require("client/objects/" .. string.gsub(v, "([^.]*).*", "%1"))
end
for k,v in pairs(File:scan_directory("scripts/client/shaders")) do
	require("client/shaders/" .. string.gsub(v, "([^.]*).*", "%1"))
end
for k,v in pairs(File:scan_directory("scripts/client/views")) do
	require("client/views/" .. string.gsub(v, "([^.]*).*", "%1"))
end

-- Initialize the UI state.
Widgets.Cursor.inst = Widgets.Cursor(Iconspec:find{name = "cursor1"})
Gui:init()
Client:init()
Player.crosshair = Object{model = "crosshair1", collision_group = Physics.GROUP_CROSSHAIR}
if Settings.join or Settings.host then
	Client:set_mode("startup")
elseif Settings.editor then
	-- FIXME: Why is this needed?
	Program:update()
	local event = Program:pop_event()
	while event do
		Eventhandler:event(event)
		event = Program:pop_event()
	end
	Client:set_mode("editor")
else
	Client:set_mode("login")
end

Program.profiling = {}

-- Main loop.
while not Program.quit do
	-- Update program state.
	--FIXME: watchdog needs to be re-enabled once its fixed
	--Program:watchdog_start(3)
	local t1 = Program.time
	Program:update()
	local t2 = Program.time
	-- Handle events.
	local event = Program:pop_event()
	while event do
		Eventhandler:event(event)
		event = Program:pop_event()
	end
	Widgets:update()
	local t3 = Program.time
	-- Render the scene.
	Program:clear_buffer()
	Widgets:draw()
	Program:swap_buffers()
	local t4 = Program.time
	-- Update profiling stats.
	Program.profiling.update = t2 - t1
	Program.profiling.event = t3 - t2
	Program.profiling.render = t4 - t3
end

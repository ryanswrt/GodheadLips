print "INFO: Loading server scripts."

require "server/util"
require "server/account"
require "server/serialize"
require "server/controls"
require "server/chat"
require "server/config"
require "server/crafting"
require "server/dialog"
require "server/event"
require "server/feat"
require "server/marker"
require "server/sectors"
require "server/admin"
require "server/quest"
require "server/modifier"
require "server/editing"
require "server/particles"
for k,v in pairs(File:scan_directory("scripts/server/actions")) do
	require("server/actions/" .. string.gsub(v, "([^.]*).*", "%1"))
end
for k,v in pairs(File:scan_directory("scripts/server/objects")) do
	require("server/objects/" .. string.gsub(v, "([^.]*).*", "%1"))
end
for k,v in pairs(File:scan_directory("scripts/server/influences")) do
	require("server/influences/" .. string.gsub(v, "([^.]*).*", "%1"))
end
for k,v in pairs(File:scan_directory("scripts/server/modifiers")) do
	require("server/modifiers/" .. string.gsub(v, "([^.]*).*", "%1"))
end
require "server/generator"
require "server/generator/main"

Network:host{port = Config.inst.server_port}
if Settings.generate or
   Serialize:get_value("map_version") ~= Generator.map_version or
   Serialize:get_value("data_version") ~= Serialize.data_version then
	Generator.inst:generate()
	Serialize:set_value("data_version", Serialize.data_version)
else
	Serialize:load()
end

Voxel.block_changed_cb = function(index, stamp)
	Vision:event{type = "voxel-block-changed", index = index, point = ARGH, stamp = stamp}
end

Program.sleep = 1/60
Program.profiling = {}

-- Main loop.
while not Program.quit do
	-- Update program state.
	local t1 = Program.time
	Program:update()
	local t2 = Program.time
	-- Handle events.
	local event = Program:pop_event()
	while event do
		Eventhandler:event(event)
		event = Program:pop_event()
	end
	local t3 = Program.time
	-- Store timings.
	Program.profiling.update = t2 - t1
	Program.profiling.event = t3 - t2
end

-- Save at exit.
Serialize:save()

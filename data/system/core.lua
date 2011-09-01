require "system/class"

Program = Class()
Program.class_name = "Program"

--- Sets the name of the mod to be executed after this one quits.
-- @param clss Program class.
-- @param args Arguments.<ul>
--  <li>1,name: Module name.</li></ul>
--  <li>2,args: Argument string to pass to the module.</li></ul>
Program.launch_mod = function(self, args)
	return Los.program_launch_mod(args)
end

--- Pops an event from the event queue.
-- @param clss Program class.
-- @return Event table or nil.
Program.pop_event = function(clss)
	if not __events then return end
	local t = __events[1]
	if t then table.remove(__events, 1) end
	return t
end

--- Pops a message sent by the parent script.
-- @param self Thread.
-- @return Message table or nil.
Program.pop_message = function(self)
	local r = Los.program_pop_message()
	if not r then return end
	if r.type == "model" and r.model then
		r.model = Class.new(Model, {handle = r.model})
	end
	return r
end

--- Pushes an event to the back of the event queue.
-- @param clss Program class.
-- @return event Event table.
Program.push_event = function(clss, event)
	if not __events then __events = {} end
	table.insert(__events, event)
end

--- Pushes a message to the parent script.
-- @param self Thread.
-- @param ... Message arguments.
-- @return True on success.
Program.push_message = function(self, ...)
	local a = ...
	if type(a) == "table" then
		if a.model then a.model = a.model.handle end
		return Los.program_push_message(a)
	else
		return Los.program_push_message(...)
	end
end

--- Unloads a sector.<br/>
-- Unrealizes all objects in the sector and clears the terrain in the sector.
-- The sector is then removed from the sector list.
-- @param clss Program class.
-- @param args Arguments.<ul>
--   <li>sector: Sector index.</li></ul>
Program.unload_sector = function(clss, args)
	for k,v in pairs(Object:find{sector = args.sector}) do v.realized = false end
	Los.program_unload_sector(args)
end

--- Unloads the world map.<br/>
-- Unrealizes all objects and destroys all sectors of the world map.
-- You usually want to do this when you're about to create a new map with
-- the map generator to avoid parts of the old map being left in the game.
-- @param clss Program class.
Program.unload_world = function(clss)
	for k,v in pairs(__objects_realized) do k.realized = false end
	Los.program_unload_world()
end

--- Calls the function and catches and handles errors.
-- @param clss Program class.
-- @param func Function to call.
Program.safe_call = function(clss, func)
	xpcall(func, function(err) print(debug.traceback("ERROR: " .. err)) end)
end

--- Request program shutdown.
-- @param clss Program class.
Program.shutdown = function(clss, args)
	Los.program_shutdown(args)
end

--- Updates the program state.
-- @param clss Program class.
Program.update = function(clss, args)
	Los.program_update(args)
end

--- Waits for the given number seconds.
-- @param clss Program class.
-- @param secs Delay in seconds.
Program.wait = function(clss, secs)
	Los.program_wait(secs)
end

--- The argument string passed to the program at startup time.
-- @name Program.args
-- @class table

--- Boolean indicating whether the game needs to exit.
-- @name Program.quit
-- @class table

--- Dictionary of indices of active sectors.
-- @name Program.sectors
-- @class table

--- Sector size in world units.
-- @name Program.sector_size
-- @class table

--- Sleep time between ticks, in seconds.
-- @name Program.quit
-- @class table

--- Short term average tick length in seconds.
-- @name Program.tick
-- @class table

--- Number of seconds the program has been running.
-- @name Program.time
-- @class table

Program.class_getters = {
	args = function(s) return Los.program_get_args() end,
	quit = function(s) return Los.program_get_quit() end,
	sectors = function(s) return Los.program_get_sectors() end,
	sector_size = function(s) return Los.program_get_sector_size() end,
	sleep = function(s) return Los.program_get_sleep() end,
	tick = function(s) return Los.program_get_tick() end,
	time = function(s) return Los.program_get_time() end}

Program.class_setters = {
	quit = function(s, v) Los.program_set_quit(v) end,
	sleep = function(s, v) Los.program_set_sleep(v) end}

Program.unittest = function()
	-- Getters and setters.
	assert(type(Program.args) == "string")
	assert(type(Program.quit) == "boolean")
	assert(type(Program.sectors) == "table")
	assert(type(Program.sleep) == "number")
	assert(type(Program.tick) == "number")
	assert(type(Program.time) == "number")
	Program.quit = true
	assert(Program.quit)
	-- Message passing disabled for non-threads.
	assert(not Program:push_message("fail", "fail"))
	assert(not Program:pop_message())
end

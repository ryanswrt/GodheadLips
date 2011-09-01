require "system/eventhandler"

Coroutine = Class()
Coroutine.class_name = "Coroutine"
Coroutine.routines = {}

--- Creates a thread executing a function.
-- @param self Coroutine class.
-- @param func Function to execute.
-- @return Coroutine.
Coroutine.new = function(clss, func)
	local co = coroutine.create(func)
	if coroutine.status(co) == "dead" then return end
	clss.routines[co] = co
	return co
end

Coroutine.sleep = function(self, delay)
	local t = 0.0
	while t < delay do t = t + coroutine.yield() end
end

--- Updates all the threads.
-- @param clss Coroutine class.
-- @param secs Number of seconds since the last update.
Coroutine.update = function(clss, secs)
	for key,value in pairs(clss.routines) do
		local ret,err = coroutine.resume(value, secs)
		if not ret then
			print(debug.traceback(value, "ERROR: " .. err))
		end
		if coroutine.status(value) == "dead" then
			clss.routines[key] = nil
		end
	end
end

Coroutine.yield = function(self)
	return coroutine.yield()
end

-- Updating threads.
Eventhandler{type = "tick", func = function(self, args)
	Coroutine:update(args.secs)
end}

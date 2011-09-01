require "system/core"

if not Los.program_load_extension("watchdog") then
	error("loading extension `watchdog' failed")
end

--- Starts or refreshes the watchdog timer.
-- @param clss Program class.
-- @param secs Timer elapsing timer.
Program.watchdog_start = function(self, secs)
	return Los.watchdog_start(secs)
end

--- Stops the watchdog timer.
-- @param clss Program class.
Program.watchdog_stop = function(clss)
	return Los.watchdog_stop()
end

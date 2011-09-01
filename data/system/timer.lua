Timer = Class()
Timer.class_name = "Timer"
Timer.dict_timer = {}
Timer.dict_delete = {}
setmetatable(Timer.dict_timer, {__mode = "v"})

--- Creates a new timer and enables it.<br/>
-- The timer will not be subject to garbage collection when enabled.
-- @param clss Timer class.
-- @param args Arguments.<ul>
--   <li>accurate: True to trigger multiple times in one frame to compensate for low frame rate.</li>
--   <li>delay: Delay in seconds or nil.</li>
--   <li>enabled: False to disable by default.</li>
--   <li>func: Callback function.</li>
--   <li>owner: Owner object or nil.</li></ul>
-- @return New timer.
Timer.new = function(clss, args)
	local self = Class.new(clss, args)
	if args.enabled == nil then
		self:enable()
	end
	return self
end

--- Disables the timer.<br/>
-- The timer will be subject to normal garbage collection when disabled.
-- @param self Timer.
Timer.disable = function(self)
	-- Since this is often called from inside callback, we add removed
	-- timers to a separate table to not screw up the update loop.
	Timer.dict_delete[self] = true
end

--- Enables the timer.<br/>
-- The timer will not be subject to garbage collection when enabled.
-- @param self Timer.
Timer.enable = function(self)
	Timer.dict_timer[self] = self.owner or true
	self.updated = Program.time
end

Timer:add_setters{
	enabled = function(s, v)
		if v then
			s:enable()
		else
			s:disable()
		end
	end}

-- Register event handler.
Eventhandler{type = "tick", func = function(self, args)
	local t = Program.time
	-- Remove timers.
	for k in pairs(Timer.dict_delete) do
		Timer.dict_timer[k] = nil
	end
	Timer.dict_delete = {}
	-- Update timers.
	for k,v in pairs(Timer.dict_timer) do
		local delay = (k.delay or 0)
		local target = k.updated + delay
		if target <= t then
			if k.accurate and delay > 0 then
				while target <= t do
					k:func(delay)
					k.updated = target
					target = target + delay
				end
			else
				k:func(delay + t - target)
				k.updated = t
			end
		end
	end
end}

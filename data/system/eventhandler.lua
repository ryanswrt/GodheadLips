Eventhandler = Class()
Eventhandler.class_name = "Eventhandler"
Eventhandler.handlers = {}
setmetatable(Eventhandler.handlers, {__mode = "v"})

--- Creates a new event handler and enables it.<br/>
-- The event handler will not be subject to garbage collection when enabled.
-- @param clss Event handler class.
-- @param args Arguments.<ul>
--   <li>func: Callback function. (required)</li>
--   <li>owner: Owner object.</li>
--   <li>type: Event type. (required)</li></ul>
-- @return New event handler.
Eventhandler.new = function(clss, args)
	local self = Class.new(clss, args)
	self:enable()
	return self
end

--- Disables the event handler.<br/>
-- The event handler will be subject to normal garbage collection when disabled.
-- @param self Event handler.
Eventhandler.disable = function(self)
	Eventhandler.handlers[self] = nil
end

--- Enables the event handler.<br/>
-- The event handler will not be subject to garbage collection when enabled.
-- @param self Event handler.
Eventhandler.enable = function(self)
	Eventhandler.handlers[self] = self.owner or true
end

--- Handles an event.
-- @param clss Event handler class.
-- @param args Event arguments.
Eventhandler.event = function(clss, args)
	-- Translate handles.
	if args.type == "packet" then
		args.packet = Class.new(Packet, {handle = args.packet})
	end
	for k,v in pairs(args) do
		if type(v) == "userdata" then
			-- TODO: Update this if some events can create other kinds of userdata.
			local d = __userdata_lookup[v]
			if d then
				args[k] = d
			else
				args[k] = Class.new(Vector, {handle = v})
			end
		end
	end
	-- Invoke event handlers.
	for k,v in pairs(clss.handlers) do
		if k.type == args.type then
			Program:safe_call(function() k:func(args) end)
		end
	end
end


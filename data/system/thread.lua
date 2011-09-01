require "system/model"

if not Los.program_load_extension("thread") then
	error("loading extension `thread' failed")
end

------------------------------------------------------------------------------

Thread = Class()

--- Creates a new thread.
-- @param clss Thread class.
-- @param ... Arguments.<ul>
--   <li>1,file: Script path to execute.</li>
--   <li>2,args: Argument string to pass./<li></ul>
--   <li>3,code: Code string to execute./<li></ul>
-- @return Thread.
Thread.new = function(clss, ...)
	local h = Los.thread_new(...)
	assert(h, "thread creation failed")
	return Class.new(Thread, {handle = h})
end

--- Pops a message sent by the child script of the thread.
-- @param self Thread.
-- @return Message table or nil.
Thread.pop_message = function(self)
	local r = Los.thread_pop_message(self.handle)
	if not r then return end
	if r.type == "model" and r.model then
		r.model = Class.new(Model, {handle = r.model})
	end
	return r
end

--- Pushes a message to the child script of the thread.
-- @param self Thread.
-- @param ... Message arguments.
-- @return True on success.
Thread.push_message = function(self, ...)
	local a = ...
	if type(a) == "table" then
		if a.model then a.model = a.model.handle end
		return Los.thread_push_message(self.handle, a)
	else
		return Los.thread_push_message(self.handle, ...)
	end
end

Thread.unittest = function()
	-- Creation.
	print("Testing thread creation...")
	local t = Thread(nil, "Hello World!", [[
		assert(Los.program_get_args() == "Hello World!")]])
	assert(t)
	-- Message passing.
	print("Testing thread message passing...")
	local t1 = Thread(nil, "", [[
		require "system/core"
		local m
		repeat m = Program:pop_message() until m
		assert(m.type == "string")
		assert(m.name == "name1")
		assert(m.string == "string1")
		assert(Program:push_message("name2", "string2"))]])
	assert(t1:push_message("name1", "string1"))
	local m
	repeat m = t1:pop_message() until m
	assert(m.type == "string")
	assert(m.name == "name2")
	assert(m.string == "string2")
	-- Model passing.
	print("Testing thread message model passing...")
	local t2 = Thread(nil, "", [[
		require "system/core"
		require "system/model"
		assert(Program:push_message{model = Model()})]])
	repeat m = t2:pop_message() until m
	assert(m.type == "model")
	assert(m.model)
	assert(m.model.class_name == "Model")
end

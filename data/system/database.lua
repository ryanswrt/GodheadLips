require "system/class"

if not Los.program_load_extension("database") then
	error("loading extension `database' failed")
end

------------------------------------------------------------------------------

Database = Class()
Database.class_name = "Database"

--- Opens a database.
-- @param clss Database class.
-- @param args Arguments.<ul>
--   <li>1,name: Unique database name.</li></ul>
-- @return New database.
Database.new = function(clss, args)
	local self = Class.new(clss)
	self.handle = Los.database_new(args)
	if not self.handle then
		local n = (type(args) == "string") and args or args.name
		assert(self.handle, string.format("creating database %q failed", n))
	end
	__userdata_lookup[self.handle] = self
	return self
end

--- Queries the database.<br/>
-- Executes an SQLite query and returns the results in a table. The returned
-- table contains a list of tables that denote the rows of the result. The row
-- tables further contain a list of values that denote the contents of the columns.<br/>
-- You can avoid escaping the arguments required by the query by writing a `?' in
-- place of the argument in the query and then passing the value in the binding
-- array. The binding array is a simple table that contains the arguments in the
-- same order as the query.
-- @param self Database.
-- @param args Arguments.<ul>
--   <li>1: Query string.</li>
--   <li>2: Array of values to bind to the statement.</ul>
-- @return Table of rows.
Database.query = function(self, a, b)
	-- Translate packets to handles.
	local s
	if b then
		s = {}
		for k,v in pairs(b) do
			s[k] = (type(v) == "table") and v.handle or v
		end
	end
	-- Perform the query.
	local t = Los.database_query(self.handle, a, s)
	-- Translate handles to packets.
	for k1,v1 in pairs(t) do
		for k2,v2 in pairs(v1) do
			if type(v2) == "userdata" then v1[k2] = Class.new(Packet, {handle = v2}) end
		end
	end
	return t
end

--- Approximate memory used by add databases, in bytes.
-- @name Database.memory_used
-- @class table

Database:add_class_getters{
	memory_used = function(self) return Los.database_get_memory_used(self) end}

Database.unittest = function()
	-- Database creation.
	local d = Database("unittest.sqlite")
	assert(d)
	-- Table editing.
	d:query("DROP TABLE IF EXISTS terrain;")
	d:query("CREATE TABLE IF NOT EXISTS terrain (sector INTEGER PRIMARY KEY,data TEXT);")
	d:query("INSERT INTO terrain (sector,data) VALUES (?,?);", {1, "unittest"})
	-- Table selection.
	local r = d:query("SELECT * FROM terrain WHERE sector=?;", {1})
	assert(type(r) == "table")
	assert(#r == 1)
	assert(type(r[1]) == "table")
	assert(r[1][1] == 1)
	assert(r[1][2] == "unittest")
end

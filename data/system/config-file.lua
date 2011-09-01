require "system/class"

if not Los.program_load_extension("config-file") then
	error("loading extension `config-file' failed")
end

------------------------------------------------------------------------------

ConfigFile = Class()
ConfigFile.class_name = "ConfigFile"

--- Opens a configuration file.
-- @param clss ConfigFile class.
-- @param args Arguments.<ul>
--   <li>1,name: Unique configuration file name.</li></ul>
-- @return New tile.
ConfigFile.new = function(clss, args)
	local self = Class.new(clss)
	self.handle = Los.config_file_new(args)
	assert(self.handle, string.format("creating config file %q failed", args[1] or args.name))
	__userdata_lookup[self.handle] = self
	return self
end

--- Gets a configuration value.
-- @param self Configuration file.
-- @param args Arguments.<ul>
--   <li>1,key: Key string.</li></ul>
-- @return Value string or nil.
ConfigFile.get = function(self, k)
	return Los.config_file_get(self.handle, k)
end

--- Saves the configuration file.
-- @param self Configuration file.
ConfigFile.save = function(self)
	return Los.config_file_save(self.handle)
end

--- Sets a configuration value.
-- @param self Configuration file.
-- @param args Arguments.<ul>
--   <li>1,key: Key string.</li>
--   <li>2,value: Value string.</li></ul>
ConfigFile.set = function(self, k, v)
	Los.config_file_set(self.handle, k, v)
end

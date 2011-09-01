Config = Class()

--- Creates a new launcher configuration instance.
-- @param clss Config class.
-- @return Config.
Config.new = function(clss)
	local self = Class.new(clss)
	-- Initialize defaults.
	self.host_account = "guest"
	self.host_saveslot = 1
	self.join_account = "guest"
	self.join_address = "localhost"
	self.join_port = 10101
	self.server_master = "http://lipsofsuna.org"
	-- Load configuration.
	self.config = ConfigFile{name = "launcher.cfg"}
	self:load()
	self:save()
	-- Setup lobby.
	Lobby.master = self.server_master
	return self
end

--- Loads launcher configuration.
-- @param self Config.
Config.load = function(self)
	local opts = {
		host_account = function(v) self.host_account = v end,
		host_saveslot = function(v) self.host_saveslot = tonumber(v) end,
		join_account = function(v) self.join_account = v end,
		join_address = function(v) self.join_address = v end,
		join_port = function(v) self.join_port = tonumber(v) end,
		server_master = function(v) self.server_master = v end}
	-- Read values from the configuration file.
	for k in pairs(opts) do
		local v = self.config:get(k)
		if v then
			local opt = opts[k]
			opt(v)
		end
	end
end

--- Saves launcher configuration.
-- @param self Config.
Config.save = function(self)
	-- Write launcher settings.
	self.config:set("host_account", host_account)
	self.config:set("host_saveslot", tostring(self.host_saveslot))
	self.config:set("join_account", self.join_account)
	self.config:set("join_address", self.join_address)
	self.config:set("join_port", tostring(self.join_port))
	self.config:set("server_master", self.server_master)
	-- Flush the file.
	self.config:save()
end

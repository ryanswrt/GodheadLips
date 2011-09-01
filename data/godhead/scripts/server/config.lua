Config = Class()

--- Creates a new server configuration instance.
-- @param clss Config class.
-- @return Config.
Config.new = function(clss)
	local self = Class.new(clss)
	-- Initialize defaults.
	self.admins = {}
	self.server_announce = false
	self.server_desc = "-"
	self.server_name = "Unnamed server"
	self.server_master = "http://lipsofsuna.org"
	self.server_port = 10101
	-- Get the spawn point.
	print("setting spawn point")
	local reg = Regionspec:find{name = "Giant Shaft"}
	Config.spawn_point = reg.spawn_point_world
	-- Load configuration.
	self.config = ConfigFile{name = "server.cfg"}
	self:load()
	self:save()
	-- Setup lobby.
	Lobby.players = 0
	if self.server_announce then
		Lobby.desc = self.server_desc
		Lobby.name = self.server_name
		Lobby.master = self.server_master
		Lobby.port = self.server_port
	end
	return self
end

--- Loads server configuration.
-- @param self Config.
Config.load = function(self)
	local opts = {
		admins = function(v)
			self.admins = {}
			for w in string.gmatch(v, "[ \t\n]*([a-zA-Z0-9]+)[ \t\n]*") do
				self.admins[w] = true
			end
		end,
		server_announce = function(v) self.server_announce = (v ~= "false") end,
		server_desc = function(v) self.server_desc = v end,
		server_name = function(v) self.server_name = v end,
		server_master = function(v) self.server_master = v end,
		server_port = function(v) self.server_port = tonumber(v) end}
	-- Read values from the configuration file.
	for k in pairs(opts) do
		local v = self.config:get(k)
		if v then
			local opt = opts[k]
			opt(v)
		end
	end
end

--- Saves server configuration.
-- @param self Config.
Config.save = function(self)
	-- Construct admin list.
	local a = {}
	for k in pairs(self.admins) do table.insert(a, k) end
	table.sort(a)
	local admins = ""
	for k,v in ipairs(a) do admins = admins .. v end
	-- Write server settings.
	self.config:set("admins", admins)
	self.config:set("server_announce", tostring(self.server_announce))
	self.config:set("server_desc", self.server_desc)
	self.config:set("server_name", self.server_name)
	self.config:set("server_master", self.server_master)
	self.config:set("server_port", tostring(self.server_port))
	-- Flush the file.
	self.config:save()
end

Config.inst = Config()

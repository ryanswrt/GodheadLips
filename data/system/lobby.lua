require "system/timer"

if not Los.program_load_extension("lobby") then
	error("loading extension `lobby' failed")
end

Lobby = Class()
Lobby.class_name = "Lobby"

--- Downloads the server list from the master server.
-- @param self Lobby class.
-- @return List of servers or nil if failed.
Lobby.download_server_list = function(self)
	if self.master then
		return Los.lobby_download_server_list{master = self.master}
	end
end

--- Sends server info to the master server.
-- @param self Lobby class.
Lobby.upload_server_info = function(self)
	if self.desc and self.master and self.name and self.players and self.port then
		Los.lobby_upload_server_info{desc = self.desc, master = self.master,
			name = self.name, players = self.players, port = self.port}
	end
end

Lobby.class_getters = {
	desc = function(s) return rawget(s, "__desc") end,
	master = function(s) return rawget(s, "__master") end,
	name = function(s) return rawget(s, "__name") end,
	players = function(s) return rawget(s, "__players") end,
	port = function(s) return rawget(s, "__port") end}

Lobby.class_setters = {
	desc = function(s, v)
		if s.desc ~= v then
			rawset(s, "__desc", v)
			s:upload_server_info()
		end
	end,
	master = function(s, v)
		if s.master ~= v then
			rawset(s, "__master", v)
			s:upload_server_info()
		end
	end,
	name = function(s, v)
		if s.name ~= v then
			rawset(s, "__name", v)
			s:upload_server_info()
		end
	end,
	players = function(s, v)
		if s.players ~= v then
			rawset(s, "__players", v)
			s:upload_server_info()
		end
	end,
	port = function(s, v)
		if s.port ~= v then
			rawset(s, "__port", v)
			s:upload_server_info()
		end
	end}

Timer{delay = 3000, func = function()
	Lobby:upload_server_info()
end}

Eventhandler{type = "login", func = function(self, event)
	print("Client login")
	-- Tell the client to authenticate.
	Network:send{client = event.client, packet = Packet(packets.CLIENT_AUTHENTICATE)}
	-- Update lobby.
	Lobby.players = Lobby.players + 1
end}

Eventhandler{type = "logout", func = function(self, event)
	print("Client logout")
	-- Detach the player object.
	local object = Player.clients[event.client]
	if object then
		object:detach()
		Player.clients[event.client] = nil
	end
	-- Update the account.
	local account = Account.dict_client[event.client]
	if account then
		Serialize:save_account(account, object)
		account.client = nil
		Account.dict_client[event.client] = nil
		Account.dict_name[account.login] = nil
	end
	-- Update lobby.
	Lobby.players = Lobby.players - 1
end}

Eventhandler{type = "object-contact", func = function(self, event)
	if event.self.contact_cb then
		event.self:contact_cb(event)
	end
end}

Eventhandler{type = "object-motion", func = function(self, event)
	Vision:event{type = "object-moved", object = event.object}
end}

local marker_timer = 0
Eventhandler{type = "tick", func = function(self, event)
	-- Update creatures.
	for k,v in pairs(Creature.dict_id) do
		if v.realized then
			v:update(event.secs)
		end
	end
	-- Update markers.
	marker_timer = marker_timer + event.secs
	if marker_timer > 2 then
		marker_timer = 0
		for k,m in pairs(Marker.dict_name) do
			if m.unlocked and m.target then
				local o = Object:find{id = m.target}
				if o and (m.position - o.position).length > 1 then
					m.position = o.position
					local p = Packet(packets.MARKER_UPDATE,
						"string", m.name,
						"float", m.position.x,
						"float", m.position.y,
						"float", m.position.z)
					for k,v in pairs(Player.clients) do
						v:send(p)
					end
				end
			end
		end
	end
end}

Protocol:add_handler{type = "CHARACTER_ACCEPT", func = function(event)
	Client:set_mode("game")
	Quickslots:reset()
end}

Protocol:add_handler{type = "CHARACTER_CREATE", func = function(event)
	Client:set_mode("chargen")
end}

Protocol:add_handler{type = "AUTHENTICATE_REJECT", func = function(event)
	local ok,s = event.packet:read("string")
	if not ok then return end
	Client:set_mode("startup")
	Client.views.startup:set_state("Authentication failed: " .. s)
end}

Protocol:add_handler{type = "CLIENT_AUTHENTICATE", func = function(event)
	Network:send{packet = Packet(packets.CLIENT_AUTHENTICATE, "string", Settings.account, "string", Settings.password)}
end}

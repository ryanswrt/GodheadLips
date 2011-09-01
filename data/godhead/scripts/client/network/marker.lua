-- Adds a map marker.
Protocol:add_handler{type = "MARKER_ADD", func = function(event)
	local ok,n,x,y,z = event.packet:read("string", "float", "float", "float")
	if not ok then return end
	local m = Marker:find{name = n}
	if not m then
		m = Marker{name = n}
		Gui.chat_history:append{text = "Location added: " .. n}
	end
	m.position = Vector(x,y,z)
end}

-- Updates a map marker.
Protocol:add_handler{type = "MARKER_UPDATE", func = function(event)
	local ok,n,x,y,z = event.packet:read("string", "float", "float", "float")
	if not ok then return end
	local m = Marker:find{name = n}
	if not m then return end
	m.position = Vector(x,y,z)
end}

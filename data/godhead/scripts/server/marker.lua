Marker.unlock = function(self)
	if self.unlocked then return end
	self.unlocked = true
	local p = Packet(packets.MARKER_ADD,
		"string", self.name,
		"float", self.position.x,
		"float", self.position.y,
		"float", self.position.z)
	for k,v in pairs(Player.clients) do
		v:send(p)
	end
	Serialize:save_marker(self)
end

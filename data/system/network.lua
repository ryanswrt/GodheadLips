require "system/class"

if not Los.program_load_extension("network") then
	error("loading extension `network' failed")
end

------------------------------------------------------------------------------

Network = Class()
Network.class_name = "Network"

--- Disconnects a client.
-- @param clss Network class.
-- @param args Arguments.<ul>
--   <li>1,client: Client number. (required)</li></ul>
Network.disconnect = function(clss, args)
	Los.network_disconnect(args)
end

--- Begins listening for clients.
-- @param clss Network class.
-- @param args Arguments.<ul>
--   <li>port: Port to listen to.</li>
--   <li>udp: True for UDP.</li></ul>
-- @return True on success.
Network.host = function(clss, args)
	return Los.network_host(args)
end

--- Connects to a server.
-- @param clss Network class.
-- @param args Arguments.<ul>
--   <li>host: Server address.</li>
--   <li>port: Port to listen to.</li>
-- @return True on success.
Network.join = function(clss, args)
	return Los.network_join(args)
end

--- Sends a network packet to the client controlling the object.
-- @param clss Network class.
-- @param args Arguments.<ul>
--   <li>client: Client ID if hosting.</li>
--   <li>packet: Packet.</li>
--   <li>reliable: Boolean.</li></ul>
Network.send = function(clss, args)
	Los.network_send{client = args.client, packet = args.packet.handle, reliable = args.reliable}
end

--- Disconnects all client and closes the network connection.
-- @param clss Network class.
Network.shutdown = function(self)
	Los.network_shutdown()
end

--- Updates the network status and generates network events.
-- @param clss Network class.
Network.update = function(clss)
	Los.network_update()
end

--- Gets the list of connected clients (read-only).
-- @name Network.clients
-- @class table

--- Controls whether clients can connect to the server.
-- @name Network.closed
-- @class table

--- Gets whether the game connected to network (read-only).
-- @name Network.connected
-- @class table

Network.class_getters = {
	clients = function(s) return Los.network_get_clients() end,
	closed = function(s) return Los.network_get_closed() end,
	connected = function(s) return Los.network_get_connected() end}

Network.class_setters = {
	closed = function(s, v) Los.network_set_closed(v) end}

------------------------------------------------------------------------------

Packet = Class()
Packet.class_name = "Packet"

--- Creates a new packet.
-- @param self Packet class.
-- @param type Packet type.
-- @param ... Packet contents.
-- @return New packet.
Packet.new = function(clss, ...)
	local self = Class.new(clss)
	self.handle = Los.packet_new(...)
	__userdata_lookup[self.handle] = self
	return self
end

--- Reads data starting from the beginning of the packet.
-- @param self Packet.
-- @param ... Types to read.
-- @return Boolean and a list of read values.
Packet.read = function(self, ...)
	return Los.packet_read(self.handle, ...)
end

--- Reads data starting from the last read positiong of the packet.
-- @param self Packet.
-- @param ... Types to read.
-- @return Boolean and a list of read values.
Packet.resume = function(self, ...)
	return Los.packet_resume(self.handle, ...)
end

--- Appends data to the packet.
-- @param self Packet.
-- @param ... Types to write.
Packet.write = function(self, ...)
	return Los.packet_write(self.handle, ...)
end

--- Size in bytes.
-- @name Packet.size
-- @class table

--- Type number.
-- @name Packet.type
-- @class table

Packet:add_getters{
	size = function(s) return Los.packet_get_size(s.handle) end,
	type = function(s) return Los.packet_get_type(s.handle) end}

Packet:add_setters{
	type = function(s, v) Los.packet_set_type(s.handle, v) end}

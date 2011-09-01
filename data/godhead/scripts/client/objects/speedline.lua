Speedline = Class(Object)

--- Creates a new speed line effect for the object.
-- @param clss Speedline class.
-- @param args Arguments.<ul>
--   <li>delay: Starting delay in seconds.</li>
--   <li>duration: Lifetime in seconds.</li>
--   <li>length: Length in vertices.</li>
--   <li>object: Object.</li></ul>
-- @return Speedline effect.
Speedline.new = function(clss, args)
	self = Object.new(clss)
	self.object = args.object
	self.path = {}
	self.verts = {}
	self.timer = 0
	self.length = args.length or 15
	self.delay = args.delay or 0
	self.duration = args.duration or 10000
	self.model = Model()
	self.model:add_material{cull = false, shader = "speedline"}
	return self
end

--- Updates the speedline.
-- @param self Speedline.
-- @param secs Seconds since the last update.
Speedline.update = function(self, secs)
	-- Limit the refresh rate.
	self.timer = self.timer + secs
	if self.timer < 0.02 then return end
	local tick = self.timer
	self.timer = 0
	-- Update the delay timer.
	if self.delay > 0 then
		self.delay = self.delay - tick
		if self.delay < 0 then
			self.delay = 0
		else
			return
		end
	end
	-- Update the duration timer.
	if self.duration > 0 then
		self.duration = self.duration - tick
		if self.duration < 0 then self.duration = 0 end
	end
	-- Calculate the path length.
	local l = self.length
	local h = 0.07
	local n = #self.path
	local rm = function()
		table.remove(self.path, 1)
		for i=1,6 do
			table.remove(self.verts, 1)
		end
	end
	-- Update the path.
	if self.object.realized and self.duration > 0 then
		-- Realize the effect.
		if not self.realized then
			self.position = self.object.position
			self.realized = true
		end
		-- Get the current line vertices.
		local p1 = self.object:find_node{name = "#blade1"}
		local p2 = self.object:find_node{name = "#blade2"}
		if not p1 or not p2 then
			p1 = self.object.position - Vector(0,h)
			p2 = self.object.position + Vector(0,h)
		else
			p1 = self.object.position + self.object.rotation * p1
			p2 = self.object.position + self.object.rotation * p2
		end
		p1 = p1 - self.position
		p2 = p2 - self.position
		-- Extrude from the previous vertices.
		table.insert(self.path, {p1,p2})
		if n > 1 then
			local a = self.verts[6 * (n - 1) - 4]
			local b = {p1.x, p1.y, p1.z, 1}
			local c = {p2.x, p2.y, p2.z, 1}
			local d = self.verts[6 * (n - 1) - 3]
			table.insert(self.verts, a)
			table.insert(self.verts, b)
			table.insert(self.verts, c)
			table.insert(self.verts, a)
			table.insert(self.verts, c)
			table.insert(self.verts, d)
		elseif n == 1 then
			local q1,q2 = self.path[1][1],self.path[1][2]
			local a = {q1.x, q1.y, q1.z, 1}
			local b = {p1.x, p1.y, p1.z, 1}
			local c = {p2.x, p2.y, p2.z, 1}
			local d = {q2.x, q2.y, q2.z, 1}
			table.insert(self.verts, a)
			table.insert(self.verts, b)
			table.insert(self.verts, c)
			table.insert(self.verts, a)
			table.insert(self.verts, c)
			table.insert(self.verts, d)
		end
		-- Remove expired vertices.
		if n == l then
			rm()
		else
			n = n + 1
		end
	else
		rm()
		n = n - 1
	end
	if n == 0 then
		self.realized = false
		self.object.speedline = nil
	end
	-- Update transparency.
	local alpha = function(i) return (l-n+i)/l end
	local a1
	local a2 = alpha(0)
	for i = 1,n-1 do
		a1 = a2
		a2 = alpha(i)
		self.verts[6 * i - 5][4] = a2
		self.verts[6 * i - 4][4] = a1
		self.verts[6 * i - 3][4] = a1
		self.verts[6 * i - 2][4] = a2
		self.verts[6 * i - 1][4] = a1
		self.verts[6 * i][4] = a2
	end
	-- Rebuild the model.
	self.model:remove_vertices()
	self.model:add_triangles{material = 1, vertices = self.verts}
	self.model:changed()
end

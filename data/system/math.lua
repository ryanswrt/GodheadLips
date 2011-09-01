Aabb = Class()
Aabb.class_name = "Aabb"

--- Creates a new axis-aligned bounding box.
-- @param clss Aabb class.
-- @param args Arguments.<ul>
--   <li>point: Minimum point vector.</li>
--   <li>size: Size vector.</li></ul>
-- @return Aabb.
Aabb.new = function(clss, args)
	local self = Class.new(clss, args)
	self.point = self.point or Vector()
	self.size = self.size or Vector()
	return self
end

--- Return true if the AABB intersects with another AABB.
-- @param self Aabb.
-- @param aabb Aabb.
-- @return True if intersects.
Aabb.intersects = function(self, aabb)
	if self.point.x + self.size.x <= aabb.point.x then return end
	if self.point.y + self.size.y <= aabb.point.y then return end
	if self.point.z + self.size.z <= aabb.point.z then return end
	if self.point.x >= aabb.point.x + aabb.size.x then return end
	if self.point.y >= aabb.point.y + aabb.size.y then return end
	if self.point.z >= aabb.point.z + aabb.size.z then return end
	return true
end

--- Return true if the AABB intersects with a point.
-- @param self Aabb.
-- @param point Position vector.
-- @return True if intersects.
Aabb.intersects_point = function(self, point)
	if point.x < self.point.x then return end
	if point.y < self.point.y then return end
	if point.z < self.point.z then return end
	if point.x > self.point.x + self.size.x then return end
	if point.y > self.point.y + self.size.x then return end
	if point.z > self.point.z + self.size.x then return end
	return true
end

------------------------------------------------------------------------------

Quaternion = Class()
Quaternion.class_name = "Vector"

--- Creates a new quaternion.
-- @param clss Quaternion class.
-- @param args Arguments.<ul>
--   <li>1: X coordinate.</li>
--   <li>2: Y coordinate.</li>
--   <li>3: Z coordinate.</li>
--   <li>4: W coordinate.</li></ul>
-- @return New quaternion.

--- Creates a new quaternion.
-- @param clss Quaternion class.
-- @param args Arguments.<ul>
--   <li>1,x: Optional X value, default is 0.</li>
--   <li>2,y: Optional Y value, default is 0.</li>
--   <li>3,z: Optional Z value, default is 0.</li>
--   <li>4,w: Optional W value, default is 1.</li>
--   <li>angle: Angle of rotation in radians.</li>
--   <li>axis: Axis of rotation.</li>
--   <li>dir: Look direction vector.</li>
--   <li>euler: Euler angles.</li>
--   <li>up: Up direction vector.</li></ul>
-- @return New quaternion.
Quaternion.new = function(clss, x, y, z, w)
	local self = Class.new(clss)
	if type(x) == "table" then
		if x.euler then
			self.handle = Los.quaternion_new_euler(x.euler)
		elseif x.axis then
			self.handle = Los.quaternion_new{axis = x.axis.handle, angle = x.angle}
		else
			self.handle = Los.quaternion_new{dir = x.dir.handle, up = x.up.handle}
		end
	else
		self.handle = Los.quaternion_new(x, y, z, w)
	end
	__userdata_lookup[self.handle] = self
	return self
end

--- Calculates the sum of two quaternions.
-- @param self Quaternion.
-- @param quat Quaternion.
-- @return New quaternion.
Quaternion.__add = function(self, quat)
	local handle = Los.quaternion_add(self.handle, quat.handle)
	return Class.new(Quaternion, {handle = handle})
end

--- Multiplies the quaternion with another value.<br/>
-- The second value can be a scalar, a vector, or another quaternion.
-- If it is a scalar, all the components of the quaternion are multiplied by it.
-- If it is a vector, the vector is rotated by the quaternion, and
-- if it is another quaternion, the rotations of the quaternions are concatenated.
-- @param self Quaternion.
-- @param value Quaternion, vector, or number.
-- @return New quaternion or vector.
Quaternion.__mul = function(self, value)
	if type(value) == "number" then
		local handle = Los.quaternion_mul(self.handle, value)
		return Class.new(Quaternion, {handle = handle})
	elseif value.class == Quaternion then
		local handle = Los.quaternion_mul(self.handle, value.handle)
		return Class.new(Quaternion, {handle = handle})
	else
		local handle = Los.quaternion_mul(self.handle, value.handle)
		return Class.new(Vector, {handle = handle})
	end
end

--- Subtracts a quaternion from another.
-- @param self Quaternion.
-- @param quat Quaternion.
-- @return New quaternion.
Quaternion.__sub = function(self, quat)
	local handle = Los.quaternion_sub(self.handle, quat.handle)
	return Class.new(Quaternion, {handle = handle})
end

--- Converts the quaternion to a string.
-- @param self Quaternion.
-- @return String.
Quaternion.__str = function(self)
	return Los.quaternion_tostring(self.handle)
end

--- Normalized linear interpolation.
-- @param self Quaternion.
-- @param quat Quaternion.
-- @param blend Interpolation factor.
-- @return New quaternion.
Quaternion.nlerp = function(self, quat, blend)
	local handle = Los.quaternion_nlerp(self.handle, quat.handle, blend)
	return Class.new(Quaternion, {handle = handle})
end

--- Calculates the normalized form of the quaternion.
-- @param self Quaternion.
-- @return New quaternion.
Quaternion.normalize = function(self)
	local handle = Los.quaternion_normalize(self.handle)
	return Class.new(Quaternion, {handle = handle})
end

--- The conjugate of the quaternion.
-- @name Quaternion.conjugate
-- @class table

--- Euler angle presentation of the quaternion.
-- @name Quaternion.euler
-- @class table

--- Length.
-- @name Quaternion.length
-- @class table

--- W value.
-- @name Quaternion.w
-- @class table

--- X value.
-- @name Quaternion.x
-- @class table

--- Y value.
-- @name Quaternion.y
-- @class table

--- Z value.
-- @name Quaternion.z
-- @class table

Quaternion:add_getters{
	conjugate = function(s) return Class.new(Quaternion, {handle = Los.quaternion_get_conjugate(s.handle)}) end,
	euler = function(s) return Los.quaternion_get_euler(s.handle) end,
	length = function(s) return Los.quaternion_get_length(s.handle) end,
	w = function(s) return Los.quaternion_get_w(s.handle) end,
	x = function(s) return Los.quaternion_get_x(s.handle) end,
	y = function(s) return Los.quaternion_get_y(s.handle) end,
	z = function(s) return Los.quaternion_get_z(s.handle) end}

Quaternion:add_setters{
	length = function(s, v) Los.vector_set_length(s.handle, v) end,
	w = function(s, v) Los.quaternion_set_w(s.handle, v) end,
	x = function(s, v) Los.quaternion_set_x(s.handle, v) end,
	y = function(s, v) Los.quaternion_set_y(s.handle, v) end,
	z = function(s, v) Los.quaternion_set_z(s.handle, v) end}

Quaternion.unittest = function()
	-- XYZW presentation.
	local q1 = Quaternion(1,0,0,0)
	assert(q1.x == 1)
	assert(q1.length == 1)
	-- Axis-angle presentation.
	local q2 = Quaternion{axis = Vector(0,1,0), angle = math.pi}
	assert(q2.y == 1)
	assert(q2.length == 1)
	-- Look-at presentation.
	local q3 = Quaternion{dir = Vector(0,0,1), up = Vector(0,1,0)}
	assert(q3.y == -1)
	assert(q3.length == 1)
	-- Euler presentation.
	local q4 = Quaternion{euler = {0,0,1.5}}
	assert(q4.euler[1] == 0)
	assert(q4.euler[2] == 0)
	assert(q4.euler[3] == 1.5)
	-- Multiplication vectors.
	local q5 = q4 * q3
	assert(q5.class == Quaternion)
	local v1 = q4 * Vector(1,0,0)
	assert(v1.class == Vector)
end

------------------------------------------------------------------------------

Vector = Class()
Vector.class_name = "Vector"

--- Creates a new vector.
-- @param clss Vector class.
-- @param args Arguments.<ul>
--   <li>1: X coordinate.</li>
--   <li>2: Y coordinate.</li>
--   <li>3: Z coordinate.</li></ul>
-- @return New vector.
Vector.new = function(clss, x, y, z)
	local self = Class.new(clss)
	self.handle = Los.vector_new()
	__userdata_lookup[self.handle] = self
	Los.vector_set_x(self.handle, x)
	Los.vector_set_y(self.handle, y)
	Los.vector_set_z(self.handle, z)
	return self
end

--- Calculates the absolute of the vector.
-- @param self Vector.
Vector.abs = function(self)
	return Vector(math.abs(self.x), math.abs(self.y), math.abs(self.z))
end

--- Calculates the ceil of the vector.
-- @param self Vector.
Vector.ceil = function(self)
	return Vector(math.ceil(self.x), math.ceil(self.y), math.ceil(self.z))
end

--- Returns a copy of the vector.
-- @param self Vector.
Vector.copy = function(self)
	return Vector(self.x, self.y, self.z)
end

--- Calculates the floor of the vector.
-- @param self Vector.
Vector.floor = function(self)
	return Vector(math.floor(self.x), math.floor(self.y), math.floor(self.z))
end

--- Calculates the cross product of two vectors.
-- @param self Vector.
-- @param v Vector.
-- @return New vector.
Vector.cross = function(self, v)
	local handle = Los.vector_cross(self.handle, v.handle)
	return Class.new(Vector, {handle = handle})
end

--- Calculates the dot product of two vectors.
-- @param self Vector.
-- @param v Vector.
-- @return Scalar.
Vector.dot = function(self, v)
	return Los.vector_dot(self.handle, v.handle)
end

--- Returns a normalized copy of the vector.
-- @param self Vector.
-- @return Vector.
Vector.normalize = function(self)
	local handle = Los.vector_normalize(self.handle)
	return Class.new(Vector, {handle = handle})
end

--- Calculates the vector rounded to the nearest integers.
-- @param self Vector.
Vector.round = function(self)
	return Vector(math.ceil(self.x + 0.5), math.ceil(self.y + 0.5), math.ceil(self.z + 0.5))
end

--- Calculates the sum of two vectors.
-- @param self Vector.
-- @param v Vector.
-- @return New vector.
Vector.__add = function(self, v)
	local handle = Los.vector_add(self.handle, v.handle)
	return Class.new(Vector, {handle = handle})
end

--- Multiplies the vector by a scalar.
-- @param self Vector.
-- @param scalar Scalar.
-- @return New vector.
Vector.__mul = function(self, value)
	if type(value) == "number" then
		local handle = Los.vector_mul(self.handle, value)
		return Class.new(Vector, {handle = handle})
	elseif value.class == Quaternion then
		local handle = Los.quaternion_mul(value.handle, self.handle)
		return Class.new(Vector, {handle = handle})
	end
end

--- Converts the vector to a string.
-- @param self Vector.
-- @return String.
Vector.__str = function(self)
	return Los.vector_tostring(self.handle)
end

--- Subtracts a vector from another.
-- @param self Vector.
-- @param vector Vector.
-- @return New vector.
Vector.__sub = function(self, v)
	local handle = Los.vector_sub(self.handle, v.handle)
	return Class.new(Vector, {handle = handle})
end

--- Length.
-- @name Vector.length
-- @class table

--- X value.
-- @name Vector.x
-- @class table

--- Y value.
-- @name Vector.y
-- @class table

--- Z value.
-- @name Vector.z
-- @class table

Vector:add_getters{
	length = function(s) return Los.vector_get_length(s.handle) end,
	x = function(s) return Los.vector_get_x(s.handle) end,
	y = function(s) return Los.vector_get_y(s.handle) end,
	z = function(s) return Los.vector_get_z(s.handle) end}

Vector:add_setters{
	length = function(s, v) Los.vector_set_length(s.handle, v) end,
	x = function(s, v) Los.vector_set_x(s.handle, v) end,
	y = function(s, v) Los.vector_set_y(s.handle, v) end,
	z = function(s, v) Los.vector_set_z(s.handle, v) end}

Vector.unittest = function()
	-- Addition.
	local vec = Vector(1, 100) + Vector(5,5,5)
	assert(vec.x == 6 and vec.y == 105 and vec.z == 5)
	-- Normalization.
	assert(math.abs(1-vec:normalize().length) < 0.0001)
	-- Scalar multiplication.
	local a = Vector(1,-2,3) * 5
	assert(a.class == Vector)
	assert(math.abs(a.x - 5) < 0.0001)
	assert(math.abs(a.y + 10) < 0.0001)
	assert(math.abs(a.z - 15) < 0.0001)
	-- Dot product.
	local b = a:dot(a)
	assert(math.abs(b - 350) < 0.0001)
	-- Cross product.
	local c = Vector(1):cross(Vector(0,1))
	assert(math.abs(c.x) < 0.0001)
	assert(math.abs(c.y) < 0.0001)
	assert(math.abs(c.z - 1) < 0.0001)
	-- Protection from nan and inf.
	local d = Vector(0/0, -0/0, 1/0)
	assert(d.x == 0 and d.y == 0 and d.z == 0)
	d:normalize()
	assert(d.x == 0 and d.y == 0 and d.z == 0)
end

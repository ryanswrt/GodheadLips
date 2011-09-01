EditorObject = Class(Object)

EditorObject:add_getters{
	spec = function(s) return rawget(s, "__spec") end}

EditorObject:add_setters{
	spec = function(s, v)
		rawset(s, "__spec", v)
		s.model = v.model
		if v.models then
			local m = s.model:copy()
			for k,v in pairs(v.models) do
				if k ~= "skeleton" then
					m:merge(Model:find_or_load{file = v})
				end
			end
			m:calculate_bounds()
			m:changed()
			s.model = m
		end
		if v.type == "species" then
			s:animate{animation = "idle", channel = 1, permanent = true}
			s:update_animations{secs = 0}
			s:deform_mesh()
		end
	end}

EditorObject.move = function(self, value, step)
	self.position = self.position + value * step
end

EditorObject.snap = function(self, pstep, rstep)
	-- Position.
	local v = self.position + Vector(pstep,pstep,pstep) * 0.5
	v.x = v.x - v.x % pstep
	v.y = v.y - v.y % pstep
	v.z = v.z - v.z % pstep
	self.position = v
	-- Rotation.
	local r = self.rotation.euler[1] + 0.5 * rstep
	self.rotation = Quaternion{euler = {r - r % rstep, 0, 0}}
end

--- Rotates the object with step snapping.
-- @param self Object.
-- @param value Step size.
-- @param steps Number of steps per revolution.
EditorObject.rotate = function(self, value, steps)
	local s = 2 * math.pi / steps
	local e = self.rotation.euler
	e[1] = math.floor(e[1] / s + value + 0.5) * s
	self.rotation = Quaternion{euler = e}
end

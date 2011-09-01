Region = Class()

Region.new = function(clss, args)
	local self = Class.new(clss, args)
	self.links = {}
	self.linked_regions = {}
	return self
end

Region.create_link = function(self, region)
	local link = {self, region}
	table.insert(self.links, link)
	table.insert(region.links, link)
	self.linked_regions[region] = true
	return link
end

--- Gets the depth layer range of the region.
-- @param self Region.
-- @return First layer, last layer.
Region.get_layer_range = function(self)
	local layer = (self.point.y - Generator.layer_offset) / Generator.layer_size
	local min = math.max(1, layer)
	local max = math.max(0, math.min(Generator.layer_count, layer + 2))
	return math.floor(min), math.ceil(max) 
end

Region.get_link_point = function(self, dst)
	local ctr = self.point + self.size * 0.5
	local dir = (dst - ctr):normalize()
	if math.abs(dir.x) < math.abs(dir.z) then
		dir.y = -1
		dir.z = dir.z < 0 and -1 or 1
	else
		dir.x = dir.x < 0 and -1 or 1
		dir.y = -1
	end
	local pt = ctr + Vector(dir.x * self.size.x, dir.y * self.size.y, dir.z * self.size.z) * 0.5 + Vector(0,3)
	return pt:floor()
end

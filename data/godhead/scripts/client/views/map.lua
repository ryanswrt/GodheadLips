Views.Map = Class(Widget)
Views.Map.class_name = "Views.Map"

--- Initializes the map UI.
-- @param self Map view class.
-- @return Map view.
Views.Map.new = function(clss)
	local self = Widget.new(clss, {cols = 1, rows = 3, spacings = {0, 0}})
	self.title = Widgets.Title{text = "Map",
		back = function() self:back() end,
		close = function() Client:set_mode("game") end,
		help = function() Client.views.help:show("map") end}
	self.frame = Widgets.Frame{style = "default", request = Vector(400, 400)}
	self.frame.scrolled = function(w, args)
		if args.button == 4 then
			self.scale = self.scale * 1.5
		else
			self.scale = self.scale / 1.5
		end
		self:update()
		return true
	end
	self:set_child(1, 1, self.title)
	self:set_child(1, 2, self.frame)
	self.scale = 1
	return self
end

--- Closes the map view.
-- @param self Map view.
Views.Map.back = function(self)
	Client:set_mode("menu")
end

--- Closes the map view.
-- @param self Map view.
Views.Map.close = function(self)
end

Views.Map.enter = function(self, from, level)
	self:update()
	Gui.menus:open{level = level, widget = self}
end

Views.Map.update = function(self)
	local size = Vector(400,400)
	local center = Client.player_object.position
	local add = function(icon, name, pos, rot)
		local icon = Iconspec:find{name = icon}
		local local_pos = (Vector(pos.x, pos.z) - Vector(center.x, center.z))
		local pixel_pos = local_pos*Voxel.tile_scale * self.scale + size*0.5 - Vector(icon.size)*0.5
		if pixel_pos.x < 0 or pixel_pos.y < 0 or pixel_pos.x >= size.x or pixel_pos.y >= size.y then return end
		local tooltip = string.format("%s\nDistance: %d\nDepth: %d", name, local_pos.length, pos.y - center.y)
		local widget = Widgets.Icon{icon = icon, offset = pixel_pos,
			rotation = rot, tooltip = Widgets.Tooltip{text = tooltip}}
		self.frame:add_child(widget)
		table.insert(self.markers, widget)
	end
	-- Remove old markers.
	if self.markers then
		for k,v in pairs(self.markers) do v:detach() end
	end
	-- Create new markers.
	local rot = Client.player_object.rotation.euler
	self.markers = {}
	add("mapmarker2", "player", center, rot[1])
	for k,v in pairs(Marker.dict_name) do
		add("mapmarker1", k, v.position, 0)
	end
end

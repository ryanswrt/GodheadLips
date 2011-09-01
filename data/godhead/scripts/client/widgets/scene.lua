Widgets.Scene = Class(Widget)

Widgets.Scene.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.scene = Scene()
	self.fixed_size = true
	return self
end

Widgets.Scene.pick = function(self)
	return Target:pick_scene()
end

Widgets.Scene.pressed = function(self)
	if not Drag or not Drag:clicked_scene() then
		Target:select_scene()
	end
end

Widgets.Scene.render = function(self)
	local postproc_enabled = Views and Client.views.options.bloom_enabled
	local render_passes = {
		{pass = 1}, -- Depth pass
		{pass = 4}, -- Opaque pass
		{pass = 6, sorting = true}} -- Transparent pass
	self.camera.viewport = {self.x, self.y, self.width, self.height}
	self.scene:render{
		hdr = Views and Client.views.options.bloom_enabled,
		modelview = self.camera.modelview,
		multisamples = Views and Client.views.options.multisamples,
		projection = self.camera.projection,
		viewport = self.camera.viewport,
		render_passes = render_passes,
		postproc_passes = postproc_enabled and {{mipmaps = true, shader = "postprocess-hdr"}}}
	Speech:draw{
		modelview = self.camera.modelview,
		projection = self.camera.projection,
		viewport = self.camera.viewport}
end

Widgets.Scene.reshaped = function(self)
	local w = self.width
	local h = self.height
	self:canvas_clear()
	if self.compass then
		self:canvas_image{
			dest_position = {6,h-77},
			dest_size = {74,74},
			rotation = self.compass,
			rotation_center = Vector(44,h-40),
			source_image = "compass1",
			source_position = {42,2},
			source_tiling = {0,74,0,0,74,0}}
		if self.compass_quest_direction then
			local frac = 0.6 + 0.4 * math.min(1,self.compass_quest_distance/(50*Voxel.tile_size))
			self:canvas_image{
				dest_position = {38,h-44-33*frac},
				dest_size = {12,50},
				rotation = self.compass_quest_direction + self.compass + math.pi,
				rotation_center = Vector(44,h-40),
				source_image = "compass1",
				source_position = {0,0},
				source_tiling = {0,12,0,0,50,0}}
		end
		if self.compass_quest_height then
			self:canvas_text{
				dest_position = {6,h-77},
				dest_size = {74,74},
				text = string.format("%+d", self.compass_quest_height),
				text_alignment = {0.5,0.5},
				text_color = {1,1,1,1},
				text_font = "tiny"}
		end
	end
	if self.action then
		local o = 20
		self:canvas_text{
			dest_position = {2,o+2},
			dest_size = {w,o},
			text = self.action,
			text_alignment = {0.5,0},
			text_color = {0,0,0,1},
			text_font = "medium"}
		self:canvas_text{
			dest_position = {0,o},
			dest_size = {w,o},
			text = self.action,
			text_alignment = {0.5,0},
			text_color = {1,1,1,1},
			text_font = "medium"}
	end
	self:canvas_compile()
end

Widgets.Scene:add_getters{
	action = function(s) return rawget(s, "__action") end,
	compass = function(s) return rawget(s, "__compass") end,
	compass_quest = function(s) return rawget(s, "__compass_quest") end}

Widgets.Scene:add_setters{
	action = function(s, v)
		if s.action == v then return end
		rawset(s, "__action", v)
		s:reshaped()
	end,
	compass = function(s, v)
		if s.compass == v then return end
		rawset(s, "__compass", v)
		s:reshaped()
	end,
	compass_quest = function(s, v)
		if s.compass_quest == v then return end
		rawset(s, "__compass_quest", v)
		s:reshaped()
	end}

Widgets.Icon = Class(Widget)
Widgets.Icon.DEFAULT = {image="icons1", offset={0,0}, size={34,34}}

Widgets.Icon.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.icon = self.icon or clss.DEFAULT
	return self
end

Widgets.Icon.reshaped = function(self)
	local icon = self.icon or self.DEFAULT
	self:set_request{
		internal = true,
		width = icon.size[1],
		height = icon.size[2]}
	local w = self.width
	local h = self.height
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,0},
		dest_size = icon.size,
		rotation = self.rotation,
		rotation_center = Vector(icon.size[1],icon.size[2])*0.5,
		source_image = icon.image,
		source_position = icon.offset,
		source_tiling = {0,icon.size[1],0,0,icon.size[2],0}}
	self:canvas_compile()
end

Widgets.Icon.pressed = function(self, args)
end

Widgets.Icon:add_getters{
	icon = function(self) return rawget(self, "__icon") end}

Widgets.Icon:add_setters{
	icon = function(self, value)
		if self.icon == value then return end
		rawset(self, "__icon", value)
		self:reshaped()
	end}

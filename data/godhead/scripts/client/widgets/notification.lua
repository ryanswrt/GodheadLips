Widgets.Notification = Class(Widget)
Widgets.Notification.class_name = "Widgets.Notification"

Widgets.Notification.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.width_request = 200
	return self
end

Widgets.Notification.reshaped = function(self)
	self:set_request{
		font = "default",
		internal = true,
		text = self.text,
		paddings = {5,16,0,11},
		width = self.width_request}
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {self.width,self.height},
		source_image = "widgets1",
		source_position = {720,0},
		source_tiling = {10,80,21,10,30,21}}
	self:canvas_text{
		dest_position = {5,3},
		dest_size = {self.width-15,self.height},
		text = self.text,
		text_alignment = {0,0},
		text_color = {1,1,1,1},
		text_font = "default"}
	self:canvas_compile()
end

Widgets.Notification.update = function(self, secs)
	if not self.visible then return end
	if self.text and self.timer < 5 then
		-- Show.
		local mode = Program.video_mode
		local off = 1
		if self.timer < 0.5 then off = 2 * self.timer end
		if self.timer > 4.5 then off = 2 * (5 - self.timer) end
		self.offset = Vector(mode[1], mode[2]) - Vector(self.width, self.height * off)
		self.timer = self.timer + secs
	else
		-- Hide.
		self.timer = nil
		self.visible = false
	end
end

Widgets.Notification:add_getters{
	text = function(self) return rawget(self, "__text") end}

Widgets.Notification:add_setters{
	text = function(self, v)
		if self.text == v then return end
		rawset(self, "__text", v)
		self.timer = 0
		self:reshaped()
		self.visible = true
	end}

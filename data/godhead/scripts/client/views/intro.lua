Views.Intro = Class(Widgets.Scene)
Views.Intro.class_name = "Views.Intro"

--- Creates a new intro view.
-- @param clss Intro class.
-- @return Intro.
Views.Intro.new = function(clss)
	-- Preview scene.
	local camera = Camera{far = 60.0, near = 0.3, mode = "first-person"}
	local self = Widgets.Scene.new(clss, {cols = 1, rows = 2, behind = true, fullscreen = true, camera = camera, spacings = {0,0}})
	self.offset = Vector(0, 1.8, -10)
	self.light = Light{ambient = {0.5,0.5,0.5,1}, diffuse = {1,1,1,1}, equation = {1,0.1,0.03}}
	self.timer = Timer{enabled = false, func = function(timer, secs) self:update(secs) end}
	self.text = Widgets.Label{halign = 0.5, valign = 0.5, font = "medium"}
	self.text:set_request{width = 800, height = 200}
	self:set_expand{row = 1, col = 1}
	self:set_child(1, 2, self.text)
	return self
end

Views.Intro.close = function(self)
	self.floating = false
	self.object.realized = false
	self.timer.enabled = false
	self.light.enabled = false
end

--- Executes the intro view.
-- @param self Intro.
-- @param from Name of the previous mode.
Views.Intro.enter = function(self, from)
	-- Show the scene.
	self.floating = true
	self.timer.enabled = true
	self.light.enabled = true
	Sound:switch_music_track("intro")
	self:show_scene(1)
end

Views.Intro.pressed = function(self, args)
	if self.scene_number < 4 then
		self:show_scene(self.scene_number + 1)
	else
		Client:set_mode("login")
	end
end

Views.Intro.show_scene = function(self, number)
	self.scene_number = number
	if number == 1 then
		self.object = Object{model = "tree1", position = Vector(1, 1, 1), type = "character"}
		self.object.realized = true
		self.camera.target_position = self.object.position + self.offset
		self.camera.target_rotation = Quaternion{axis = Vector(0, 1, 0), angle = math.pi}
		self.light.position = self.object.position + self.object.rotation * Vector(0, 2, -5)
		self.camera:warp()
		self.text.text = "TODO 1"
	elseif number == 2 then
		self.text.text = "TODO 2"
	elseif number == 3 then
		self.text.text = "TODO 3"
	elseif number == 4 then
		self.text.text = "TODO 4"
	end
end

Views.Intro.update = function(self, secs)
	self.object:refresh()
	self.camera.viewport = {self.x, self.y, self.width, self.height}
	self.camera:update(secs)
end

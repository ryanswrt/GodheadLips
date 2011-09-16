Views.Chargen = Class(Widgets.Scene)
Views.Chargen.class_name = "Views.Chargen"
Views.Chargen.list_races = {
	{"Aer", "aer"}
}

-- Set to true to dump the settings to the console in a copiable form.
Views.Chargen.dump_presets = false
Views.Chargen.list_presets = {
	{name="Female 1",body_style={[1]=0,[2]=0.5,[3]=0.5,[4]=0.76635514018692,[5]=0.032710280373832,[6]=0.023364485981308,[7]=0.0046728971962617,[8]=0.55607476635514},skin_color={[1]=1,[2]=1,[3]=1},eye_color={[1]=0.70740740740741,[2]=0.97037037037037,[3]=0.49259259259259},body_scale=0.91120294158403,hair_color={[1]=0.70740740740741,[2]=0.5037037037037,[3]=0.34814814814815},face_style={[1]=0.68303312579311,[2]=0.39000927069691,[3]=0.5925592596608,[4]=0.71896150043186,[5]=0.019047619047619,[6]=0.4952380952381,[7]=0.019047619047619,[8]=0.23809523809524,[9]=0.015353406786618,[10]=0.51618284011082,[11]=0.2371160533452,[12]=0.57142857142857,[13]=0.65562895250303,[14]=0.033333333333333,[15]=0.82454440362032},hair_style="hair4"},
	{name="Female 2",body_style={[1]=0.2196261682243,[2]=0.5,[3]=0.5,[4]=0.76635514018692,[5]=0.20093457943925,[6]=0.33644859813084,[7]=0.46261682242991,[8]=0.60280373831776},skin_color={[1]=1,[2]=1,[3]=1},eye_color={[1]=0.70740740740741,[2]=0.97037037037037,[3]=0.49259259259259},body_scale=0.99065420560748,hair_color={[1]=0.70740740740741,[2]=0.5037037037037,[3]=0.34814814814815},face_style={[1]=0.31696687420689,[2]=0.64285714285714,[3]=0.5047619047619,[4]=0.78095238095238,[5]=0.50952380952381,[6]=0.41904761904762,[7]=0.019047619047619,[8]=0.37142857142857,[9]=0.25714285714286,[10]=0.48381715988918,[11]=0.48571428571429,[12]=0.57142857142857,[13]=0.65562895250303,[14]=0.33333333333333,[15]=0.82454440362032},hair_style="hair1"},
	{name="Female 3",body_style={[1]=0.2196261682243,[2]=0.66355140186916,[3]=0.14018691588785,[4]=0.76635514018692,[5]=0.20093457943925,[6]=0.2196261682243,[7]=0.12616822429907,[8]=0.54205607476636},skin_color={[1]=1,[2]=1,[3]=1},eye_color={[1]=0.97037037037037,[2]=0.97037037037037,[3]=0.20740740740741},body_scale=1.0485981308411,hair_color={[1]=0.70740740740741,[2]=0.5037037037037,[3]=0.34814814814815},face_style={[1]=0.30952380952381,[2]=0.50952380952381,[3]=0.69047619047619,[4]=0.62857142857143,[5]=0.31428571428571,[6]=0.41904761904762,[7]=0.1952380952381,[8]=0.061904761904762,[9]=0.25714285714286,[10]=0.48381715988918,[11]=0.15714285714286,[12]=0.57142857142857,[13]=0.51904761904762,[14]=0.0095238095238095,[15]=0.17619047619048},hair_style="hair5"},
	{name="Male 1",body_style={[1]=0.44392523364486,[2]=0.5607476635514,[3]=0.018691588785047,[4]=0.023364485981308,[5]=0.78504672897196,[6]=0.73831775700935,[7]=0.50934579439252,[8]=0.73364485981308},skin_color={[1]=1,[2]=1,[3]=1},eye_color={[1]=0.70740740740741,[2]=0.18888888888889,[3]=0.49259259259259},body_scale=0.91120294158403,hair_color={[1]=0.70740740740741,[2]=0.5037037037037,[3]=0.34814814814815},face_style={[1]=0.64285714285714,[2]=0.78571428571429,[3]=0.2,[4]=0.48571428571429,[5]=0.12857142857143,[6]=0.5047619047619,[7]=0.17142857142857,[8]=0.50952380952381,[9]=0.42380952380952,[10]=0.43333333333333,[11]=0.63333333333333,[12]=0.57142857142857,[13]=0.77142857142857,[14]=0.68095238095238,[15]=0.24761904761905},hair_style="hair5"},
	{name="Male 2",body_style={[1]=0.33177570093458,[2]=0.45327102803738,[3]=0.0093457943925234,[4]=0.0093457943925234,[5]=0.36854397941779,[6]=0.91121495327103,[7]=0.18224299065421,[8]=0.88317757009346},skin_color={[1]=1,[2]=1,[3]=1},eye_color={[1]=0.98758912830967,[2]=0.50854464038673,[3]=0.50543860742144},body_scale=0.97416166254513,hair_color={[1]=0.19428798472243,[2]=0.68148148148148,[3]=0.81481481481481},face_style={[1]=0.095238095238095,[2]=0.44285714285714,[3]=0.12857142857143,[4]=0.97711486973665,[5]=0.070011058389214,[6]=0.27460515605035,[7]=0.74761904761905,[8]=0.1952380952381,[9]=0.44866463469745,[10]=0.36289419623227,[11]=0.78943752580762,[12]=0.28424429627333,[13]=0.35238095238095,[14]=0.91790563423089,[15]=0.65714285714286},hair_style="hair5"},
	{name="Male 3",body_style={[1]=0.63084112149533,[2]=0.42056074766355,[3]=0.018691588785047,[4]=0.023364485981308,[5]=0.78504672897196,[6]=0.97196261682243,[7]=0.13084112149533,[8]=0.77570093457944},skin_color={[1]=1,[2]=1,[3]=1},eye_color={[1]=0.95925925925926,[2]=0.57777777777778,[3]=0.25185185185185},body_scale=1.0366804412376,hair_color={[1]=0.36666666666667,[2]=0.53703703703704,[3]=0.9037037037037},face_style={[1]=0.64285714285714,[2]=0.78571428571429,[3]=0.0047619047619048,[4]=0.84761904761905,[5]=0.12857142857143,[6]=0.35238095238095,[7]=0.17142857142857,[8]=0.63809523809524,[9]=0.42380952380952,[10]=0.25238095238095,[11]=0.5952380952381,[12]=0.15714285714286,[13]=0.9952380952381,[14]=0.98095238095238,[15]=0.24761904761905},hair_style="hair5"}}

Views.Chargen.list_spawnpoints={{"Tutorial","lyra"},{"Lips Town","mentor"},{"Home (if you've made one)","home"}}

--- Creates a new chargen view.
-- @param clss Chargen class.
-- @return Chargen.
Views.Chargen.new = function(clss)
	-- Preview scene.
	local camera = Camera{far = 60.0, near = 0.3, mode = "first-person"}
	local self = Widgets.Scene.new(clss, {rows = 1, behind = true, fullscreen = true, camera = camera, spacings = {0,0}})
	self.view_offset = Vector(0, 1.8, -2)
	self.margins = {5,5,5,5}
	self.skills = Widgets.Skills{changed = function(widget, skill)
		skill.value = skill.cap
	end}
	self.object = Creature{position = Vector(1, 1, 1)}
	self.light = Light{ambient = {1.0,1.0,1.0,1.0}, diffuse = {1.0,1.0,1.0,1.0}, equation = {2,0.3,0.03}}
	self.timer = Timer{enabled = false, func = function(timer, secs) self:update(secs) end}
	self.list_hair_styles = {}
	self.list_eye_styles = {}
	self.list_skin_styles = {}
	-- Spawn Point Selector
	local spawnpoints = {}
	for k,v in ipairs(self.list_spawnpoints) do
		table.insert(spawnpoints, {v[1], function() end})
	end
	self.label_spawn = Widgets.Label{text="Spawn Point:"}
	self.combo_spawn = Widgets.ComboBox(spawnpoints)
	--self.combo_spawn.activated = function() self.list_spawnpoints[self.combo_spawn.value][2]() end
	-- Character name.
	self.label_name = Widgets.Label{text = "Name:"}
	self.entry_name = Widgets.Entry()

	-- Race selector.
	local races = {}
	for k,v in ipairs(self.list_races) do
		table.insert(races, {v[1], function() self:set_race(k) end})
	end
	self.label_race = Widgets.Label{text = "Race:"}
	self.label_race:set_request{width = 100}
	self.combo_race = Widgets.ComboBox(races)
	-- Eye style and color selectors.
	self.label_eye_style = Widgets.Label{text = "Eyes:"}
	self.combo_eye_style = Widgets.ComboBox()
	self.label_eye_color = Widgets.Label{text = "  Color:"}
	self.color_eye = Widgets.ColorSelector{pressed = function(widget, point)
		Widgets.ColorSelector.pressed(widget, point)
		self:update_model()
	end}
	-- Hair style and color selectors.
	self.label_hair_style = Widgets.Label{text = "Hair:"}
	self.combo_hair_style = Widgets.ComboBox()
	self.label_hair_color = Widgets.Label{text = "  Color:"}
	self.color_hair = Widgets.ColorSelector{pressed = function(widget, point)
		Widgets.ColorSelector.pressed(widget, point)
		self:update_model()
	end}
	-- Skin style and color selectors.
	self.label_skin_style = Widgets.Label{text = "Skin:"}
	self.combo_skin_style = Widgets.ComboBox()
	self.label_skin_color = Widgets.Label{text = "  Color:"}
	self.color_skin = Widgets.ColorSelector{pressed = function(widget, point)
		Widgets.ColorSelector.pressed(widget, point)
		self:update_model()
	end}
	-- Apply and quit buttons.
	self.button_create = Widgets.Button{text = "Create", pressed = function() self:apply() end}
	self.button_create:set_request{height = 40}
	self.button_quit = Widgets.Button{text = "Quit", pressed = function() self:quit() end}
	-- Body apperance.
	self.label_body = {}
	self.scroll_body = {}
	local body_slider_names = {
		"Arm muscularity", "Body width", "Bust size", "Hips width", "Leg muscularity",
		"Torso width", "Waist fatness", "Waist width"}
	local body_slider_inverse = {
		false, true, false, false, false, false, false, false}
	self.group_body = Widgets.Frame{rows = 1 + #body_slider_names, cols = 2, spacings = {0,2}}
	self.group_body:set_expand{col = 2}
	for k,v in ipairs(body_slider_names) do
		local label = Widgets.Label{text = v .. ":"}
		local slider = Widgets.Progress{min = 0, max = 1, value = 0, inverse = body_slider_inverse[k],
			pressed = function(widget) self:set_body(k, widget:get_value_at(Program.cursor_position)) end}
		self.label_body[k] = label
		self.scroll_body[k] = slider
		self.group_body:set_child(1, k + 1, label)
		self.group_body:set_child(2, k + 1, slider)
	end
	self.label_height = Widgets.Label{text = "Height:"}
	self.scroll_height = Widgets.Progress{min = 0, max = 1, value = 1,
		pressed = function(widget) self:set_height(widget:get_value_at(Program.cursor_position)) end}
	self.group_body:set_child(1, 1, self.label_height)
	self.group_body:set_child(2, 1, self.scroll_height)
	-- Face appearance.
	self.label_face = {}
	self.scroll_face = {}
	local face_slider_names = {
		"Cheekbone size", "Cheek size", "Chin sharpness", "Chin size",
		"Eye inner", "Eye distance", "Eye outer", "Eye size",
		"Face roughness", "Jaw pronounced", "Jaw width", "Lips protruded",
		"Mouth width", "Nose pointedness", "Nose position"}
	local face_slider_inverse = {
		true, true, false, true, false, true, false, true,
		false, true, false, false, false, true, false}
	self.group_face = Widgets.Frame{rows = #face_slider_names, cols = 2, spacings = {0,2}}
	self.group_face:set_expand{col = 2}
	for k,v in ipairs(face_slider_names) do
		local label = Widgets.Label{text = v .. ":"}
		local slider = Widgets.Progress{min = 0, max = 1, value = 0, inverse = face_slider_inverse[k],
			pressed = function(widget) self:set_face(k, widget:get_value_at(Program.cursor_position)) end}
		self.label_face[k] = label
		self.scroll_face[k] = slider
		self.group_face:set_child(1, k, label)
		self.group_face:set_child(2, k, slider)
	end
	-- Appearance presets.
	self.group_presets = Widgets.Frame{cols = 1, rows = #self.list_presets, spacings = {0,2}}
	self.group_presets:set_expand{col = 1}
	self.toggle_presets = {}
	for k,v in ipairs(self.list_presets) do
		local widget = Widgets.Toggle{text = v.name, preset = v, pressed = function(w)
			if self.active_preset then self.active_preset.active = false end
			self.active_preset = w
			w.active = true
			self:set_preset(w.preset)
		end}
		table.insert(self.toggle_presets, widget)
		self.group_presets:set_child(1, k, widget)
	end
	-- Miscellaneous apperance.
	self.group_hair = Widget{rows = 1, cols = 2, homogeneous = true}
	self.group_hair:set_child{row = 1, col = 1, widget = self.combo_hair_style}
	self.group_hair:set_child{row = 1, col = 2, widget = self.combo_hair_color}
	self.group_hair:set_expand{col = 1}
	self.group_hair:set_expand{col = 2}
	self.group_race = Widget{rows = 2, cols = 2, spacings = {0,2}}
	self.group_race:set_child{row = 1, col = 1, widget = self.label_name}
	self.group_race:set_child{row = 1, col = 2, widget = self.entry_name}
	self.group_race:set_child{row = 2, col = 1, widget = self.label_race}
	self.group_race:set_child{row = 2, col = 2, widget = self.combo_race}
	self.group_race:set_expand{col = 2}
	self.group_race1 = Widgets.Frame{rows = 2, cols = 1}
	self.group_race1:set_child{row = 1, col = 1, widget = self.group_race}
	self.group_race1:set_child{row = 2, col = 1, widget = self.skills}
	self.group_appearance = Widgets.Frame{rows = 6, cols = 2, spacings = {0,2}}
	self.group_appearance:set_expand{col = 2}
	self.group_appearance:set_child{row = 1, col = 1, widget = self.label_eye_style}
	self.group_appearance:set_child{row = 1, col = 2, widget = self.combo_eye_style}
	self.group_appearance:set_child{row = 2, col = 1, widget = self.label_eye_color}
	self.group_appearance:set_child{row = 2, col = 2, widget = self.color_eye}
	self.group_appearance:set_child{row = 3, col = 1, widget = self.label_hair_style}
	self.group_appearance:set_child{row = 3, col = 2, widget = self.combo_hair_style}
	self.group_appearance:set_child{row = 4, col = 1, widget = self.label_hair_color}
	self.group_appearance:set_child{row = 4, col = 2, widget = self.color_hair}
	self.group_appearance:set_child{row = 5, col = 1, widget = self.label_skin_style}
	self.group_appearance:set_child{row = 5, col = 2, widget = self.combo_skin_style}
	self.group_appearance:set_child{row = 6, col = 1, widget = self.label_skin_color}
	self.group_appearance:set_child{row = 6, col = 2, widget = self.color_skin}
	-- Appearance tab selector.
	self.group_tabs = Widgets.Frame{rows = 2, cols = 3, spacings = {0,2}}
	self.group_tabs:set_expand{col = 1}
	self.group_tabs:set_expand{col = 2}
	self.group_tabs:set_expand{col = 3}
	self.toggle_appearance_presets = Widgets.Toggle{active = true, text = "Presets", pressed = function(w)
		w.active = true
		self.toggle_appearance_body.active = false
		self.toggle_appearance_face.active = false
		self.toggle_appearance_misc.active = false
		self.group_right:set_child(1, 3, self.group_presets)
	end}
	self.toggle_appearance_body = Widgets.Toggle{text = "Body", pressed = function(w)
		w.active = true
		self.toggle_appearance_presets.active = false
		self.toggle_appearance_face.active = false
		self.toggle_appearance_misc.active = false
		self.group_right:set_child(1, 3, self.group_body)
	end}
	self.toggle_appearance_face = Widgets.Toggle{text = "Face", pressed = function(w)
		w.active = true
		self.toggle_appearance_presets.active = false
		self.toggle_appearance_body.active = false
		self.toggle_appearance_misc.active = false
		self.group_right:set_child(1, 3, self.group_face)
	end}
	self.toggle_appearance_misc = Widgets.Toggle{text = "Misc", pressed = function(w)
		w.active = true
		self.toggle_appearance_presets.active = false
		self.toggle_appearance_body.active = false
		self.toggle_appearance_face.active = false
		self.group_right:set_child(1, 3, self.group_appearance)
	end}
	self.group_tabs:set_child(1, 1, self.toggle_appearance_presets)
	self.group_tabs:set_child(2, 1, self.toggle_appearance_body)
	self.group_tabs:set_child(3, 1, self.toggle_appearance_face)
	self.group_tabs:set_child(1, 2, self.toggle_appearance_misc)
	-- Packing.
	self.group_spawn = Widget{rows = 1, cols =2, margins={0,0,0,0}}
	self.group_spawn:set_child{row=1,col=1,widget=self.label_spawn}
	self.group_spawn:set_child{row=1,col=2,widget=self.combo_spawn}
	self.group_buttons = Widget{rows = 3, cols = 1, margins = {0,0,5,5}}
	self.group_buttons:set_child{row = 1, col = 1, widget = self.group_spawn}
	self.group_buttons:set_child{row = 2, col = 1, widget = self.button_create}
	self.group_buttons:set_child{row = 3, col = 1, widget = self.button_quit}
	self.group_buttons:set_expand{col = 1}
	self.group_left = Widget{cols = 1, spacings = {0,0}}
	self.group_left:append_row(Widgets.Frame{style = "title", text = "Character"})
	self.group_left:append_row(self.group_race1)
	self.group_left:append_row(self.group_buttons)
	self.group_left:set_expand{col = 1}
	self.group_right = Widget{cols = 1, rows = 3, spacings = {0,0}}
	self.group_right:set_child(1, 1, Widgets.Frame{style = "title", text = "Apperance"})
	self.group_right:set_child(1, 2, self.group_tabs)
	self.group_right:set_child(1, 3, self.group_presets)
	self.group_right:set_expand{col = 1}
	self:append_col(self.group_left)
	self:append_col(nil)
	self:append_col(self.group_right)
	self:set_expand{row = 1, col = 2}
	return self
end

Views.Chargen.apply = function(self)
	local scroll_value = function(scroll)
		return 255 * (scroll.inverse and (1 - scroll.value) or scroll.value)
	end
	local packet = Packet(packets.CHARACTER_CREATE,
		"string", self.entry_name.text,
		"string", self.list_races[self.combo_race.value][2],
		-- Skills.
		"uint8", self.skills:get_value("dexterity"),
		"uint8", self.skills:get_value("health"),
		"uint8", self.skills:get_value("intelligence"),
		"uint8", self.skills:get_value("perception"),
		"uint8", self.skills:get_value("strength"),
		"uint8", self.skills:get_value("willpower"),
		-- Body style.
		"uint8", scroll_value(self.scroll_height),
		"uint8", scroll_value(self.scroll_body[1]),
		"uint8", scroll_value(self.scroll_body[2]),
		"uint8", scroll_value(self.scroll_body[3]),
		"uint8", scroll_value(self.scroll_body[4]),
		"uint8", scroll_value(self.scroll_body[5]),
		"uint8", scroll_value(self.scroll_body[6]),
		"uint8", scroll_value(self.scroll_body[7]),
		"uint8", scroll_value(self.scroll_body[8]),
		-- Eye style.
		"string", "default",
		"uint8", 255 * self.color_eye.red,
		"uint8", 255 * self.color_eye.green,
		"uint8", 255 * self.color_eye.blue,
		-- Face style.
		"uint8", scroll_value(self.scroll_face[1]),
		"uint8", scroll_value(self.scroll_face[2]),
		"uint8", scroll_value(self.scroll_face[3]),
		"uint8", scroll_value(self.scroll_face[4]),
		"uint8", scroll_value(self.scroll_face[5]),
		"uint8", scroll_value(self.scroll_face[6]),
		"uint8", scroll_value(self.scroll_face[7]),
		"uint8", scroll_value(self.scroll_face[8]),
		"uint8", scroll_value(self.scroll_face[9]),
		"uint8", scroll_value(self.scroll_face[10]),
		"uint8", scroll_value(self.scroll_face[11]),
		"uint8", scroll_value(self.scroll_face[12]),
		"uint8", scroll_value(self.scroll_face[13]),
		"uint8", scroll_value(self.scroll_face[14]),
		"uint8", scroll_value(self.scroll_face[15]),
		-- Hair style.
		"string", self.list_hair_styles[self.combo_hair_style.value][2],
		"uint8", 255 * self.color_hair.red,
		"uint8", 255 * self.color_hair.green,
		"uint8", 255 * self.color_hair.blue,
		-- Skin style.
		"string", "default",
		"uint8", 255 * self.color_skin.red,
		"uint8", 255 * self.color_skin.green,
		"uint8", 255 * self.color_skin.blue,
		--Spawnpoint
		"string", self.list_spawnpoints[self.combo_spawn.value][2])
	Network:send{packet = packet}
end

Views.Chargen.close = function(self)
	self.floating = false
	self.object.realized = false
	self.timer.enabled = false
	self.light.enabled = false
end

--- Executes the character generator.
-- @param self Chargen.
-- @param from Name of the previous mode.
Views.Chargen.enter = function(self, from)
	-- Cleanup if disconnected.
	if from ~= "startup" then
		for k,v in pairs(Object.objects) do v:detach() end
		Player.object = nil
	end
	-- Show the UI.
	self.floating = true
	self.object.realized = true
	self.timer.enabled = true
	self.light.enabled = true
	self:random()
	self:update(0.0)
	self.camera:warp()
	Sound:switch_music_track("char")
end

Views.Chargen.pressed = function(self, args)
	if args.button == 1 then
		self.dragging = true
	end
end

Views.Chargen.event = function(self, args)
	if args.type == "mouserelease" then
		if args.button == 1 then
			self.dragging = nil
		end
	elseif args.type == "mousemotion" then
		if self.dragging then
			local y = self.view_offset.y + args.dy / 300
			self.view_offset.y = math.min(math.max(y, 1), 2)
			self:rotate(math.pi * args.dx / 300)
		end
	end
end

Views.Chargen.quit = function(self)
	Program.quit = true
end

--- Randomizes the character.
-- @param self Chargen.
Views.Chargen.random = function(self)
	self:set_race(math.random(1, #self.list_races))
	self.update_needed = true
end

--- Rotates the character.
-- @param self Chargen.
-- @param rad Radians.
Views.Chargen.rotate = function(self, rad)
	local rot = Quaternion{axis = Vector(0, 1, 0), angle = rad}
	self.object.rotation = self.object.rotation * rot
end

Views.Chargen.set_body = function(self, slider, value)
	self.scroll_body[slider].value = value
	self.update_needed = true
end

Views.Chargen.set_eye_style = function(self, index)
	self.combo_eye_style.value = index
	self.combo_eye_style.text = self.list_eye_styles[index][1]
	self.eye_style = self.list_eye_styles[index][2]
	self.update_needed = true
end

Views.Chargen.set_face = function(self, slider, value)
	self.scroll_face[slider].value = value
	self.update_needed = true
end

Views.Chargen.set_eye_color = function(self, r, g, b)
	self.color_eye.red = r
	self.color_eye.green = g
	self.color_eye.blue = b
	self.update_needed = true
end

Views.Chargen.set_hair_style = function(self, index)
	self.combo_hair_style.value = index
	self.combo_hair_style.text = self.list_hair_styles[index][1]
	self.hair_style = self.list_hair_styles[index][2]
	self.update_needed = true
end

Views.Chargen.set_hair_color = function(self, r, g, b)
	self.color_hair.red = r
	self.color_hair.green = g
	self.color_hair.blue = b
	self.update_needed = true
end

Views.Chargen.set_height = function(self, value)
	self.scroll_height.value = value
	self.update_needed = true
end

Views.Chargen.get_race = function(self)
	return Species:find{name = self.list_races[self.combo_race.value][2]}
end

Views.Chargen.set_race = function(self, index)
	-- Set the race selection.
	self.combo_race.value = index
	self.combo_race.text = self.list_races[index][1]
	self.race = self.list_races[index][2]
	local spec = self:get_race()
	self.spec = spec
	-- Rebuild the eye style list.
	self.list_eye_styles = {}
	self.combo_eye_style:clear()
	for k,v in ipairs(spec.eye_styles) do
		self.list_eye_styles[k] = v
		self.combo_eye_style:append{text = v[1], pressed = function() self:set_eye_style(k) end}
	end
	-- Rebuild the hair style list.
	self.list_hair_styles = {}
	self.combo_hair_style:clear()
	for k,v in ipairs(spec.hair_styles) do
		self.list_hair_styles[k] = v
		self.combo_hair_style:append{text = v[1], pressed = function() self:set_hair_style(k) end}
	end
	-- Rebuild the skin style list.
	self.list_skin_styles = {}
	self.combo_skin_style:clear()
	for k,v in ipairs(spec.skin_styles) do
		self.list_skin_styles[k] = v
		self.combo_skin_style:append{text = v[1], pressed = function() self:set_skin_style(k) end}
	end
	-- Choose a random preset.
	local index = math.random(1, #self.list_presets)
	self.toggle_presets[index]:pressed()
	-- Randomize the name.
	self.entry_name.text = Names:random{race = self.list_races[self.combo_race.value][2], gender = "female"}
	-- Reset skills.
	self.skills:set_species(spec)
end

Views.Chargen.set_skin_style = function(self, index)
	self.combo_skin_style.value = index
	self.combo_skin_style.text = self.list_skin_styles[index][1]
	self.skin_style = self.list_skin_styles[index][2]
	self.update_needed = true
end

Views.Chargen.set_skin_color = function(self, r, g, b)
	self.color_skin.red = r
	self.color_skin.green = g
	self.color_skin.blue = b
	self.update_needed = true
end

Views.Chargen.update = function(self, secs)
	-- Update model.
	if self.update_needed then
		self.update_needed = nil
		self:update_model()
	end
	self.object:refresh()
	-- Update light.
	self.light.position = self.object.position + self.object.rotation * Vector(0, 2, -5)
	-- Update camera.
	self.camera.target_position = self.object.position + self.view_offset
	self.camera.target_rotation = Quaternion{axis = Vector(0, 1, 0), angle = math.pi}
	self.camera.viewport = {self.x, self.y, self.width, self.height}
	self.camera:update(secs)
end

--- Sets the character from a preset.
-- @param self Chargen.
-- @param args Preset table.
Views.Chargen.set_preset = function(self, args)
	local set = function(var, field, value)
		if value then var[field] = value end
	end
	if args.body_scale then
		self:set_height(self.spec.body_scale[1] + args.body_scale * (self.spec.body_scale[2] - self.spec.body_scale[1]))
	end
	if args.body_style then
		for k,v in pairs(self.scroll_body) do
			v = args.body_style[k] or 0
			if self.scroll_body[k].inverse then v = 1 - v end
			set(self.scroll_body[k], "value", v)
		end
	end
	if args.eye_color then
		set(self.color_eye, "red", args.eye_color[1])
		set(self.color_eye, "green", args.eye_color[2])
		set(self.color_eye, "blue", args.eye_color[3])
	end
	if args.eye_style then
		self:set_eye_style(math.random(1, #self.list_eye_styles))
	end
	if args.face_style then
		for k,v in pairs(self.scroll_face) do
			v = args.face_style[k] or 0
			if self.scroll_face[k].inverse then v = 1 - v end
			set(self.scroll_face[k], "value", v)
		end
	end
	if args.hair_color then
		set(self.color_hair, "red", args.hair_color[1])
		set(self.color_hair, "green", args.hair_color[2])
		set(self.color_hair, "blue", args.hair_color[3])
	end
	if args.hair_style then
		self:set_hair_style(math.random(1, #self.list_hair_styles))
	end
	if args.skin_color then
		set(self.color_skin, "red", args.skin_color[1])
		set(self.color_skin, "green", args.skin_color[2])
		set(self.color_skin, "blue", args.skin_color[3])
	end
	if args.skin_style then
		self:set_skin_style(math.random(1, #self.list_skin_styles))
	end
end

Views.Chargen.update_model = function(self)
	local spec = Species:find{name = self.race .. "-player"}
	local items = {}
	local body = {}
	for k,v in pairs(self.scroll_body) do
		if v.inverse then
			body[k] = 1 - v.value
		else
			body[k] = v.value
		end
	end
	local face = {}
	for k,v in pairs(self.scroll_face) do
		if v.inverse then
			face[k] = 1 - v.value
		else
			face[k] = v.value
		end
	end
	self.object.spec = spec
	self.object.body_scale = self.scroll_height.value
	self.object.body_style = body
	self.object.equipment = items
	self.object.eye_color = {self.color_eye.red, self.color_eye.green, self.color_eye.blue}
	self.object.eye_style = nil
	self.object.face_style = face
	self.object.hair_color = {self.color_hair.red, self.color_hair.green, self.color_hair.blue}
	self.object.hair_style = self.hair_style
	self.object.skin_color = {self.color_skin.red, self.color_skin.green, self.color_skin.blue}
	self.object.skin_style = nil
	self.object:set_model()
	self.object:animate_spec("idle")
	if self.dump_presets then
		print(self.object:write_preset())
	end
end

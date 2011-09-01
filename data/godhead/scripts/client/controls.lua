Action{name = "attack", mode = "toggle", key1 = "mouse1", func = function(v)
	if Client.mode == "game" then
		-- Game controls.
		Network:send{packet = Packet(packets.PLAYER_ATTACK, "bool", v)}
	elseif Client.mode == "editor" then
		-- Editor controls.
		if v then

			Client.views.editor.editor.mode = "grab"

			local add = Action.dict_press[Keysym.LSHIFT]
			Client.views.editor.editor:select(add)
		else
			-- Released.
			Client.views.editor.editor.mode = nil

		end
	end
end}

Action{name = "block", mode = "toggle", key1 = "mouse3", func = function(v)
	if Client.mode == "game" then
		Network:send{packet = Packet(packets.PLAYER_BLOCK, "bool", v)}
	end
end}

Action{name = "camera", mode = "press", key1 = Keysym.y, func = function()
	if Client.player_object then
		if Client.camera_mode == "first-person" then
			Client.camera_mode = "third-person"
			if Client.player_object then
				local e = Client.player_object.rotation.euler
				e[3] = 0
				Client.player_object.rotation = Quaternion{euler = e}
			end
		else
			Client.camera_mode = "first-person"
		end
	end
end}

Action{name = "chat", mode = "press", key1 = Keysym.t, func = function()
	if Client.mode == "game" then
		Gui.chat_active = not Gui.chat_active
	end
end}

Action{name = "choice_1", mode = "press", key1 = Keysym.F1, func = function()
	if Client.player_object then
		local w = Gui.dialog_choices and Gui.dialog_choices[1]
		if not w then return end
		w:pressed()
	end
end}

Action{name = "choice_2", mode = "press", key1 = Keysym.F2, func = function()
	if Client.player_object then
		local w = Gui.dialog_choices and Gui.dialog_choices[2]
		if not w then return end
		w:pressed()
	end
end}

Action{name = "choice_3", mode = "press", key1 = Keysym.F3, func = function()
	if Client.player_object then
		local w = Gui.dialog_choices and Gui.dialog_choices[3]
		if not w then return end
		w:pressed()
	end
end}

Action{name = "choice_4", mode = "press", key1 = Keysym.F4, func = function()
	if Client.player_object then
		local w = Gui.dialog_choices and Gui.dialog_choices[4]
		if not w then return end
		w:pressed()
	end
end}

Action{name = "choice_5", mode = "press", key1 = Keysym.F5, func = function()
	if Client.player_object then
		local w = Gui.dialog_choices and Gui.dialog_choices[5]
		if not w then return end
		w:pressed()
	end
end}

Action{name = "choice_6", mode = "press", key1 = Keysym.F6, func = function()
	if Client.player_object then
		local w = Gui.dialog_choices and Gui.dialog_choices[6]
		if not w then return end
		w:pressed()
	end
end}

Action{name = "choice_7", mode = "press", key1 = Keysym.F7, func = function()
	if Client.player_object then
		local w = Gui.dialog_choices and Gui.dialog_choices[7]
		if not w then return end
		w:pressed()
	end
end}

Action{name = "choice_8", mode = "press", key1 = Keysym.F8, func = function()
	if Client.player_object then
		local w = Gui.dialog_choices and Gui.dialog_choices[8]
		if not w then return end
		w:pressed()
	end
end}

Action{name = "climb", mode = "press", key1 = Keysym.c, func = function()
	if Client.mode == "game" then
		Network:send{packet = Packet(packets.PLAYER_CLIMB)}
	end
end}

Action{name = "editor_rotate", mode = "press", key1 = Keysym.r, func = function(v)
	if Client.mode == "editor" then
		-- Editor controls.
		if Client.views.editor.editor.mode == "rotate" then
			Client.views.editor.editor.mode = nil
		else
			Client.views.editor.editor.mode = "rotate"
		end
	end
end}

Action{name = "editor_select_rect", mode = "toggle", key1 = Keysym.LCTRL, func = function(v)
	if Client.mode == "editor" then
		-- Editor controls.
		Client.views.editor.editor:set_rect_select(v)
	end
end}

Action{name = "feats", mode = "press", key1 = Keysym.f, func = function()
	if Client.mode == "feats" then
		-- Close with same button.
		Client:set_mode("game")
	elseif Client.player_object then
		-- Game controls.
		Client:set_mode("feats")
	elseif Client.mode == "editor" then
		-- Editor controls.
		if Client.views.editor.editor.prev_tiles[1] and Client.views.editor.editor.prev_tiles[2] then
			Client.views.editor.editor:fill(Client.views.editor.editor.prev_tiles[1], Client.views.editor.editor.prev_tiles[2])
		end
	end
end}

Action{name = "grab", mode = "press", key1 = Keysym.g, func = function(v)
	if Client.mode == "game" then
		-- Game controls.
		-- TODO
	elseif Client.mode == "editor" then
		-- Editor controls.
		if Client.views.editor.editor.mode == "grab" then
			Client.views.editor.editor.mode = nil
		else
			Client.views.editor.editor.mode = "grab"
		end
	end
end}

Action{name = "copy", mode = "press", key1 = Keysym.x, func = function(v)
	if Client.mode == "game" then
		-- Game controls.
		-- TODO
	elseif Client.mode == "editor" then
		-- Editor controls.
		Client.views.editor.editor:copy()
	end
end}

Action{name = "paste", mode = "press", key1 = Keysym.v, func = function(v)
	if Client.mode == "game" then
		-- Game controls.
		-- TODO
	elseif Client.mode == "editor" then
		-- Editor controls.
		Client.views.editor.editor:paste()
	end
end}

Action{name = "inventory", mode = "press", key1 = Keysym.i, func = function()
	if Client.mode == "inventory" then
		-- Close with same button.
		Client:set_mode("game")
	elseif Client.player_object then
		Client:set_mode("inventory")
	end
end}

Action{name = "jump", mode = "press", key1 = Keysym.SPACE, func = function()
	if Client.mode == "game" then
		Network:send{packet = Packet(packets.PLAYER_JUMP)}
	end
end}

Action{name = "map", mode = "press", key1 = Keysym.m, func = function()
	if Client.mode == "map" then
		-- Close with same button.
		Client:set_mode("game")
	elseif Client.player_object then
		Client:set_mode("map")
	end
end}

Action{name = "menu", mode = "press", key1 = Keysym.ESCAPE, func = function()
	if Client.mode == "game" then
		-- Game controls.
		Client:set_mode("menu")
	elseif Client.mode == "editor" then
		-- Editor controls.
		Program.cursor_grabbed = not Program.cursor_grabbed
	elseif Client.player_object then
		-- Menu controls.
		Client:set_mode("game")
	end
end}

Action{name = "move", mode = "analog", key1 = Keysym.w, key2 = Keysym.s, func = function(v)
	if Client.mode == "game" then
		-- Game controls.
		v = math.max(-1, math.min(1, v))
		Network:send{packet = Packet(packets.PLAYER_MOVE, "int8", v * -127)}
	elseif Client.mode == "editor" then
		-- Editor controls.
		local mult = Action.dict_press[Keysym.LCTRL] and 1 or 10
		if Action.dict_press[Keysym.LSHIFT] then
			Client.views.editor.editor.camera.lifting = -mult * v
			Client.views.editor.editor.camera.movement = nil
		else
			Client.views.editor.editor.camera.movement = -mult * v
			Client.views.editor.editor.camera.lifting = nil
		end
	end
end}

Action{name = "options", mode = "press", key1 = Keysym.o, func = function()
	if Client.mode == "options" then
		-- Close with same button.
		Client:set_mode("game")
	elseif Client.player_object then
		Client:set_mode("options")
	end
end}

Action{name = "pick_up", mode = "press", key1 = Keysym.COMMA, func = function()
	if Client.mode == "game" then
		Commands:pickup()
	end
end}

Action{name = "quests", mode = "press", key1 = Keysym.q, func = function()
	if Client.mode == "quests" then
		-- Close with same button.
		Client:set_mode("game")
	elseif Client.player_object then
		Client:set_mode("quests")
	end
end}

Action{name = "skills", mode = "press", key1 = Keysym.k, func = function()
	if Client.mode == "skills" then
		-- Close with same button.
		Client:set_mode("game")
	elseif Client.player_object then
		-- Game controls.
		Client:set_mode("skills")
	elseif Client.mode == "editor" then
		-- Editor controls.
		if Client.views.editor.editor.prev_tiles[1] and Client.views.editor.editor.prev_tiles[2] then
			Client.views.editor.editor:fill(Client.views.editor.editor.prev_tiles[1], Client.views.editor.editor.prev_tiles[2], true)
		end
	end
end}

Action{name = "quickslot_mode", mode = "press", key1 = Keysym.TAB, func = function()
	if Client.player_object then
		-- Game controls.
		if Quickslots.mode == "feats" then
			Quickslots.mode = "items"
		else
			Quickslots.mode = "feats"
		end
	elseif Client.mode == "editor" then
		-- Editor controls.
		local mult = Action.dict_press[Keysym.LCTRL] and 0.25 or 0.5
		for k,v in pairs(Client.views.editor.editor.selection) do
			if v.object then
				v.object:snap(mult * Voxel.tile_size, mult * math.pi)
				v:refresh()
			end
		end
	end
end}

Action{name = "quickslot_1", mode = "press", key1 = Keysym.NUM1, func = function()
	if Client.player_object then
		Quickslots:activate(1)
	end
end}

Action{name = "quickslot_2", mode = "press", key1 = Keysym.NUM2, func = function()
	if Client.player_object then
		Quickslots:activate(2)
	end
end}

Action{name = "quickslot_3", mode = "press", key1 = Keysym.NUM3, func = function()
	if Client.player_object then
		Quickslots:activate(3)
	end
end}

Action{name = "quickslot_4", mode = "press", key1 = Keysym.NUM4, func = function()
	if Client.player_object then
		Quickslots:activate(4)
	end
end}

Action{name = "quickslot_5", mode = "press", key1 = Keysym.NUM5, func = function()
	if Client.player_object then
		Quickslots:activate(5)
	end
end}

Action{name = "quickslot_6", mode = "press", key1 = Keysym.NUM6, func = function()
	if Client.player_object then
		Quickslots:activate(6)
	end
end}

Action{name = "quickslot_7", mode = "press", key1 = Keysym.NUM7, func = function()
	if Client.mode == "game" then
		Quickslots:activate(7)
	end
end}

Action{name = "quickslot_8", mode = "press", key1 = Keysym.NUM8, func = function()
	if Client.player_object then
		Quickslots:activate(8)
	end
end}

Action{name = "quickslot_9", mode = "press", key1 = Keysym.NUM9, func = function()
	if Client.player_object then
		Quickslots:activate(9)
	end
end}

Action{name = "quickslot_10", mode = "press", key1 = Keysym.NUM0, func = function()
	if Client.player_object then
		Quickslots:activate(10)
	end
end}

Action{name = "run", mode = "toggle", key1 = Keysym.LSHIFT, func = function(v)
	if Client.mode == "game" then
		Network:send{packet = Packet(packets.PLAYER_RUN, "bool", not v)}
	end
end}

Action{name = "screenshot", mode = "press", key1 = Keysym.PRINT, func = function()
	if Client.player_object then
		local n = Program:capture_screen()
		Gui.chat_history:append{text = "Screenshot: " .. n}
	end
end}

Action{name = "strafe", mode = "analog", key1 = Keysym.a, key2 = Keysym.d, func = function(v)
	if Client.mode == "game" then
		-- Game controls.
		Network:send{packet = Packet(packets.PLAYER_STRAFE, "int8", v * 127)}
	elseif Client.mode == "editor" then
		-- Editor controls.
		local mult = Action.dict_press[Keysym.LCTRL] and 1 or 10
		Client.views.editor.editor.camera.strafing = mult * v
	end
end}

Action{name = "tilt", mode = "analog", key1 = "mousey", func = function(v)
	local sens = 0.01 * Client.views.options.mouse_sensitivity
	if Client.views.options.invert_mouse then sens = -sens end
	if Client.mode == "game" then
		-- Game controls.
		if Action.dict_press[Keysym.LCTRL] then
			Client.camera.tilt_state = Client.camera.tilt_state - v * sens
		else
			Player.tilt_state = Player.tilt_state + v * sens
		end
	elseif Client.mode == "editor" then
		-- Editor controls.
		if Client.views.editor.editor.mode == "grab" then
			Client.views.editor.editor:grab(Vector(0, v * Client.views.editor.editor.mouse_sensitivity))
		else
			Client.views.editor.editor.camera:rotate(0, v * sens)
		end
	end
end}

Action{name = "turn", mode = "analog", key1 = "mousex", func = function(v)
	local sens = 0.01 * Client.views.options.mouse_sensitivity
	if Client.mode == "game" then
		-- Game controls.
		if Action.dict_press[Keysym.LCTRL] then
			Client.camera.turn_state = Client.camera.turn_state + v * sens
		else
			Player.turn_state = Player.turn_state - v * sens
		end
	elseif Client.mode == "editor" then
		-- Editor controls.
		if Client.views.editor.editor.mode == "grab" then
			-- Move the selection
			Client.views.editor.editor:grab(Vector(-v * Client.views.editor.editor.mouse_sensitivity), 0)
		elseif Client.views.editor.editor.mode ~= "rotate" then
			-- Rotate the camera.
			Client.views.editor.editor.camera:rotate(-v * sens, 0)
		else
			-- Rotate the selected objects.
			-- If left control is pressed, rotation is 10x slower.
			local mult = Action.dict_press[Keysym.LCTRL] and 0.1 or 1
			local drot = Quaternion{euler = {-v * mult * sens, 0, 0}}
			for k,v in pairs(Client.views.editor.editor.selection) do
				v:rotate(drot)
			end
		end
	end
end}

Action{name = "use", mode = "press", key1 = Keysym.e, func = function()
	if Client.mode == "game" then
		Commands:use()
	end
end}

Action{name = "zoom", mode = "analog", key1 = "mouse4", key2 = "mouse5", func = function(v)
	if Client.mode == "game" then
		-- Game controls.
		Client.camera:zoom{rate = 1 * v}
	elseif Client.mode == "editor" then
		-- Editor controls.
		if Program.cursor_grabbed and v ~= 0 then
			Client.views.editor.editor:extrude(v < 0)
		end
	end
end}

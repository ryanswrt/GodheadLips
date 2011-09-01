local editor_gui_actions = {menu = true, screenshot = true}

Eventhandler{type = "keypress", func = function(self, args)
	if Client.views.controls.editing_binding then
		-- Binding input.
		Client.views.controls:input(args)
		Action:event(args, {})
	elseif Client.mode == "editor" then
		-- Editor input.
		if not Program.cursor_grabbed then
			Widgets:handle_event(args)
			Action:event(args, editor_gui_actions)
		else
			Action:event(args)
		end
	elseif Client.mode ~= "game" and Widgets:handle_event(args) then
		-- Widget input.
		Action:event(args, {})
	elseif Gui.chat_active then
		-- Chat input.
		if args.code == Keysym.ESCAPE then
			Gui.chat_active = false
		else
			Gui.chat_entry:event(args)
			if args.code == Keysym.RETURN then
				Gui.chat_active = false
			end
		end
		Action:event(args, {})
	else
		-- Game controls.
		Action:event(args)
	end
end}

Eventhandler{type = "keyrelease", func = function(self, args)
	if Client.views.controls.editing_binding then
		-- Binding input.
		Client.views.controls:input(args)
		Action:event(args, {})
	elseif Client.mode == "editor" then
		-- Editor input.
		if not Program.cursor_grabbed then
			Widgets:handle_event(args)
			Action:event(args, editor_gui_actions)
		else
			Action:event(args)
		end
	else
		-- Normal input.
		Action:event(args)
	end
end}

Eventhandler{type = "mousepress", func = function(self, args)
	if Client.views.controls.editing_binding then
		-- Binding input.
		Client.views.controls:input(args)
		Action:event(args, {})
	elseif Client.mode == "editor" then
		-- Editor input.
		if not Program.cursor_grabbed then
			Widgets:handle_event(args)
			Action:event(args, editor_gui_actions)
		else
			Action:event(args)
		end
	else
		-- Normal input.
		if args.button == 4 and Drag.drag then
			Drag:change_count(1)
		elseif args.button == 5 and Drag.drag then
			Drag:change_count(-1)
		elseif Client.mode ~= "game" then
			Widgets:handle_event(args)
			Action:event(args, {})
		else
			Action:event(args)
		end
	end
end}

Eventhandler{type = "mouserelease", func = function(self, args)
	if Client.views.controls.editing_binding then
		-- Binding input.
		Client.views.controls:input(args)
		Action:event(args, {})
	elseif Client.mode == "editor" then
		-- Editor input.
		if not Program.cursor_grabbed then
			Widgets:handle_event(args)
			Action:event(args, editor_gui_actions)
		else
			Action:event(args)
		end
	else
		-- Normal input.
		if Client.mode ~= "game" then
			Widgets:handle_event(args)
			Action:event(args, {})
		else
			Action:event(args)
		end
	end
end}

Eventhandler{type = "mousemotion", func = function(self, args)
	if Client.views.controls.editing_binding then
		-- Binding input.
		Client.views.controls:input(args)
		Action:event(args, {})
	elseif Client.mode == "editor" then
		-- Editor input.
		if not Program.cursor_grabbed then
			Widgets:handle_event(args)
			Action:event(args, editor_gui_actions)
		else
			Action:event(args)
		end
	else
		-- Normal input.
		if Client.mode ~= "game" then
			Widgets:handle_event(args)
			Action:event(args, {})
		else
			Action:event(args)
		end
	end
end}

Eventhandler{type = "music-ended", func = function(self, args)
	Sound:switch_music_track()
end}

Eventhandler{type = "quit", func = function(self, args)
	Program.quit = true
end}

local animt = 0
local compasst = 0
local fpst = 0
local ipolt = 0
Eventhandler{type = "tick", func = function(self, args)
	-- Update the connection status.
	if Client.views.startup.joined and not Network.connected then
		Client:set_mode("startup")
		Client.views.startup:set_state("Lost connection to the server!")
	end
	-- Update the window size.
	if Gui:resize() then
		Client.views.options:changed()
	end
	-- Update the notification widget.
	Gui.notification:update(args.secs)
	-- Update the cursor.
	Widgets.Cursor.inst:update()
	-- Update built models.
	while true do
		local msg = Client.threads.model_builder:pop_message()
		if not msg then break end
		if msg.model then
			local obj = Object:find{id = tonumber(msg.name)}
			if obj and obj.spec then
				msg.model:changed()
				obj:set_model(msg.model)
			end
		end
	end
	-- Update active objects.
	animt = animt + args.secs
	local anim = Object.deform_mesh and animt > 0.2 * (1 - Client.views.options.animation_quality)
	for k,v in pairs(Object.dict_active) do
		-- Update animations.
		if anim and k.animated then
			k:update_animations{secs = animt}
			k:update_sound(animt)
			k:deform_mesh()
		end
		-- Interpolate positions.
		k:update_motion_state(args.secs)
		-- Update slots and special effects.
		k:update(args.secs)
		-- Maintain activity.
		if k.spec and k.spec.type ~= "species" then
			v = v - args.secs
			if v <= 0 then v = nil end
			Object.dict_active[k] = v
		end
	end
	if anim then animt = 0 end
	-- Update player state.
	if Client.player_object then
		Player:update_pose(args.secs)
		Player:update_rotation(args.secs)
		Client.camera1.object = Client.player_object
		Client.camera3.object = Client.player_object
		Client.camera1:update(args.secs)
		Client.camera3:update(args.secs)
		Player:update_light(args.secs)
		-- Sound playback.
		local p,r = Client.player_object:find_node{name = "#neck", space = "world"}
		if p then
			Sound.listener_position = p
			Sound.listener_rotation = r
		else
			Sound.listener_position = Client.player_object.position + Vector(0,1.5,0)
			Sound.listener_rotation = Client.player_object.rotation
		end
		local vel = Client.player_object.velocity
		if vel then Sound.listener_velocity = vel end
		-- Refresh the active portion of the map.
		Client.player_object:refresh()
		-- Maintain the respawn widget.
		Gui:set_dead(Client.player_object.dead)
	end
	-- Update the 3D cursor.
	-- This really needs to be done every frame since the 3rd person
	-- camera suffers greatly from any big cursor position changes.
	if Client.player_object and Program.cursor_grabbed then
		Player:pick_look()
	else
		Target.target_object = nil
	end
	-- Periodically update the compass.
	compasst = compasst + args.secs
	if compasst > 0.1 then
		compasst = 0
		Player:update_compass()
	end
	-- Update the FPS label.
	if Gui.fps_label then
		fpst = fpst + args.secs
		if fpst > 0.1 then
			Gui.fps_label.text = "FPS: " .. math.floor(Program.fps + 0.5)
			fpst = 0
		end
	end
end}

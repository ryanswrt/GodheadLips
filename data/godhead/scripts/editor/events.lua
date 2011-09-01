local gui_actions = {editor_menu = true, editor_rotate = true, editor_snap = true}

Eventhandler{type = "keypress", func = function(self, args)
	if not Program.cursor_grabbed then
		Widgets:handle_event(args)
		Action:event(args, gui_actions)
	else
		Action:event(args)
	end
end}

Eventhandler{type = "keyrelease", func = function(self, args)
	if not Program.cursor_grabbed then
		Widgets:handle_event(args)
		Action:event(args, gui_actions)
	else
		Action:event(args)
	end
end}

Eventhandler{type = "mousepress", func = function(self, args)
	if not Program.cursor_grabbed then
		Widgets:handle_event(args)
		Action:event(args, gui_actions)
	else
		Action:event(args)
	end
end}

Eventhandler{type = "mousemotion", func = function(self, args)
	if not Program.cursor_grabbed then
		Widgets:handle_event(args)
		Action:event(args, gui_actions)
	else
		Action:event(args)
	end
end}

Eventhandler{type = "quit", func = function(self, args)
	Program.quit = true
end}

Eventhandler{type = "tick", func = function(self, args)
	-- Update the editor.
	Editor.inst:update(args.secs)
	-- Update the cursor.
	Widgets.Cursor.inst:update()
end}

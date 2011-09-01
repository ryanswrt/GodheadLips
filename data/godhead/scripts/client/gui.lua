Gui = Class()

--- Initializes the in-game user interface.
-- @param clss Gui class.
Gui.init = function(clss)
	Gui.menus = Widgets.Menus()
	Gui.notification = Widgets.Notification()
	-- Chat widgets.
	Gui.chat_history = Widgets.Log()
	Gui.chat_label = Widgets.Label{text = " "}
	Gui.chat_entry = Widgets.Entry{color = {1,1,1,1}, visible = false, width_request = 500,
		background = {dest_position = {0,4}, source_image = "widgets1",
		source_position = {722,63}, source_tiling = {6,32,6,6,24,6}}}
	Gui.chat_entry.pressed = function(self)
		Network:send{packet = Packet(packets.PLAYER_CHAT, "string", self.text)}
		self:clear()
	end
	-- Skill widgets.
	Gui.skill_health = Widgets.SkillControl{compact = true}
	Gui.skill_mana = Widgets.SkillControl{compact = true}
	Gui.fps_label = Widgets.Label{valign = 1, request = Vector(60,20)}
	-- NPC dialog.
	Gui.group_dialog = Widget{cols = 1, rows = 3}
	Gui.group_dialog:set_expand{row = 1}
	Gui.group_dialog:set_child(1, 2, Widgets.Label())
	-- Modifiers, respawning and skill background.
	Gui.modifiers = Widgets.Modifiers()
	Gui.button_respawn = Widgets.Button{font = "medium", text = "Create a new character", visible = false,
		pressed = function() Network:send{packet = Packet(packets.PLAYER_RESPAWN)} end}
	Gui.skills_group = Widgets.Frame{style = "quickbar"}
	Gui.crosshair = Widgets.Icon{icon = Iconspec:find{name = "crosshair1"}}
	-- Packing.
	Gui.scene = Widgets.Scene{cols = 1, rows = 3, behind = true, fullscreen = true, margins = {5,5,0,0}, spacings = {0,0}}
	Gui.scene:set_expand{col = 1, row = 1}
	Gui.scene:set_request{width = 32, height = 32}
	Gui.scene:add_child(Gui.notification)
	Gui.scene:add_child(Gui.button_respawn)
	Gui.scene:add_child(Gui.menus)
	Gui.scene:add_child(Gui.group_dialog)
	Gui.scene:add_child(Quickslots.group)
	Gui.scene:add_child(Gui.modifiers)
	Gui.scene:add_child(Gui.fps_label)
	Gui.scene:add_child(Gui.chat_history)
	Gui.scene:add_child(Gui.chat_label)
	Gui.scene:add_child(Gui.chat_entry)
	Gui.scene:add_child(Gui.skill_health)
	Gui.scene:add_child(Gui.skill_mana)
	Gui.scene:add_child(Gui.skills_group)
	Gui.scene:add_child(Gui.crosshair)
	Gui:resize()
end

Gui.resize = function(self)
	-- Check for changes to the screen size.
	local size = Vector(Program.video_mode[1], Program.video_mode[2])
	if not self.need_rebuild and self.prev_size and size.x == self.prev_size.x and size.y == self.prev_size.y then return end
	self.need_rebuild = nil
	self.prev_size = size
	-- Update widget positions to match the new screen size.
	self.skills_group.offset = Vector(4, size.y - 80)
	self.skills_group.request = Vector(size.x-4,82)
	self.button_respawn.offset = Vector(size.x / 2 - 100, size.y / 2 - 30)
	self.menus.offset = Vector((size.x - self.menus.width) / 2, size.y - self.menus.height - 80)
	self.modifiers.offset = Vector(size.x - self.modifiers.width - 5, 5)
	self.group_dialog.offset = Vector(size.x - 510, size.y - self.group_dialog.height - 60)
	self.chat_label.offset = Vector(250, size.y - 70)
	self.chat_entry.offset = Vector(300, size.y - 70)
	self.chat_history.offset = Vector(5, 0)
	self.skill_health.offset = Vector(83, size.y - 37)
	self.skill_mana.offset = Vector(83, size.y - 21)
	self.fps_label.offset = Vector(size.x - 60, size.y - 20)
	Quickslots.group.offset = Vector(250, size.y - 36)
	local chx = (size.x - self.crosshair.width) / 2
	local chy = (size.y - self.crosshair.height) / 2
	self.crosshair.offset = Vector(chx, chy)
	return true
end

Gui.set_dead = function(self, value)
	if self.button_respawn.visible == value then return end
	self.button_respawn.visible = value
end

--- Sets or unsets the text of the action label.
-- @param clss Gui class.
-- @param text String or nil.
Gui.set_action_text = function(clss, text)
	clss.scene.action = text
end

--- Sets the ID of the object whose dialog is shown.
-- @param self Gui class.
-- @param id Object ID.
Gui.set_dialog = function(clss, id)
	-- Find the dialog.
	clss.active_dialog = id
	local obj = id and Object:find{id = id}
	local dlg = obj and obj.dialog
	-- Update the dialog UI.
	if not dlg then
		-- Hide the dialog UI.
		clss.dialog_choices = nil
		clss.group_dialog.rows = 1
		clss.group_dialog.visible = false
	elseif dlg.type == "choice" then
		-- Show a dialog choice.
		clss.dialog_choices = {}
		clss.group_dialog.rows = 1
		for k,v in ipairs(dlg.choices) do
			local widget = Widgets.DialogLabel{choice = true, index = k, text = v, pressed = function()
				Network:send{packet = Packet(packets.DIALOG_ANSWER, "uint32", id, "string", v)}
			end}
			table.insert(clss.dialog_choices, widget)
			clss.group_dialog:append_row(widget)
		end
		clss.group_dialog.visible = true
	else
		-- Show a dialog line.
		clss.dialog_choices = {}
		clss.group_dialog.rows = 1
		local pressed = function() Network:send{packet = Packet(packets.DIALOG_ANSWER, "uint32", id, "string", "")} end
		if dlg.character ~= "" then
			local widget = Widgets.DialogLabel{text = dlg.character, pressed = pressed}
			table.insert(clss.dialog_choices, widget)
			clss.group_dialog:append_row(widget)
		end
		local widget = Widgets.DialogLabel{index = 1, text = dlg.message, pressed = pressed}
		table.insert(clss.dialog_choices, widget)
		clss.group_dialog:append_row(widget)
		clss.group_dialog.visible = true
	end
	-- Make sure the dialog widget is positioned correctly.
	clss.need_rebuild = true
end

--- Sets or unsets the active target.
-- @param clss Gui class.
-- @param text String or nil.
Gui.set_target_text = function(clss, text)
	clss.scene.action = text
end

Gui.class_getters = {
	chat_active = function(s) return s.chat_entry.visible end}

Gui.class_setters = {
	chat_active = function(s, v)
		s.chat_entry.visible = v
		s.chat_label.text = v and "TALK: " or " "
		Client:set_mode("game")
	end}

Editing = Class()
Editing.visible = false

--- Initializes the editor.
-- @param clss Editor class.
Editing.init = function(clss)

	-- Creating terrain.
	clss.spin_terrain = Widgets.ComboBox()
	local mats = {}
	for k,v in ipairs(Material.dict_id) do table.insert(mats, v.name) end
	table.sort(mats)
	for k,v in ipairs(mats) do
		clss.spin_terrain:append{text = v}
	end
	clss.spin_terrain:activate{index = 1}
	clss.group_terrain_create = Widget{cols = 1}
	clss.group_terrain_create:set_expand{col = 1}
	clss.group_terrain_create:append_row(Widgets.Label{text = "Material:"})
	clss.group_terrain_create:append_row(clss.spin_terrain)
	clss.group_terrain_create:append_row(Widgets.Button{text = "Create", pressed = function()
		Editing:insert()
	end})

	-- Deleting terrain.
	clss.group_terrain_delete = Widget{cols = 1}
	clss.group_terrain_delete:set_expand{col = 1}
	clss.group_terrain_delete:append_row(Widgets.Button{text = "Delete", pressed = function()
		Editing:erase()
	end})

	-- Popup button.
	clss.popup_button = Widgets.ComboBox{
		{"Terrain: create", function() Editing:set_mode(1) end},
		{"Terrain: delete", function() Editing:set_mode(2) end}}

	-- Packing.
	clss.dialog = Widgets.Frame{cols = 1, rows = 3, style = "popup"}
	clss.dialog.spacings = {0, 20}
	clss.dialog:set_expand{col = 1}
	clss.dialog:set_request{width = 200, height = 300}
	clss.dialog:set_child{col = 1, row = 1, widget = Widgets.Label{font = "medium", text = "Editor"}}
	clss.dialog:set_child{col = 1, row = 2, widget = clss.popup_button}
	clss:set_mode(1)
end

--- Sets the mode of the editor dialog.
-- @param clss Editing class.
-- @param mode Mode number.
Editing.set_mode = function(clss, mode)
	local funs =
	{
		function()
			clss.dialog:set_child{col = 1, row = 3, widget = clss.group_terrain_create}
			clss.popup_button.text = "Terrain: create"
		end,
		function()
			clss.dialog:set_child{col = 1, row = 3, widget = clss.group_terrain_delete}
			clss.popup_button.text = "Terrain: delete"
		end
	}
	local fun = funs[mode]
	fun()
end

--- Toggles visibility of the editor.
-- @param clss Editor class.
function Editing.toggle(clss)
	clss.visible = not clss.visible
	Gui.editor_group.visible = clss.visible
end

Editing.enter = function(self, from, level)
	Gui.menus:open{level = level, widget = self.dialog}
end

function Editing.erase(self)
	local function func(where, id, slot)
		if where == "map" and slot ~= nil then
			local t,p = Voxel:find_tile{match = "full", point = slot}
			if p then
				Network:send{packet = Packet(packets.EDIT_TILE,
					"int32", p.x, "int32", p.y, "int32", p.z, "uint8", t)}
			end
			Editing:erase()
		end
	end
	Target:start("Erasing terrain...", func)
end

function Editing.insert(self)
	local function func(where, id, slot)
		if where == "map" and slot ~= nil then
			local t,p = Voxel:find_tile{match = "empty", point = slot}
			if p then
				local m = Material:find{name = self.spin_terrain.text}
				if m then
					Network:send{packet = Packet(packets.EDIT_TILE,
						"uint32", p.x, "uint32", p.y, "uint32", p.z, "uint8", m.id)}
				end
			end
			Editing:insert(how)
		end
	end
	Target:start("Creating terrain...", func)
end

Editing:init()

require "client/widgets/menu"

Views.Admin = Class(Widgets.Menu)

Views.Admin.new = function(clss)
	local self = Widget.new(clss, {cols = 3, rows = 2, spacings = {0, 0}})
	self.title = Widgets.Title{text = "Admin stats",
		back = function() self:back() end,
		close = function() Client:set_mode("game") end,
		help = function() Client.views.help:show("admin") end}
	self.label_server = Widgets.Text{valign = 0}
	self.label_client = Widgets.Text{valign = 0}
	self.label_database = Widgets.Text{valign = 0}
	self:set_child(1, 1, self.title)
	self:set_child(1, 2, self.label_server)
	self:set_child(2, 2, self.label_client)
	self:set_child(3, 2, self.label_database)
	return self
end

Views.Admin.enter = function(self, from, level)
	Gui.menus:open{level = level, widget = self}
	Network:send{packet = Packet(packets.ADMIN_STATS)}
	-- Client stats.
	local userdata = 0
	local dicttype = {}
	for k,v in pairs(__userdata_lookup) do
		local type = v.class_name or "???"
		userdata = userdata + 1
		dicttype[type] = (dicttype[type] or 0) + 1
	end
	local listtype = {}
	for k,v in pairs(dicttype) do
		table.insert(listtype, string.format("%s: %d", k, v))
	end
	table.sort(listtype)
	local numactive = 0
	for k in pairs(Object.dict_active) do
		numactive = numactive + 1
	end
	local t = string.format([[
Active objects: %d
Userdata: %d]], numactive, userdata)
	for k,v in ipairs(listtype) do
		t = string.format("%s\n%s", t, v)
	end
	self.label_client.text = t
	self.label_client:build()
	-- Memory stats.
	local models = 0
	for k,v in pairs(__userdata_lookup) do
		if v.class_name == "Model" then
			models = models + v.memory_used
		end
	end
	self.label_database.text = string.format([[
Database memory: %d kB
Script memory: %d kB
Terrain memory: %d kB
Model memory: %d kB
Update tick: %d ms
Event tick: %d ms
Render tick: %d ms
]], Database.memory_used / 1024, gcinfo(), Voxel.memory_used / 1024, models / 1024,
1000 * Program.profiling.update, 1000 * Program.profiling.event, 1000 * Program.profiling.render)
	self.label_database:build()
end

Views.Admin.back = function(self)
	Client:set_mode("menu")
end

Views.Admin.close = function(self)
end

require "client/config"
require "client/options"
require "client/firstpersoncamera"
require "client/thirdpersoncamera"

Client = Class()
Client.class_name = "Client"

-- Initialize the database.
-- FIXME: Should be initialized in the initializer.
Client.db = Database{name = "client.sqlite"}
Client.db:query("CREATE TABLE IF NOT EXISTS keyval (key TEXT PRIMARY KEY,value TEXT);")

Client.init = function(self)
	self.config = Config()
	self.options = Options.inst
	-- Initialize the world.
	self.sectors = Sectors{database = Client.db, save_objects = false}
	self.sectors:erase_world()
	-- Initialize the camera.
	-- These need to be initialized before options since they'll be
	-- reconfigured when the options are loaded.
	self.camera1 = FirstPersonCamera{collision_mask = Physics.MASK_CAMERA, far = 40.0, fov = 1.1, near = 0.01}
	self.camera3 = ThirdPersonCamera{collision_mask = Physics.MASK_CAMERA, far = 40.0, fov = 1.1, near = 0.01}
	self.camera_mode = "third-person"
	-- Initialize views.
	self.views = {}
	self.views.admin = Views.Admin()
	self.views.book = Views.Book()
	self.views.chargen = Views.Chargen()
	self.views.controls = Views.Controls()
	self.views.editor = Views.Editor()
	self.views.feats = Views.Feats()
	self.views.game = Views.Game()
	self.views.help = Views.Help()
	self.views.host = Views.Host()
	self.views.intro = Views.Intro()
	self.views.inventory = Views.Inventory()
	self.views.join = Views.Join()
	self.views.login = Views.Login()
	self.views.map = Views.Map()
	self.views.menu = Views.Menu()
	self.views.options = Views.Options()
	self.views.quests = Views.Quests()
	self.views.skills = Views.Skills{main = true, sync = true}
	self.views.startup = Views.Startup()
	-- Initialize helper threads.
	self.threads = {}
	self.threads.model_builder = Thread("client/threads/modelbuilder.lua")
end

--- Applies a world space quake.
-- @param self Client class.
-- @param point Quake point in world space.
-- @param magnitude Quake magnitude.
Client.apply_quake = function(self, point, magnitude)
	if point and magnitude and self.player_object then
		local dist = (self.player_object.position - point).length
		local quake = math.min(math.max(magnitude / (0.05 * dist * dist + 0.5), 0), 1)
		self.camera1.quake = math.max(self.camera1.quake or 0, quake)
		self.camera3.quake = math.max(self.camera3.quake or 0, quake)
	end
end

Client.set_mode = function(self, mode, level)
	-- Check for state changes.
	if self.mode == mode then return end
	-- Close the old view.
	Target:cancel()
	Drag:cancel()
	if self.view and self.view.close then self.view:close() end
	if Widgets.ComboBox.active then Widgets.ComboBox.active:close() end
	-- Open the new view.
	local from = self.mode
	self.mode = mode
	self.view = self.views[mode]
	if self.view and self.view.enter then self.view:enter(from, level or 1) end
end

Client:add_class_getters{
	camera_mode = function(self)
		if self.camera == self.camera1 then
			return "first-person"
		else
			return "third-person"
		end
	end,
	player_object = function(self) return Player.object end}

Client:add_class_setters{
	camera_mode = function(self, v)
		if v == "first-person" then
			self.camera = self.camera1
		else
			self.camera = self.camera3
		end
		self.camera:reset()
		Gui.scene.camera = self.camera
	end,
	mouse_smoothing = function(self, v)
		local s = v and 0.15 or 1
		if self.player_object then
			self.player_object.rotation_smoothing = s
		end
		self.camera3.rotation_smoothing = s
		self.camera3.position_smoothing = s
	end,
	notification_text = function(self, v)
		Gui.notification.text = v
	end,
	player_object = function(self, v)
		Client.views.skills:set_species(v.spec)
		self.views.feats:set_race(v.spec.name)
		self.views.feats:show(1)
		Player.object = v
		Player.species = v.spec.name
		Camera.mode = "third-person"
	end}

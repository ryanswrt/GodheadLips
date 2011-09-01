Options = Class()

Options.new = function(clss)
	-- Initialize defaults.
	local self = Class.new(clss)
	self.vsync = false
	self.fullscreen = false
	self.window_width = 1024
	self.window_height = 768
	self.bloom_enabled = false
	self.bloom_exposure = 1.5
	self.bloom_luminance = 1
	self.model_quality = 1
	self.animation_quality = 1
	self.anisotrophic_filter = 0
	self.invert_mouse = false
	self.mouse_sensitivity = 1
	self.mouse_smoothing = true
	self.multisamples = 2
	self.nudity_enabled = false
	self.shader_quality = 2
	self.transparency_quality = 0.3
	self.sound_volume = 1.0
	self.music_volume = 0.1
	-- Read values from the configuration file.
	self.config = ConfigFile{name = "options.cfg"}
	local opts = {
		animation_quality = {"float", 0, 1},
		anisotrophic_filter = {"int", 0, 32},
		bloom_enabled = {"bool"},
		bloom_exposure = {"float", 0, 10},
		bloom_luminance = {"float", 0, 1},
		fullscreen = {"bool"},
		invert_mouse = {"bool"},
		model_quality = {"bool"},
		mouse_sensitivity = {"float", 0, 1},
		mouse_smoothing = {"bool"},
		multisamples = {"pow", 0, 6},
		music_volume = {"float", 0, 1},
		nudity_enabled = {"bool"},
		shader_quality = {"int", 1, 3},
		sound_volume = {"float", 0, 1},
		transparency_quality = {"float", 0, 1},
		vsync = {"bool"},
		window_height = {"int", 32, 65536},
		window_width = {"int", 32, 65536}}
	for k,t in pairs(opts) do
		local v = self.config:get(k)
		if v then
			-- Boolean.
			if t[1] == "bool" then
				if v == "true" then
					self[k] = true
				elseif v == "false" then
					self[k] = false
				end
			-- Float.
			elseif t[1] == "float" then
				local v = tonumber(v)
				if v then
					v = math.max(t[2], v)
					v = math.min(t[3], v)
					self[k] = v
				end
			-- Integer.
			elseif t[1] == "int" then
				local v = tonumber(v)
				if v then
					v = math.floor(v)
					v = math.max(t[2], v)
					v = math.min(t[3], v)
					self[k] = v
				end
			-- Power of two integer.
			elseif t[1] == "pow" then
				local v = tonumber(v)
				if v then
					local vv = 2^t[2]
					for i=t[2],t[3] do
						if v < 2^i then break end
						vv = 2^i
					end
					self[k] = vv
				end
			end
		end
	end
	self:save()
	return self
end

Options.save = function(self)
	local write = function(k, v)
		self.config:set(k, tostring(v))
	end
	write("animation_quality", self.animation_quality)
	write("anisotrophic_filter", self.anisotrophic_filter)
	write("bloom_enabled", self.bloom_enabled)
	write("bloom_exposure", self.bloom_exposure)
	write("bloom_luminance", self.bloom_luminance)
	write("fullscreen", self.fullscreen)
	write("invert_mouse", self.invert_mouse)
	write("model_quality", self.model_quality)
	write("mouse_sensitivity", self.mouse_sensitivity)
	write("mouse_smoothing", self.mouse_smoothing)
	write("multisamples", self.multisamples)
	write("music_volume", self.music_volume)
	write("nudity_enabled", self.nudity_enabled)
	write("shader_quality", self.shader_quality)
	write("sound_volume", self.sound_volume)
	write("transparency_quality", self.transparency_quality)
	write("vsync", self.vsync)
	write("window_height", self.window_height)
	write("window_width", self.window_width)
	self.config:save()
end

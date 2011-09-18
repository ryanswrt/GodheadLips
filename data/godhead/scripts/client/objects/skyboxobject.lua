Player.enable_skybox = function(clss, name)
	if not clss.object then return end
    -- Find the skybox model.
	local box = Model:find_or_load{file = name}
	if not box then return end
	SkyboxObject{
		model = box,
		object = clss.object,
		sound = box.sound,
		sound_pitch = box.sound_pitch,
		sound_positional = box.sound_positional,
		sound_volume = box.sound_volume,
		realized = true}
end


SkyboxObject = Class(Object)

--- Creates a new Skybox.
-- @param clss SkyboxObject class.
-- @param args Arguments.<ul>
--   <li>model: Model name.</li>
--   <li>object: Parent object or nil.</li>
--   <li>position: Position in world space.</li>
--   <li>sound: Sound effect name.</li>
--   <li>sound_delay: Sound delay in seconds.</li>
--   <li>sound_pitch: Sound effect pitch range.</li>
--   <li>sound_positional: False to make the sound non-positional.</li>
-- @return Object.
SkyboxObject.new = function(clss, args)
	local parent = args.object
	-- Attach a model.
	local self = Object.new(clss, args)
	if Object.particle_animation then
		self:particle_animation{loop = true}
	end
	self.model = args.model
	self.model:add_material{cull = false, shader = "nolitdiff"}
	-- Attach a sound effect.
	if args.sound then
		local volume = (args.sound_volume or 1) * Client.views.options.sound_volume
        if args.sound_pitch then
            Sound:effect{object = self, effect = args.sound, volume = volume,
                positional = args.sound_positional,
                pitch = 1 + args.sound_pitch * (math.random() - 0.5)}
        else
            Sound:effect{object = self, effect = args.sound,
                positional = args.sound_positional, volume = volume}
        end
	end
	-- Copy parent transformation.
	if parent then
        self.position = parent.position + self.position
	end
	-- Update in a thread until the skybox is unloaded.
	Coroutine(function()
		local moved = parent and (self.position - parent.position) or self.position
		while (not parent or parent.realized) do
			local secs = coroutine.yield()
			if parent then
                self.position = parent.position + moved
			end
		end
		self.realized = false
	end)
	return self
end

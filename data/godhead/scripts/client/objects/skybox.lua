Skybox.enable = function(clss, name)
	if not Player.object then return end
	clss:enable_object(name, Player.object)
end

Skybox.enable_object = function(clss, name, object, node)
	-- Find the effect.
	local effect = Effect:find{name = name}
	if not effect then return end
	-- Find the node.
	local p
	local n = node or effect.node
	if n then p = object:find_node{name = n} end
	-- Create the effect object.
	SkyboxObject{
		model = effect.model,
		object = object,
		node = p and n,
		rotation_inherit = (effect.rotation ~= false),
		sound = effect.sound,
		sound_delay = effect.sound_delay,
		sound_pitch = effect.sound_pitch,
		sound_positional = effect.sound_positional,
		sound_volume = effect.sound_volume,
		realized = true}
end


SkyboxObject = Class(Object)

--- Creates a new Skybox.
-- @param clss SkyboxObject class.
-- @param args Arguments.<ul>
--   <li>model: Particle effect name.</li>
--   <li>node: Parent node or nil.</li>
--   <li>object: Parent object or nil.</li>
--   <li>position: Position in world space.</li>
--   <li>rotation_inherit: True to inherit rotation from the parent object.</li>
--   <li>sound: Sound effect name.</li>
--   <li>sound_delay: Sound delay in seconds.</li>
--   <li>sound_pitch: Sound effect pitch range.</li>
--   <li>sound_positional: False to make the sound non-positional.</li>
-- @return Object.
SkyboxObject.new = function(clss, args)
	local parent = args.object
	local node = args.node
	local rotate = args.rotation_inherit
	-- Attach a model effect.
	local self = Object.new(clss, args)
	if Object.particle_animation then
		self:particle_animation{loop = true}
	end
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
		local p = node and parent:find_node{name = node}
		if p then
			self.position = parent.position + parent.rotation * p
			if rotate then self.rotation = parent.rotation end
		else
			self.position = parent.position + self.position
			if rotate then self.rotation = parent.rotation end
		end
	end
	-- Update in a thread until the effect ends.
	Coroutine(function()
		local moved = parent and (self.position - parent.position) or self.position
		while (not parent or parent.realized) do
			local secs = coroutine.yield()
			moved = moved + velocity * secs
			if parent then
				local p = node and parent:find_node{name = node}
				if p then
					self.position = parent.position + parent.rotation * p
					if rotate then self.rotation = parent.rotation end
				else
					self.position = parent.position + moved
					if rotate then self.rotation = parent.rotation end
				end
			elseif args.velocity then
				self.position = moved
			end
		end
		self.realized = false
	end)
	return self
end

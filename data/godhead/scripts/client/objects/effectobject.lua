Effect.play = function(clss, name)
	if not Player.object then return end
	clss:play_object(name, Player.object)
end

Effect.play_object = function(clss, name, object, node)
	-- Find the effect.
	local effect = Effect:find{name = name}
	if not effect then return end
	-- Find the node.
	local p
	local n = node or effect.node
	if n then p = object:find_node{name = n} end
	-- Create the effect object.
	EffectObject{
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
	-- Quake the camera.
	Client:apply_quake(object.position, effect.quake)
end

Effect.play_world = function(clss, name, position)
	-- Find the effect.
	local effect = Effect:find{name = name}
	if not effect then return end
	-- Create the effect object.
	EffectObject{
		model = effect.model,
		position = position,
		sound = effect.sound,
		sound_delay = effect.sound_delay,
		sound_pitch = effect.sound_pitch,
		sound_positional = effect.sound_positional,
		sound_volume = effect.sound_volume,
		realized = true}
	-- Quake the camera.
	Client:apply_quake(Vector(x,y,z), effect.quake)
end

EffectObject = Class(Object)

--- Creates a new effect.
-- @param clss EffectObject class.
-- @param args Arguments.<ul>
--   <li>font: Font name.</li>
--   <li>font_color: Font color table.</li>
--   <li>life: Life time in seconds.</li>
--   <li>model: Particle effect name.</li>
--   <li>node: Parent node or nil.</li>
--   <li>object: Parent object or nil.</li>
--   <li>position: Position in world space.</li>
--   <li>rotation_inherit: True to inherit rotation from the parent object.</li>
--   <li>sound: Sound effect name.</li>
--   <li>sound_delay: Sound delay in seconds.</li>
--   <li>sound_pitch: Sound effect pitch range.</li>
--   <li>sound_positional: False to make the sound non-positional.</li>
--   <li>text: Text effect string.</li>
--   <li>velocity: Velocity vector.</li></ul>
-- @return Object.
EffectObject.new = function(clss, args)
	local life = args.life or 10
	local parent = args.object
	local node = args.node
	local velocity = args.velocity or Vector()
	local rotate = args.rotation_inherit
	-- Attach a model effect.
	local self = Object.new(clss, args)
	if Object.particle_animation then
		self:particle_animation{loop = false}
	end
	-- Attach a sound effect.
	if args.sound then
		local volume = (args.sound_volume or 1) * Client.views.options.sound_volume
		local play = function()
			if args.sound_pitch then
				Sound:effect{object = self, effect = args.sound, volume = volume,
					positional = args.sound_positional,
					pitch = 1 + args.sound_pitch * (math.random() - 0.5)}
			else
				Sound:effect{object = self, effect = args.sound,
					positional = args.sound_positional, volume = volume}
			end
		end
		if args.sound_delay then
			Timer{delay = args.sound_delay, func = function(t)
				play()
				t:disable()
			end}
		else
			play()
		end
	end
	-- Attach a text effect.
	if args.text then
		Speech:add{object = self, diffuse = args.text_color, font = args.text_font,
			message = args.text, fade_time = args.text_fade_time, life_time = args.life}
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
		local t = 0
		local moved = parent and (self.position - parent.position) or self.position
		while t < life and (not parent or parent.realized) do
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
			t = t + secs
		end
		self.realized = false
	end)
	return self
end

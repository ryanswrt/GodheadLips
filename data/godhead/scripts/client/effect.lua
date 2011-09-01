require "client/objects/effectobject"

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

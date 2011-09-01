Effect = Class()
Effect.dict_name = {}

--- Finds an effect by name.
-- @param clss Effect class.
-- @param args Arguments.<ul>
--   <li>name: Effect name.</li></ul>
-- @return Effect or nil.
Effect.find = function(clss, args)
	return clss.dict_name[args.name]
end

--- Registers a new effect.
-- @param clss Feat class.
-- @param args Arguments.<ul>
--   <li>model: Model to show.</li>
--   <li>name: Name of the effect.</li>
--   <li>node: Name of the anchor when emitted by an object.</li>
--   <li>rotation: False to not inherit rotation from the parent object.</li>
--   <li>sound: Sound effect to play.</li>
--   <li>sound_delay: Delay of the sound start, in seconds.</li>
--   <li>sound_pitch: Pitch range of the sound effect.</li>
--   <li>sound_positional: False to make the sound non-positional.</li></ul>
-- @return New effect.
Effect.new = function(clss, args)
	local self = Class.new(clss, args)
	clss.dict_name[args.name] = self
	return self
end

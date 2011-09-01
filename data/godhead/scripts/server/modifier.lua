Modifier = Class(Spec)
Modifier.type = "modifier"
Modifier.dict_id = {}
Modifier.dict_name = {}

--- Registers a new modifier type.
-- @param clss Modifier class.
-- @param args Arguments.<ul>
--   <li>name: Modifier name.</li>
--   <li>func: Update function.</li></ul>
-- @return Modifier.
Modifier.new = function(clss, args)
	local self = Spec.new(clss, args)
	return self
end

--- Updates the modifiers of the object.
-- @param clss Modifier class.
-- @param object Object whose modifiers to update.
-- @param secs Seconds since the last update.
-- @param args Arguments sent to the modifier
Modifier.update = function(clss, object, secs)
	local num = 0
	local keep = {}
	-- Update each modifier.
	for k,v in pairs(object.modifiers) do
		-- Update the modifier.
		local m = Modifier:find{name = k}
		if m and m.func then
			v = m:func(object, v, secs)
		else
			v.st = 0
		end
		-- Handle removal.
		if v.st > 0 then
			keep[k] = v
			num = num + 1
		else
			object:removed_modifier(k)
		end
	end
	-- Update the modifier list.
	if object.dead then
		for k,v in pairs(keep) do
			object:remove_modifier(k)
		end
		object.modifiers = nil
	else
		object.modifiers = num > 0 and keep or nil
	end
end

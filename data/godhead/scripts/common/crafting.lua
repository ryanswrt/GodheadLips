Crafting = Class()

--- Checks if a specific item can be crafted by the user.
-- @param clss Crafting class.
-- @param args Arguments.<ul>
--   <li>get_item: Function used to check if the user has enough materials.</li>
--   <li>get_skill: Function used to check if the user has enough skills.</li>
--   <li>spec: Item specification.</li></ul>
-- @return True if can craft.
Crafting.can_craft = function(clss, args)
	local spec = args.spec
	if not spec or not spec.crafting_enabled then return end
	-- Check for skills.
	for name,req in pairs(spec.crafting_skills) do
		local val = args.get_skill(name)
		if not val or val < req then return end
	end
	-- Check for materials.
	for name,req in pairs(spec.crafting_materials) do
		local cnt = args.get_item(name)
		if not cnt or cnt < req then return end
	end
	return true
end

--- Gets the names of all craftable items.
-- @param clss Crafting class.
-- @param args Arguments.<ul>
--   <li>get_item: Function used to check if the user has enough materials.</li>
--   <li>get_skill: Function used to check if the user has enough skills.</li></ul>
-- @return Table of item names.
Crafting.get_craftable = function(clss, args)
	local items = {}
	if args and args.get_item and args.get_skill then
		-- Check if the player has the materials and skills.
		local a = {}
		for k,v in pairs(args) do a[k] = v end
		for name,spec in pairs(Itemspec.dict_name) do
			a.spec = spec
			if clss:can_craft(a) then
				table.insert(items, name)
			end
		end
	else
		-- Return any craftable items regardless of the player status.
		for name,spec in pairs(Itemspec.dict_name) do
			if spec.crafting_enabled then
				table.insert(items, name)
			end
		end
	end
	return items
end

--- Gets the names of items that require the required itemspec for crafting.
-- @param clss Crafting class.
-- @param spec Itemspec.
-- @return Table of item names.
Crafting.get_requiring_items = function(clss, spec)
	local items = {}
	for k,v in pairs(Itemspec.dict_name) do
		if v.crafting_materials[spec.name] then
			table.insert(items, k)
		end
	end
	return items
end

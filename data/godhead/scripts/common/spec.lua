Spec = Class()
Spec.dict_id = {}
Spec.dict_cat = {}
Spec.dict_name = {}

--- Finds a spec.
-- @param clss Spec class.
-- @param args Arguments.<ul>
--   <li>category: Category name.</li>
--   <li>name: Spec name.</li>
--   <li>id: Spec ID.</li></ul>
-- @return Spec or nil if searching by name or ID. Table or nil otherwise.
Spec.find = function(clss, args)
	if not args then return end
	if args.category then
		return clss.dict_cat[args.category]
	end
	if args.name then
		return clss.dict_name[args.name]
	end
	if args.id then
		return clss.dict_id[args.id]
	end
end

--- Creates a new spec.
-- @param clss Spec class.
-- @param args Arguments.<ul>
--   <li>categories: List of categories or nil.</li>
--   <li>name: Spec name.</li></ul>
-- @return New item spec.
Spec.new = function(clss, args)
	local self = Class.new(clss, args)
	self.id = #clss.dict_id + 1
	-- Setup categories.
	self.categories = {}
	if args.categories then
		for k,v in pairs(args.categories) do
			self.categories[v] = true
		end
	end
	-- Register.
	clss.dict_id[self.id] = self
	clss.dict_name[self.name] = self
	for k in pairs(self.categories) do
		local cat = clss.dict_cat[k]
		if not cat then
			cat = {self}
			clss.dict_cat[k] = cat
		else
			table.insert(cat, self)
		end
	end
	return self
end

--- Returns a random spec.
-- @param clss Spec class.
-- @param args Arguments.<ul>
--   <li>category: Category name or nil.</li></ul>
-- @return Obstaclespec or nil.
Spec.random = function(clss, args)
	if args and args.name then
		return clss.dict_name[args.name]
	elseif args and args.category then
		local cat = clss.dict_cat[args.category]
		if not cat then return end
		return cat[math.random(#cat)]
	end
	return clss.dict_id[math.random(#clss.dict_id)]
end

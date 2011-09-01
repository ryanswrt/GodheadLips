Influencespec = Class(Spec)
Influencespec.class_name = "Influencespec"
Influencespec.type = "influence"
Influencespec.dict_id = {}
Influencespec.dict_cat = {}
Influencespec.dict_name = {}

--- Creates a new influence specification.
-- @param clss Influencespec class.
-- @param args Arguments.<ul>
--   <li>categories: List of categories to which the influence belongs.</li>
--   <li>func: Function to handle the influence.</li>
--   <li>name: Name of the influence type.</li></ul>
-- @return New influence specification.
Influencespec.new = function(clss, args)
	local self = Spec.new(clss, args)
	return self
end

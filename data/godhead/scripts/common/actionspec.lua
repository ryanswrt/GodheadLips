require "common/spec"

Actionspec = Class(Spec)
Actionspec.class_name = "Actionspec"
Actionspec.type = "action"
Actionspec.dict_id = {}
Actionspec.dict_cat = {}
Actionspec.dict_name = {}

--- Creates a new action specification.
-- @param clss Actionspec class.
-- @param args Arguments.<ul>
--   <li>categories: List of categories to which the action belongs.</li>
--   <li>func: Function to handle the action.</li>
--   <li>name: Name of the action type.</li></ul>
-- @return New action specification.
Actionspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	return self
end

require "common/spec"

Modelspec = Class(Spec)
Modelspec.class_name = "Modelspec"
Modelspec.type = "model"
Modelspec.dict_id = {}
Modelspec.dict_cat = {}
Modelspec.dict_name = {}

--- Creates a new model specification.
-- @param clss Modelspec class.
-- @param args Arguments.<ul>
--   <li>file: Overriding filename.</li>
--   <li>name: Name of the model.</li>
--   <li>replace_shaders: Dictionary of shaders overrides.</li></ul>
-- @return New model specification.
Modelspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	return self
end

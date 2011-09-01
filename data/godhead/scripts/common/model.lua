require "common/modelspec"

--- Finds or loads a model by name.
-- @param clss Model class.
-- @param args Arguments.<ul>
--   <li>file: Model filename.</li>
--   <li>mesh: False to not load the mesh.</li></ul>
-- @return Model.
Model.find_or_load = function(clss, args)
	-- Reuse existing models.
	local self = clss:find(args)
	if self then return self end
	-- Load a new model.
	local spec = Modelspec:find{name = args.file}
	self = Model{name = spec and spec.file or args.file}
	self:load(args)
	-- Replace shaders.
	if args.mesh ~= false and spec and spec.replace_shaders then
		for k,v in pairs(spec.replace_shaders) do
			self:edit_material{match_shader = k, shader = v}
		end
		self:changed()
	end
	return self
end

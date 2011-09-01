require "system/tiles"
require "common/model"

Material.dict_id = {}
Material.dict_name = {}
local instfunc = Material.new

--- Finds a material
-- @param clss Model class.
-- @param args Arguments.<ul>
--   <li>id: Material ID.</li>
--   <li>name: Material name.</li></ul>
-- @return Model or nil.
Material.find = function(clss, args)
	if args.id then
		return clss.dict_id[args.id]
	end
	if args.name then
		return clss.dict_name[args.name]
	end
end

--- Creates a new material.
-- @param clss Model class.
-- @param args Arguments.
-- @return New models.
Material.new = function(clss, args)
	if type(args.model) == "string" then
		args.model = Model:find_or_load{file = args.model}
	end
	local self = instfunc(clss, args)
	clss.dict_id[self.id] = self
	clss.dict_name[self.name] = self
	return self
end

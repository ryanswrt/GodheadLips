require "server/generator"

Generator.Border = Class()
Generator.Border.class_name = "Generator.Border"
Generator.Border.mats = {
	Material:find{name = "basalt1"}} -- FIXME

--- Generates a map border area.
-- @param self Border generator.
-- @param pos Offset of the generated area.
-- @param size Size of the generated area.
Generator.Border.generate = function(self, pos, size)
	Voxel:fill_region{point = pos, size = size, tile = self.mats[1].id}
end

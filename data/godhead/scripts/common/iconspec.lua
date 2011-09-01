require "common/spec"

Iconspec = Class(Spec)
Iconspec.type = "icon"
Iconspec.dict_id = {}
Iconspec.dict_cat = {}
Iconspec.dict_name = {}

--- Creates a new icon specification.
-- @param clss Iconspec class.
-- @param args Arguments.<ul>
--   <li>categories: List of categories to which the icon belongs.</li>
--   <li>name: Name of the item type.</li>
--   <li>source_image: Source image name.</li>
--   <li>source_position: Pixel offset in the source image.</li></ul>
-- @return New icon specification.
Iconspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.source_image = self.source_image or "icons1"
	self.source_position = self.source_position or {0,0}
	return self
end

Widgets.Log = Class(Widgets.Label)

Widgets.Log.new = function(clss, args)
	local self = Widgets.Label.new(clss, args)
	self.lines = {}
	return self
end

Widgets.Log.append = function(self, args)
	-- Append and scroll.
	local l = self.lines
	table.insert(l, args.text)
	if #l > 5 then table.remove(l, 1) end
	-- Rebuild the text.
	local t = l[1]
	for i = 2,#l do
		t = t .. "\n" .. l[i]
	end
	self.text = t
end

Widgets.Log.reshaped = function(self)
	self:set_request{
		font = self.font,
		internal = true,
		paddings = {2,2,2,2},
		text = self.text,
		width = self:get_request()}
	local w = self.width
	local h = self.height
	self:canvas_clear()
	self:canvas_text{
		dest_position = {0,0},
		dest_size = {w,h},
		text = self.text,
		text_alignment = {0,1},
		text_color = {1,1,1,1},
		text_font = self.font}
	self:canvas_compile()
end

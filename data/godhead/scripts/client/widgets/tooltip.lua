Widgets.Tooltip = Class(Widgets.Frame)

Widgets.Tooltip.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.spacings = {0,0}
	self.style = "tooltip"
	self.cols = 1
	if self.text then
		self:append(self.text)
		self.text = nil
	end
	return self
end

Widgets.Tooltip.append = function(self, what)
	if type(what) == "string" then
		local label = Widgets.Label{text = what}
		label:set_request{width = 150}
		self:append_row(label)
	else
		self:append_row(what)
	end
end

Widgets.Tooltip.popup = function(self, point)
	local mode = Program.video_mode
	self.floating = true
	if point.x > mode[1] - self.width then
		self.x = mode[1] - self.width
	else
		self.x = point.x
	end
	if point.y > mode[2] - self.height then
		self.y = mode[2] - self.height
	else
		self.y = point.y
	end
end

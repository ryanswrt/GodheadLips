Widgets.Markup = Class(Widget)
Widgets.Markup.class_name = "Widgets.Markup"

Widgets.Markup.new = function(clss, args)
	local arg = function(n, d) return args and args[n] or d end
	local self = Widget.new(clss, {cols = 1, rows = 0})
	self:set_expand{col = 1}
	self.text = arg("text", {})
	self.width_request = arg("width_request")
	self:build()
	return self
end

Widgets.Markup.build = function(self)
	self.rows = 0
	if self.text then
		for k,v in ipairs(self.text) do
			if v[1] == "link" then
				-- Help links.
				local w = Widgets.Button{text = v[2], width_request = self.width_request,
					pressed = function() Client.views.help:show(v[3]) end}
				self:append_row(w)
			elseif v[1] == "paragraph" then
				-- Paragraphs.
				local w = Widgets.Label{color = {0,0,0,1}, text = v[2],
					width_request = self.width_request}
				self:append_row(w)
			end
		end
	end
end

Widgets.Markup:add_getters{
	text = function(self) return rawget(self, "__text") end}

Widgets.Markup:add_setters{
	text = function(self, v)
		rawset(self, "__text", v)
	end}

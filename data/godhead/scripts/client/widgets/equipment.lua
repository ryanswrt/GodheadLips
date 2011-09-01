Widgets.Equipment = Class(Widgets.Frame)

Widgets.Equipment.new = function(clss, args)
	local self = Widgets.Frame.new(clss, {cols = 4, pressed = args.pressed, style = "equipment"})
	self:set_expand{col = 4}
	-- TODO: Different formats for different species using args.spec.
	self.dict_name = {}
	self.dict_name["head"] = Widgets.ItemButton{pressed = function(w, a) self:pressed(a, "head") end}
	self.dict_name["upperbody"] = Widgets.ItemButton{pressed = function(w, a) self:pressed(a, "upperbody") end}
	self.dict_name["hand.L"] = Widgets.ItemButton{pressed = function(w, a) self:pressed(a, "hand.L") end}
	self.dict_name["hand.R"] = Widgets.ItemButton{pressed = function(w, a) self:pressed(a, "hand.R") end}
	self.dict_name["lowerbody"] = Widgets.ItemButton{pressed = function(w, a) self:pressed(a, "lowerbody") end}
	self.dict_name["feet"] = Widgets.ItemButton{pressed = function(w, a) self:pressed(a, "feet") end}
	self.dict_name["arms"] = Widgets.ItemButton{pressed = function(w, a) self:pressed(a, "arms") end}
	self:append_row(Widgets.Label(), self.dict_name["head"])
	self:append_row(self.dict_name["arms"], self.dict_name["upperbody"])
	self:append_row(self.dict_name["hand.R"], self.dict_name["lowerbody"], self.dict_name["hand.L"])
	self:append_row(Widgets.Label(), self.dict_name["feet"])
	return self
end

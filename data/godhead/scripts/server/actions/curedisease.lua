-- Cures diseases.
Actionspec{name = "curedisease", type = "item", func = function(item, user)
	user:remove_modifier("plague")
	item:subtract{count = 1}
	user:add_item{object = Item{spec = Itemspec:find{name = "empty bottle"}}}
end}

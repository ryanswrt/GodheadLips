-- Loots a container.
Actionspec{name = "loot", type = "item", func = function(item, user)
	item:loot(user)
end}

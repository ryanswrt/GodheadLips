require "common/spec"

Dialogspec = Class(Spec)
Dialogspec.type = "dialog"
Dialogspec.dict_id = {}
Dialogspec.dict_cat = {}
Dialogspec.dict_name = {}

--- Creates a new dialog specification.
-- @param clss Dialogspec class.
-- @param args Arguments.<ul>
--   <li>name: Unique dialog name.</li>
--   <li>unique: True to allow only one access to the dialog at a time.</li></ul>
-- @return New dialog specification.
Dialogspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	local copy = function(f, d) if self[f] == nil then self[f] = d end end
	copy("unique")
	self.commands = {}
	for k,v in ipairs(args) do self.commands[k] = v end
	self:validate()
	return self
end

--- Validates the dialog.
-- @param self Dialogspec.
Dialogspec.validate = function(self, args)
	local validate_arguments
	local validate_branch
	local validate_command
	local validate_cond
	local commands = {
		branch = function(c)
			assert(type(c.cond) == "nil" or type(c.cond) == "string", "argument \"cond\" of \"branch\" must be the flag string")
			assert(type(c.cond_dead) == "nil" or type(c.cond_dead) == "boolean", "argument \"cond_dead\" of \"branch\" must be a boolean")
			assert(type(c.cond_not) == "nil" or type(c.cond_not) == "string", "argument \"cond_not\" of \"branch\" must be the flag string")
			validate_arguments(c, {check = true, cond = true, cond_dead = true, cond_not = true})
			validate_cond(c.check)
			validate_branch(c, 2)
		end,
		["break"] = function(c)
			assert(type(c[2]) == "nil" or type(c[2]) == "number", "argument #2 of \"break\" must be the number of branches to break")
			assert(type(c[3]) == "nil", "too many arguments to \"break\" command")
			validate_arguments(c, {})
		end,
		choice = function(c)
			assert(type(c[2]) == "string", "argument #2 of \"choice\" must be the choice string")
			assert(type(c.cond) == "nil" or type(c.cond) == "string", "argument \"cond\" of \"choice\" must be the flag string")
			assert(type(c.cond_not) == "nil" or type(c.cond_not) == "string", "argument \"cond_not\" of \"choice\" must be the flag string")
			validate_arguments(c, {check = true, cond = true, cond_not = true})
			validate_cond(c.check)
			validate_branch(c, 3)
		end,
		["default death check"] = function(c)
			assert(type(c[2]) == "nil", "too many arguments to \"loot\" command")
			validate_arguments(c, {})
		end,
		effect = function(c)
			assert(type(c[2]) == "nil" or type(c[2]) == "string", "argument #2 of \"effect\" must be the effect name string")
		end,
		["effect player"] = function(c)
			assert(type(c[2]) == "nil" or type(c[2]) == "string", "argument #2 of \"effect player\" must be the effect name string")
		end,
		exit = function(c)
			assert(type(c[2]) == "nil", "too many arguments to \"exit\" command")
			validate_arguments(c, {})
		end,
		feat = function(c)
			assert(type(c[2]) == "nil", "too many arguments to \"feat\" command")
			assert(type(c.category) == "nil" or type(c.category) == "string", "argument \"category\" of \"feat\" must be a feat category string")
			validate_arguments(c, {category = true})
		end,
		flag = function(c)
			assert(type(c[2]) == "string", "argument #2 of \"flag\" must be the flag name")
			assert(type(c[3]) == "nil", "too many arguments to \"flag\" command")
			validate_arguments(c, {})
		end,
		func = function(c)
			assert(type(c[2]) == "function", "argument #2 of \"func\" must be a function")
			assert(type(c[3]) == "nil", "too many arguments to \"func\" command")
			validate_arguments(c, {})
		end,
		["give player item"] = function(c)
			assert(type(c[2]) == "string", "argument #2 of \"give player item\" must be the item name string")
			assert(type(c[3]) == "nil", "command \"give player item\" only takes 1 argument")
			validate_arguments(c, {count = true})
		end,
		info = function(c)
			assert(type(c[2]) == "string", "argument #2 of \"info\" must be the info string")
			assert(type(c[3]) == "nil", "too many arguments to \"info\" command")
			validate_arguments(c, {})
		end,
		loop = function(c)
			assert(type(c[2]) == "nil", "too many arguments to \"loop\" command")
			validate_arguments(c, {})
		end,
		loot = function(c)
			assert(type(c[2]) == "nil", "too many arguments to \"loot\" command")
			validate_arguments(c, {})
		end,
		notification = function(c)
			assert(type(c[2]) == "string", "argument #2 of \"notification\" must be the notification string")
			assert(type(c[3]) == "nil", "too many arguments to \"notification\" command")
			validate_arguments(c, {})
		end,
		quest = function(c)
			assert(type(c[2]) == "string", "argument #2 of \"quest\" must be the quest name")
			assert(type(c[3]) == "nil", "too many arguments to \"quest\" command")
			assert(type(c.marker) == "nil" or type(c.marker) == "string", "argument \"marker\" of \"quest\" must be a marker name string")
			assert(c.status == nil or c.status == "active" or c.status == "completed", "argument \"status\" of \"quest\" must be either \"active\" or \"completed\"")
			assert(type(c.text) == "nil" or type(c.text) == "string", "argument \"text\" of \"quest\" must be a quest text string")
			validate_arguments(c, {marker = true, status = true, text = true})
		end,
		random = function(c)
			assert(type(c[2]) == "table", "command \"random\" must have at least one branch")
			validate_arguments(c, {})
			validate_branch(c, 2)
		end,
		["remove player item"] = function(c)
			assert(type(c[2]) == "string", "argument #2 of \"remove player item\" must be the item name string")
			assert(type(c[3]) == "table", "argument #3 of \"remove player item\" must be a script branch")
			assert(type(c[4]) == "table" or type(c[4]) == "nil", "argument #4 of \"remove player item\" must be a script branch")
			assert(type(c[5]) == "nil", "command \"remove player item\" can only have two child commands")
			validate_arguments(c, {})
			validate_branch(c, 3)
		end,
		["require player item"] = function(c)
			assert(type(c[2]) == "string", "argument #2 of \"require player item\" must be the item name string")
			assert(type(c[3]) == "table", "argument #3 of \"require player item\" must be a script branch")
			assert(type(c[4]) == "table" or type(c[4]) == "nil", "argument #4 of \"require player item\" must be a script branch")
			assert(type(c[5]) == "nil", "command \"require player item\" can only have two child commands")
			validate_arguments(c, {})
			validate_branch(c, 3)
		end,
		say = function(c)
			assert(type(c[2]) == "string", "argument #2 of \"say\" must be the name of the NPC")
			assert(type(c[3]) == "string", "argument #3 of \"say\" must be the said string")
			assert(type(c[4]) == "nil", "too many arguments to \"say\" command")
			validate_arguments(c, {})
		end,
		["spawn object"] = function(c)
			assert(type(c[2]) == "string", "argument #2 of \"spawn object\" must be the name of the object")
			assert(type(c[3]) == "nil", "too many arguments to \"spawn object\" command")
			validate_arguments(c, {assign_marker = true, distance_max = true, distance_min = true, position_absolute = true, position_marker = true, position_relative = true, rotation = true})
		end,
		["spawn pattern"] = function(c)
			assert(type(c[2]) == "string", "argument #2 of \"spawn pattern\" must be the name of the pattern")
			assert(type(c[3]) == "nil", "too many arguments to \"spawn pattern\" command")
			validate_arguments(c, {distance_max = true, distance_min = true, erase_tiles = true, position_absolute = true, position_marker = true, position_relative = true, rotation = true})
		end,
		teleport = function(c)
			assert(type(c[2]) == "nil", "too many arguments to \"teleport\" command")
			assert(type(c.marker) == "string", "argument \"marker\" of \"teleport\" must be the name of the map marker")
			validate_arguments(c, {marker = true})
		end,
		trade = function(c)
			assert(type(c[2]) == "nil", "too many arguments to \"trade\" command")
			validate_arguments(c, {})
		end,
		["unlock marker"] = function(v)
			assert(type(c[2]) == "string", "argument #2 of \"unlock marker\" must be the name of the marker")
			assert(type(c[3]) == "nil", "too many arguments to \"unlock marker\" command")
			validate_arguments(c, {})
		end,
		var = function(c)
			assert(type(c[2]) == "string", "argument #2 of \"var\" must be the variable name")
			assert(type(c[3]) == "nil", "too many arguments to \"var\" command")
			validate_arguments(c, {})
		end}
	validate_arguments = function(cmd, allow)
		for k,v in pairs(cmd) do
			if type(k) == "string" and not allow[k] then
				assert(false, string.format("invalid named argument \"%s\" to command \"%s\"", k, cmd[1]))
			end
		end
	end
	validate_command = function(c)
		assert(type(c[1]) == "string", "argument #1 of dialog command must be the command name string")
		local f = commands[c[1]]
		assert(f, string.format("invalid dialog command \"%s\"", c[1]))
		f(c)
	end
	validate_cond = function(c)
		if not c then return true end
		assert(type(c) == "string", "argument \"check\" must be a string")
		for _,str in ipairs(string.split(c, "&")) do
			local list = string.split(str, ":")
			assert(#list <= 2, string.format("invalid check %q", c))
			local type,name = list[1],list[2]
			if type == "dead" then
				assert(not name, string.format("invalid check %q", c))
			elseif type == "!dead" then
				assert(not name, string.format("invalid check %q", c))
			elseif type == "flag" then
				assert(name, string.format("invalid check %q", c))
			elseif type == "!flag" then
				assert(name, string.format("invalid check %q", c))
			elseif type == "var" then
				assert(name, string.format("invalid check %q", c))
			elseif type == "!var" then
				assert(name, string.format("invalid check %q", c))
			else
				assert(false, string.format("invalid check %q", c))
			end
		end
	end
	validate_branch = function(b, start, name)
		for k,v in ipairs(b) do
			if k >= start then
				assert(type(v) == "table", string.format("command #%d of \"%s\" must be a table", k - start + 1, name or b[1]))
				validate_command(v)
			end
		end
	end
	local o,e = pcall(function() validate_branch(self.commands, 1, "Dialogspec") end)
	if not o then
		print(string.format("ERROR: in dialog \"%s\": %s", self.name,
			string.match(e, ".*:[0-9]*: (.*)")))
	end
end

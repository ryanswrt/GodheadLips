Serialize = Class()
Serialize.account_version = "2"
Serialize.data_version = "9"

--- Initializes the serializer.
-- @param clss Serialize class.
Serialize.init = function(clss)
	-- Create the save database.
	clss.db = Database{name = "save" .. Settings.file .. ".sqlite"}
	clss.sectors = Sectors{database = clss.db}
	clss.db:query("CREATE TABLE IF NOT EXISTS keyval (key TEXT PRIMARY KEY,value TEXT);")
	if clss:get_value("data_version") ~= clss.data_version then
		clss.db:query("DROP TABLE IF EXISTS dialog_flags;")
		clss.db:query("DROP TABLE IF EXISTS markers;")
		clss.db:query("DROP TABLE IF EXISTS quests;")
	end
	clss.db:query("CREATE TABLE IF NOT EXISTS dialog_flags (name TEXT PRIMARY KEY,value TEXT);")
	clss.db:query("CREATE TABLE IF NOT EXISTS generator_sectors (id INTEGER PRIMARY KEY,value TEXT);")
	clss.db:query("CREATE TABLE IF NOT EXISTS generator_settings (key TEXT PRIMARY KEY,value TEXT);")
	clss.db:query("CREATE TABLE IF NOT EXISTS markers (name TEXT PRIMARY KEY,id INTEGER,x FLOAT,y FLOAT,z FLOAT,unlocked INTENGER,discoverable INTEGER);")
	clss.db:query("CREATE TABLE IF NOT EXISTS quests (name TEXT PRIMARY KEY,status TEXT,desc TEXT,marker TEXT);")
	Sectors.instance = clss.sectors
	-- Create the account database.
	clss.accounts = Database{name = "accounts" .. Settings.file .. ".sqlite"}
	clss.accounts:query("CREATE TABLE IF NOT EXISTS keyval (key TEXT PRIMARY KEY,value TEXT);")
	local rows = clss.accounts:query("SELECT value FROM keyval WHERE key=?;", {"account_version"})
	if #rows ~= 1 or rows[1][1] ~= clss.account_version then
		clss.accounts:query("DROP TABLE IF EXISTS accounts;")
		clss.accounts:query("DELETE FROM keyval;")
		clss.accounts:query("REPLACE INTO keyval (key,value) VALUES (?,?);", {"account_version", clss.account_version})
	end
	clss.accounts:query("CREATE TABLE IF NOT EXISTS accounts (login TEXT PRIMARY KEY,password TEXT,permissions INTEGER,character TEXT,spawn_point TEXT);")
	rows = clss.accounts:query("SELECT value FROM keyval WHERE key=?;", {"password_salt"})
	if #rows ~= 1 then
		clss.accounts.password_salt = Password:random_salt()
		clss.accounts:query("REPLACE INTO keyval (key,value) VALUES (?,?);", {"password_salt", clss.accounts.password_salt})
	else
		clss.accounts.password_salt = rows[1][1]
	end
end

--- Makes a string out of an inventory and saves the items to the database.
-- @param clss Serialize class.
-- @param inv Inventory.
-- @return String.
Serialize.encode_inventory = function(clss, inv)
	if not inv then return "" end
	local str = ""
	for slot,obj in pairs(inv.slots) do
		obj:save()
		str = string.format("%sself:add_item{slot=%s,object=Object:load{id=%s}}\n",
			str, serialize(slot), serialize(obj.id))
	end
	return str
end

--- Makes a string out of skills.
-- @param clss Serialize class.
-- @param skills Skills.
-- @return String.
Serialize.encode_skills = function(clss, skills)
	if not skills then return "" end
	local str = string.format("self.skills.enabled=%s\n", serialize(skills.enabled))
	for k,v in pairs(skills:get_names()) do
		local val = skills:get_value{skill = v}
		local max = skills:get_maximum{skill = v}
		str = string.format("%sself.skills:set{skill=%s,maximum=%s,value=%s}\n",
			str, serialize(v), serialize(max), serialize(val))
	end
	return str
end

--- Gets a value from the key-value database.
-- @param clss Serialize class.
-- @param key Key string.
-- @return Value string or nil.
Serialize.get_value = function(clss, key)
	local rows = clss.db:query("SELECT value FROM keyval WHERE key=?;", {key})
	for k,v in ipairs(rows) do
		return v[1]
	end
end

--- Loads everything except map data.
-- @param clss Serialize class.
Serialize.load = function(clss)
	clss:load_generator()
	clss:load_markers()
	clss:load_quests()
end

--- Loads an account from the account database.
-- @param clss Serialize class.
-- @param login Login name.
-- @return Account database row or nil.
Serialize.load_account = function(clss, login)
	local r = clss.accounts:query("SELECT login,password,permissions,character,spawn_point FROM accounts WHERE login=?;", {login})
	for k,v in ipairs(r) do
		return v
	end
end

--- Loads map generator data from the database.
-- @param clss Serialize class.
Serialize.load_generator = function(clss)
	-- Load settings.
	local r1 = clss.db:query("SELECT key,value FROM generator_settings;")
	local f1 = {
		seed1 = function(v) Generator.inst.seed1 = tonumber(v) end,
		seed2 = function(v) Generator.inst.seed2 = tonumber(v) end,
		seed3 = function(v) Generator.inst.seed3 = tonumber(v) end}
	for k,v in ipairs(r1) do
		local f = f1[v[1]]
		if f then f(v[2]) end
	end
	-- Load special sectors.
	local r2 = clss.db:query("SELECT id,value FROM generator_sectors;")
	for k,v in ipairs(r2) do
		Generator.inst.sectors[v[1]] = v[2]
	end
end

--- Loads map markers from the database.
-- @param clss Serialize class.
Serialize.load_markers = function(clss)
	local r = clss.db:query("SELECT name,id,x,y,z,unlocked,discoverable FROM markers;")
	for k,v in ipairs(r) do
		Marker{name = v[1], target = v[2], position = Vector(v[3], v[4], v[5]),
			unlocked = (v[6] == 1), discoverable = (v[7] == 1)}
	end
end

--- Loads quests from the database.
-- @param clss Serialize class.
Serialize.load_quests = function(clss)
	local r = clss.db:query("SELECT name,status,desc,marker FROM quests;")
	for k,v in ipairs(r) do
		local quest = Quest:find{name = v[1]}
		if quest then
			quest:update{status = v[2], text = v[3], marker = v[4]}
		end
	end
	local r = clss.db:query("SELECT name,value FROM dialog_flags;")
	for k,v in ipairs(r) do
		Dialog.flags[v[1]] = v[2]
	end
end

--- Saves everything.
-- @param clss Serialize class.
-- @param erase True to erase existing database entries first.
Serialize.save = function(clss, erase)
	clss.sectors:save_world(erase)
	clss:save_generator(erase)
	clss:save_markers(erase)
	clss:save_quests(erase)
	clss:save_accounts(erase)
end

--- Saves a player account.
-- @param clss Serialize class.
-- @param account Account.
-- @param object Player object or nil.
Serialize.save_account = function(clss, account, object)
	clss.accounts:query("BEGIN TRANSACTION;")
	clss.accounts:query("REPLACE INTO accounts (login,password,permissions,character,spawn_point) VALUES (?,?,?,?,?);",
		{account.login, account.password, account.permissions, object and object:write(), account.spawn_point and tostring(account.spawn_point)})
	clss.accounts:query("END TRANSACTION;")
end

--- Saves all active player accounts.
-- @param clss Serialize class.
-- @param erase True to erase characters, "all" to erase all account data.
Serialize.save_accounts = function(clss, erase)
	-- Delete accounts or characters.
	if erase == "all" then
		clss.accounts:query("DELETE FROM accounts;")
	elseif erase then
		clss.accounts:query("UPDATE accounts SET character = NULL;")
		clss.accounts:query("UPDATE accounts SET spawn_point = NULL;")
	end
	-- Write accounts.
	for k,v in pairs(Player.clients) do
		clss:save_account(v.account, v)
	end
end

--- Saves the map generator state.
-- @param clss Serialize class.
-- @param erase True to erase existing database entries first.
Serialize.save_generator = function(clss, erase)
	clss.db:query("BEGIN TRANSACTION;")
	if erase then
		clss.db:query("DELETE FROM generator_settings;")
		clss.db:query("DELETE FROM generator_sectors;")
	end
	clss.db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed1", tostring(Generator.inst.seed1)})
	clss.db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed2", tostring(Generator.inst.seed2)})
	clss.db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed3", tostring(Generator.inst.seed3)})
	for k,v in pairs(Generator.inst.sectors) do
		clss.db:query("REPLACE INTO generator_sectors (id,value) VALUES (?,?);", {k, v})
	end
	clss.db:query("END TRANSACTION;")
end

--- Saves a map marker.
-- @param clss Serialize class.
-- @param marker Map marker.
Serialize.save_marker = function(clss, marker)
	clss.db:query("BEGIN TRANSACTION;")
	clss.db:query("REPLACE INTO markers (name,id,x,y,z,unlocked,discoverable) VALUES (?,?,?,?,?,?,?);",
		{marker.name, marker.target, marker.position.x, marker.position.y, marker.position.z,
		 marker.unlocked and 1 or 0, marker.discoverable and 1 or 0})
	clss.db:query("END TRANSACTION;")
end

--- Saves all map markers.
-- @param clss Serialize class.
-- @param erase True to erase existing database entries first.
Serialize.save_markers = function(clss, erase)
	clss.db:query("BEGIN TRANSACTION;")
	if erase then
		clss.db:query("DELETE FROM markers;")
	end
	for k,v in pairs(Marker.dict_name) do
		clss.db:query("REPLACE INTO markers (name,id,x,y,z,unlocked,discoverable) VALUES (?,?,?,?,?,?,?);",
			{k, v.target, v.position.x, v.position.y, v.position.z,
			 v.unlocked and 1 or 0, v.discoverable and 1 or 0})
	end
	clss.db:query("END TRANSACTION;")
end

--- Saves a quest.
-- @param clss Serialize class.
-- @param quest Quest.
Serialize.save_quest = function(clss, quest)
	clss.db:query("BEGIN TRANSACTION;")
	clss.db:query("REPLACE INTO quests (name,status,desc,marker) VALUES (?,?,?,?);",
		{quest.name, quest.status, quest.text, quest.marker})
	clss.db:query("END TRANSACTION;")
end

--- Saves all quests.
-- @param clss Serialize class.
-- @param erase True to erase existing database entries first.
Serialize.save_quests = function(clss, erase)
	clss.db:query("BEGIN TRANSACTION;")
	if erase then
		clss.db:query("DELETE FROM quests;")
		clss.db:query("DELETE FROM dialog_flags;")
	end
	for k,v in pairs(Quest.dict_name) do
		clss.db:query("REPLACE INTO quests (name,status,desc,marker) VALUES (?,?,?,?);",
			{k, v.status, v.text, v.marker})
	end
	for k,v in pairs(Dialog.flags) do
		clss.db:query("REPLACE INTO dialog_flags (name,value) VALUES (?,?);", {k, v})
	end
	clss.db:query("END TRANSACTION;")
end

--- Stores a value to the key-value database.
-- @param clss Serialize class.
-- @param key Key string.
-- @param value Value string.
Serialize.set_value = function(clss, key, value)
	clss.db:query("REPLACE INTO keyval (key,value) VALUES (?,?);", {key, value})
end

Serialize:init()

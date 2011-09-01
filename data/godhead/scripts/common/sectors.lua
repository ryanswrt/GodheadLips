Sectors = Class()

--- Initializes the serializer.
-- @param clss Serialize class.
-- @param args Arguments.<ul>
--   <li>database: Database.</li>
--   <li>save_objects: False to disable saving of objects.</li>
--   <li>save_terrain: False to disable saving of terrain.</li>
--   <li>unload_time: Number of seconds it takes for an inactive sector to be unloaded.</li></ul>
-- @return New sectors.
Sectors.new = function(clss, args)
	local self = Class.new(clss, args)
	self.sectors = {}
	self.save_objects = (self.save_objects ~= false)
	self.save_terrain = (self.save_terrain ~= false)
	self.unload_time = self.unload_time or 10
	-- Use asynchronous writes for much better write performance.
	self.database:query("PRAGMA synchronous=OFF;")
	self.database:query("PRAGMA count_changes=OFF;")
	-- Initialize the database tables needed by us.
	if self.save_objects then
		self.database:query("CREATE TABLE IF NOT EXISTS objects (id INTEGER PRIMARY KEY,sector UNSIGNED INTEGER,data TEXT);");
	end
	if self.save_terrain then
		self.database:query("CREATE TABLE IF NOT EXISTS terrain (sector INTEGER PRIMARY KEY,data BLOB);");
	end
	-- Load and unload sectors automatically from now on.
	self.handler = Eventhandler{type = "sector-load", func = function(_,a) self:load_sector(a.sector) end}
	self.timer = Timer{delay = 2, func = function() self:update() end}
	return self
end

--- Called when a sector is created by the game.
-- @param self Sectors.
-- @param sector Sector index.
-- @param terrain True if terrain was loaded.
-- @param objects Array of objects.
Sectors.created_sector = function(self, sector, terrain, objects)
end

--- Removes all sectors from the database.
-- @param self Sectors.
Sectors.erase_world = function(self, erase)
	self.database:query("BEGIN TRANSACTION;")
	if self.save_objects then self.database:query("DELETE FROM objects;") end
	if self.save_terrain then self.database:query("DELETE FROM terrain;") end
	self.database:query("END TRANSACTION;")
end

--- Reads a sector from the database.
-- @param self Sectors.
-- @param sector Sector index.
Sectors.load_sector = function(self, sector)
	local objects = {}
	local terrain = nil
	-- Only load once.
	if self.sectors[sector] then return end
	self.sectors[sector] = true
	-- Load terrain.
	if self.save_terrain then
		local rows = self.database:query("SELECT * FROM terrain WHERE sector=?;", {sector})
		if #rows ~= 0 then
			for k,v in ipairs(rows) do
				Voxel:paste_region{sector = sector, packet = v[2]}
			end
			terrain = true
		end
	end
	-- Load objects.
	if self.save_objects then
		local rows = self.database:query("SELECT * FROM objects WHERE sector=?;", {sector})
		for k,v in ipairs(rows) do
			local func = assert(loadstring("return function()\n" .. v[3] .. "\nend"))()
			if func then
				local ok,ret = pcall(func)
				if not ok then
					print(ret)
				elseif ret then
					ret.realized = true
					table.insert(objects, ret)
				end
			end
		end
	end
	-- Load custom content.
	self:created_sector(sector, terrain, objects)
end

--- Saves a sector to the database.
-- @param self Sectors.
-- @param sector Sector index.
Sectors.save_sector = function(self, sector)
	-- Write objects.
	if self.save_objects then
		self.database:query("DELETE FROM objects WHERE sector=?;", {sector})
		local objs = Object:find{sector = sector}
		for k,v in pairs(objs) do
			if v.class ~= Player or not v.client then
				v:save()
			end
		end
	end
	-- Write terrain.
	if self.save_terrain then
		self.database:query("DELETE FROM terrain WHERE sector=?;", {sector})
		local data = Voxel:copy_region{sector = sector}
		self.database:query("INSERT INTO terrain (sector,data) VALUES (?,?);", {sector, data})
	end
end

--- Saves all active sectors to the database.
-- @param self Sectors.
-- @param erase True to completely erase the old map.
-- @param progress Progress callback.
Sectors.save_world = function(self, erase, progress)
	self.database:query("BEGIN TRANSACTION;")
	-- Erase old world from the database.
	if erase then
		if self.save_objects then self.database:query("DELETE FROM objects;") end
		if self.save_terrain then self.database:query("DELETE FROM terrain;") end
	end
	-- Write the new world data.
	local sectors = Program.sectors
	if progress then
		-- Write with progress updates.
		local total = 0
		for k in pairs(sectors) do total = total + 1 end
		local i = 1
		for k,v in pairs(sectors) do
			self:save_sector(k)
			progress(i / total)
			i = i + 1
		end
	else
		-- Write without progress updates.
		for k,v in pairs(sectors) do
			self:save_sector(k)
		end
	end
	self.database:query("END TRANSACTION;")
end

--- Unloads the world without saving.
-- @param self Sectors.
Sectors.unload_world = function(self)
	Program:unload_world()
	self.sectors = {}
end

--- Unloads sectors that have been inactive long enough.
-- @param self Sectors.
Sectors.update = function(self)
	if not self.unload_time then return end
	local written = 0
	for k,d in pairs(Program.sectors) do
		if d > self.unload_time and written < 40 then
			-- Group into a single transaction.
			if written == 0 then self.database:query("BEGIN TRANSACTION;") end
			written = written + 1
			-- Save and unload the sector.
			self:save_sector(k)
			self.sectors[k] = nil
			Program:unload_sector{sector = k}
		end
	end
	-- Finish the transaction.
	if written > 0 then self.database:query("END TRANSACTION;") end
end

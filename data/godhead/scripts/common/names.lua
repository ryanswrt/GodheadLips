Names = Class()
Names.races = {}

--- Returns a random character name.
-- @param clss Names class.
-- @param args Arguments.<ul>
--   <li>race: Race name.</li>
--   <li>gender: Gender string: "male"/"female".</li>
-- @return Name or nil.
Names.random = function(clss, args)
	local race = (args and args.race or "aer")
	local t = clss.races[race]
	if not t then return end
	return t.list[math.random(1, t.count)]
end

--- Registers a random character name.
-- @param clss Names class.
-- @param args Arguments.<ul>
--   <li>race: Race name.</li>
--   <li>gender: Gender string, "male" or "female".</li>
Names.register = function(clss, args)
	local race = (args.race or "aer")
	local t = clss.races[race]
	if not t then
		t = {count = 0, dict = {}, list = {}}
		clss.races[race] = t
	end
	if t.dict[args.name] then return end
	t.count = t.count + 1
	t.list[t.count] = args.name
	t.dict[args.name] = true
end

-- Register some names.
do
	-- Aer female: Poetic theme.
	local aerf = {"Bella", "Cherry", "Fate", "Iris", "Lori", "Luna", "Sonata", "Sophie", "Sumi", "Terra"}
	-- Aer male: Cool theme.
	local aerm = {"Adam", "Simon"}
	-- Devora: Nonsense exclamations. Single gender only.
	local devora = {"Kya", "Myu", "Pui", "Pyo", "Unyu"}
	-- Kraken: Water theme. Single gender only.
	local kraken = {"Aqua", "Coral", "Marina", "Pearl", "Pristina"}
	-- Wyrm: Lizard names. Female names only differ by ending to an A.
	local wyrm = {"Draggsl", "Slisht", "Xhassl"}
	for k,v in pairs(aerf) do Names:register{race = "aer", name = v} end
	for k,v in pairs(aerm) do Names:register{race = "aermale", name = v} end
	for k,v in pairs(aerf) do Names:register{race = "android", name = v} end
	for k,v in pairs(aerm) do Names:register{race = "androidmale", name = v} end
	for k,v in pairs(devora) do Names:register{race = "devora", name = v} end
	for k,v in pairs(kraken) do Names:register{race = "kraken", name = v} end
	for k,v in pairs(wyrm) do
		Names:register{race = "wyrm", name = v}
		Names:register{race = "wyrmmale", name = v .. "a"}
	end
end

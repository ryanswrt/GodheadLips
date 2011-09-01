require "common/string"

Settings = Class()
Settings.arguments = string.split(Program.args)
Settings.address = "localhost"
Settings.port = 10101
Settings.account = "guest"
Settings.password = ""
Settings.file = 1

--- Parses command line arguments.
-- @param clss Settings class.
Settings.parse_command_line = function(clss)
	local i = 1
	local a = clss.arguments
	local parse_pattern = function()
		local num = 0
		while i <= #a and string.sub(a[i], 1, 1) ~= "-" do
			if clss.pattern then
				clss.pattern = clss.region .. " " .. a[i]
			else
				clss.pattern = a[i]
			end
			i = i + 1
		end
		if not clss.pattern then clss.pattern = "spawnpoint1" end
		return num
	end
	local parse_addr_port = function()
		if i > #a or string.sub(a[i], 1, 1) == "-" then return 0 end
		Settings.address = a[i]
		if i > #a or string.sub(a[i + 1], 1, 1) == "-" then return 1 end
		Settings.port = tostring(a[i + 1])
		return 2
	end
	-- Parse arguments.
	while i <= #a do
		if a[i] == "--account" or a[i] == "-a" then
			i = i + 1
			if i <= #a then
				clss.account = a[i]
				i = i + 1
			end
		elseif a[i] == "--admin" or a[i] == "-d" then
			clss.admin = true
			i = i + 1
		elseif a[i] == "--editor" or a[i] == "-E" then
			clss.editor = true
			i = i + 1
			i = i + parse_pattern()
		elseif a[i] == "--file" or a[i] == "-f" then
			i = i + 1
			if i <= #a then
				local f = tonumber(a[i])
				if f then clss.file = f end
				i = i + 1
			end
		elseif a[i] == "--generate" or a[i] == "-g" then
			clss.generate = true
			i = i + 1
		elseif a[i] == "--help" or a[i] == "-h" then
			clss.help = true
			i = i + 1
		elseif a[i] == "--host" or a[i] == "-H" then
			clss.host = true
			clss.client = true
			i = i + 1
			i = i + parse_addr_port()
		elseif a[i] == "--join" or a[i] == "-J" then
			clss.join = true
			clss.client = true
			i = i + 1
			i = i + parse_addr_port()
		elseif a[i] == "--password" or a[i] == "-p" then
			i = i + 1
			if i <= #a then
				clss.password = a[i]
				i = i + 1
			end
		elseif a[i] == "--quit" or a[i] == "-q" then
			clss.quit = true
			i = i + 1
		elseif a[i] == "--server" or a[i] == "-S" then
			clss.server = true
			i = i + 1
			i = i + parse_addr_port()
		else
			clss.help = true
			break
		end
	end
	-- Host by default.
	if not clss.client and not clss.server and not clss.editor then
		clss.client = true
	end
	-- Check for validity.
	if clss.help then return end
	if clss.host and clss.editor then return end
	if clss.join and clss.editor then return end
	if clss.client and clss.server then return end
	if clss.client and clss.editor then return end
	if clss.editor and clss.server then return end
	if clss.admin and not clss.host and not clss.server then return end
	if not clss.host and not clss.server and clss.generate then return end
	return true
end

--- Returns the usage string.
-- @param clss Settings class.
-- @return Usage string.
Settings.usage = function(clss)
	return [[Usage: lipsofsuna [options]

Options:
  -a --account <account>      Name of the player account.
  -d --admin                  Play as an admin on a hosted server.
  -E --editor <pattern>       Edit a map region.
  -f --file <number>          Save file number.
  -g --generate               Generate a new map.
  -h --help                   Show this help message and exit.
  -H --host localhost <port>  Start a server and join it.
  -J --join <server> <port>   Join a remove server.
  -p --password <password>    Password of the player account.
  -q --quit                   Quit immediately after startup.
  -S --server                 Run as a dedicated server.
  -U --unittest               Run unittests.]]
end

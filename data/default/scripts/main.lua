require "system/core"

if Program.args == "-U" or Program.args == "--unittest" then
	Program:launch_mod{name = "unittest", args = Program.args}
else
	Program:launch_mod{name = "godhead", args = Program.args}
end
Program.quit = true

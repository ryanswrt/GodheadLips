for k,v in pairs(File:scan_directory("scripts/content/patterns")) do
	require("content/patterns/" .. string.gsub(v, "([^.]*).*", "%1"))
end
--require "content/patterns/"

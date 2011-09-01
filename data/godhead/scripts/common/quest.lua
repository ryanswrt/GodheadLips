Quest = Class()
Quest.dict_id = {}
Quest.dict_name = {}

--- Creates a new quest specification.
-- @param clss Quest class.
-- @param args Arguments.<ul>
--   <li>name: Quest name. (required)</li>
--   <li>progress: Progress number.</li>
--   <li>status: Quest status. ("unused"/"inactive"/"active"/"completed")</li>
--   <li>text: Textual description of the quest status.</li></ul>
-- @return New quest instance or nil.
Quest.new = function(clss, args)
	local self = Class.new(clss, args)
	if self then
		self.id = #clss.dict_id + 1
		clss.dict_id[self.id] = self
		clss.dict_name[self.name] = self
	end
	self.progress = self.progress or 0
	self.status = self.status or "unused"
	self.text = self.text or ""
	return self
end

--- Finds a quest by name or ID.
-- @param clss Class.
-- @param args Arguments.<ul>
--   <li>name: Quest name.</li>
--   <li>id: Quest ID.</li></ul>
-- @return Quest or nil.
Quest.find = function(clss, args)
	if args.name then
		return clss.dict_name[args.name]
	else
		return clss.dict_id[args.id]
	end
end

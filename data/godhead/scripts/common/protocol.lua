Protocol = Class()
Protocol.types = {}

--- Registers a new packet handler.
-- @param clss Protocol class.
-- @param args Arguments.<ul>
--   <li>type: Message type.</li>
--   <li>func: Handler function.</li></ul>
Protocol.add_handler = function(clss, args)
	-- Find message number.
	local msg = packets[args.type]
	if not msg then return end
	-- Find handler group.
	local t = clss.types[msg]
	if not t then
		t = {}
		clss.types[msg] = t
	end
	-- Allocate handler ID.
	local id = 1
	while t[id] do id = math.random(1, 65535) end
	-- Add to handler group.
	t[id] = args.func
end

Eventhandler{type = "packet", func = function(self, args)
	local t = Protocol.types[args.message]
	if t then
		for k,v in pairs(t) do
			v(args)
		end
	end
end}

packets = {}
do
	local index = 0
	local function add(name)
		packets[name] = index
		index = index + 1
	end
	add("ADMIN_DELETE")
	add("ADMIN_SAVE")
	add("ADMIN_SHUTDOWN")
	add("ADMIN_SPAWN")
	add("ADMIN_PRIVILEGE")
	add("ADMIN_STATS")
	add("EDIT_TILE") -- FIXME: ADMIN_TILE

	add("AUTHENTICATE_REJECT")

	add("BOOK")
	add("CHARACTER_ACCEPT")
	add("CHARACTER_CREATE")
	add("CLIENT_AUTHENTICATE")

	add("CRAFTING")

	add("DIALOG_ANSWER")
	add("DIALOG_CHOICE")
	add("DIALOG_CLOSE")
	add("DIALOG_LINE")

	add("EFFECT")
	add("EFFECT_WORLD")

	add("FEAT")
	add("FEAT_UNLOCK")

	add("GENERATOR_STATUS")

	add("INVENTORY_CREATED")
	add("INVENTORY_CLOSED")
	add("INVENTORY_ITEM_ADDED")
	add("INVENTORY_ITEM_REMOVED")
	add("INVENTORY_WEIGHT")

	add("MARKER_ADD")
	add("MARKER_REMOVE")
	add("MARKER_UPDATE")

	add("MESSAGE")
	add("MESSAGE_NOTIFICATION")

	add("MODIFIER_ADD")
	add("MODIFIER_REMOVE")

	add("MOVE_ITEM") -- FIXME: Ugly

	add("OBJECT_ANIMATED")
	add("OBJECT_BEHEADED")
	add("OBJECT_DEAD")
	add("OBJECT_DIALOG_CHOICE")
	add("OBJECT_DIALOG_MESSAGE")
	add("OBJECT_DIALOG_NONE")
	add("OBJECT_EFFECT")
	add("OBJECT_FEAT")
	add("OBJECT_HIDDEN")
	add("OBJECT_MOVED")
	add("OBJECT_SHOWN")
	add("OBJECT_SKILL")
	add("OBJECT_SLOT")
	add("OBJECT_SPEECH")

	add("QUEST_MARKER")
	add("QUEST_STATUS")

	add("PLAYER_ATTACK")
	add("PLAYER_BLOCK")
	add("PLAYER_CHAT")
	add("PLAYER_CLIMB")
	add("PLAYER_EXAMINE")
	add("PLAYER_JUMP")
	add("PLAYER_MOVE")
	add("PLAYER_RESPAWN")
	add("PLAYER_RUN")
	add("PLAYER_SKILLS")
	add("PLAYER_STRAFE")
	add("PLAYER_TURN")
	add("PLAYER_USE")

	add("TRADING_ACCEPT")
	add("TRADING_ADD_BUY")
	add("TRADING_ADD_SELL")
	add("TRADING_CANCEL")
	add("TRADING_REMOVE_BUY")
	add("TRADING_REMOVE_SELL")
	add("TRADING_START")

	add("VOXEL_DIFF")
end

Protocol.object_flags = {
	SPEEDLINE = 0x1,
	BEHEADED = 0x2}

Protocol.object_show_flags = {
	ANIMS = 0x000100,
	BODY_STYLE = 0x000200,
	HEAD_STYLE = 0x000400,
	NAME = 0x000800,
	POSITION = 0x001000,
	ROTATION = 0x002000,
	SELF = 0x004000,
	SKILLS = 0x008000,
	SLOTS = 0x010000,
	SPEC = 0x020000,
	SPECIES = 0x040000,
	DIALOG = 0x080000}

effects = {}
do
	local index = 0
	local function add(name)
		effects[name] = index
		index = index + 1
	end
	add("DEFAULT")
	add("EXPLOSION")
	add("EVAPORATE")
	add("HEAL")
	add("SQUISH")
end

moveitem = { INVENTORY = 0, EQUIPMENT = 1, WORLD = 2 }

Animation = {}
Animation.CHANNEL_TILT = 127
Animation.CHANNEL_CUSTOMIZE = 128

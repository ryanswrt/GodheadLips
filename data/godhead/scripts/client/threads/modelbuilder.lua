require "system/class"
require "system/core"
require "system/math"
require "system/model"
require "system/model-editing"
require "common/bitwise"
require "common/faction"
require "common/feat"
require "common/model"
require "common/itemspec"
require "common/species"
require "content/armor"
require "content/enemies"
require "content/factions"
require "content/feats"
require "content/items"
require "content/models"
require "content/npcs"
require "content/species"
require "content/weapons"

-- Character meshes are kept in memory to ensure fast builds.
-- TODO: Could also cache morphed models for a short period.
Model.dict_name = {}

local build = function(args)
	local species = Species:find{name = args.species}
	if not species then return end
	-- Get the base meshes.
	local meshes = {skeleton = species.model}
	for k,v in pairs(species.models) do
		meshes[k] = v
	end
	-- Add the hair model.
	if args.hair_style and args.hair_style ~= "" then
		meshes.hair = args.hair_style
	end
	-- Add equipment models.
	if args.equipment then
		for slot,name in pairs(args.equipment) do
			local spec = Itemspec:find{name = name}
			if spec and spec.equipment_models then
				local models = spec:get_equipment_models(species.equipment_class or species.name, lod)
				if models then
					for k,v in pairs(models) do
						meshes[k] = v
					end
				end
			end
		end
	end
	-- Remove worksafety stuff in the eyecandy mode.
	if args.nudity then
		meshes["lower_safe"] = nil
		meshes["upper_safe"] = nil
	end
	-- Remove the top if the character is sufficiently male.
	if args.body_style then
		local male = 0
		if args.body_style[3] and args.body_style[3] < 0.1 then male = male + 5 end -- breast size
		if args.body_style[4] and args.body_style[4] < 0.3 then male = male + 1 end -- hips wide
		if args.body_style[4] and args.body_style[4] > 0.6 then male = male - 5 end -- hips wide
		if args.body_style[6] and args.body_style[6] > 0.3 then male = male + 1 end -- torso wide
		if args.body_style[8] and args.body_style[8] > 0.5 then male = male + 1 end -- waist wide
		if male >= 7 then meshes["upper_safe"] = nil end
	end
	-- Create the skeleton.
	local m = Model:find_or_load{file = meshes.skeleton}
	m = m:copy()
	-- Add other meshes.
	local has_head = not args.beheaded
	local mesh_head = {eyes = true, head = true, hair = true}
	for k,v in pairs(meshes) do
		if k ~= "skeleton" and (has_head or not mesh_head[k]) then
			local tmp
			local ref = Model:find_or_load{file = v}
			-- Face customization.
			if args.face_style and (string.match(k, ".*head.*") or string.match(k, ".*eye.*")) then
				tmp = ref:copy()
				if args.face_style[1] then tmp:morph("cheekbone small", args.face_style[1], ref) end
				if args.face_style[2] then tmp:morph("cheek small", args.face_style[2], ref) end
				if args.face_style[3] then tmp:morph("chin sharp", args.face_style[3], ref) end
				if args.face_style[4] then tmp:morph("chin small", args.face_style[4], ref) end
				if args.face_style[5] then tmp:morph("eye inner", args.face_style[5], ref) end
				if args.face_style[6] then tmp:morph("eye near", args.face_style[6], ref) end
				if args.face_style[7] then tmp:morph("eye outer", args.face_style[7], ref) end
				if args.face_style[8] then tmp:morph("eye small", args.face_style[8], ref) end
				if args.face_style[9] then tmp:morph("face wrinkle", args.face_style[9], ref) end
				if args.face_style[10] then tmp:morph("jaw straight", args.face_style[10], ref) end
				if args.face_style[11] then tmp:morph("jaw wide", args.face_style[11], ref) end
				if args.face_style[12] then tmp:morph("lips protrude", args.face_style[12], ref) end
				if args.face_style[13] then tmp:morph("mouth wide", args.face_style[13], ref) end
				if args.face_style[14] then tmp:morph("nose dull", args.face_style[14], ref) end
				if args.face_style[15] then tmp:morph("nose up", args.face_style[15], ref) end
			end
			-- Body customization.
			if args.body_style then
				if not tmp then tmp = ref:copy() end
				if args.body_style[1] then tmp:morph("arms muscular", args.body_style[1], ref) end
				if args.body_style[2] then tmp:morph("body thin", args.body_style[2], ref) end
				if args.body_style[3] then
					if args.body_style[3] < 0.5 then
						tmp:morph("breast small", 1 - 2 * args.body_style[3], ref)
					elseif args.body_style[3] > 0.5 then
						tmp:morph("breast big", 2 * args.body_style[3] - 1, ref)
					end
				end
				if args.body_style[4] then tmp:morph("hips wide", args.body_style[4], ref) end
				if args.body_style[5] then tmp:morph("legs muscular", args.body_style[5], ref) end
				if args.body_style[6] then tmp:morph("torso wide", args.body_style[6], ref) end
				if args.body_style[7] then tmp:morph("waist fat", args.body_style[7], ref) end
				if args.body_style[8] then tmp:morph("waist wide", args.body_style[8], ref) end
			end
			-- Merge to the character model.
			m:merge(tmp or ref)
		end
	end
	-- Colorize materials.
	m:edit_material{match_shader = "hair", diffuse = args.hair_color}
	m:edit_material{match_shader = "skin", diffuse = args.skin_color,
		shader = species.skin_shader, textures = species.skin_textures}
	m:edit_material{match_shader = "eye", diffuse = args.eye_color}
	-- Recalculate the bounding box.
	m:calculate_bounds()
	return m
end

while not Program.quit do
	-- Get a build order.
	local msg
	while true do
		msg = Program:pop_message()
		if msg then break end
		Program:wait(0.02)
		if Program.quit then return end
	end
	-- Build the model.
	local args = assert(loadstring("return " .. msg.string))()
	local mdl = args and build(args)
	-- Return the model.
	Program:push_message{name = msg.name, model = mdl}
end

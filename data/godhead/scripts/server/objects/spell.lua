require "server/objects/object"

Spell = Class(Object)

--- Creates a new spell.
-- @param clss Spell class.
-- @param args Arguments.<ul>
--   <li>effect: Effect type.</li>
--   <li>feat: Feat invoking the spell.</li>
--   <li>model: Model name.</li>
--   <li>owner: Caster of the spell.</li>
--   <li>position: Position in world space.</li>
--   <li>power: Effect power</li></ul>
-- @return Spell.
Spell.new = function(clss, args)
	local self = Object.new(clss, {effect = args.effect, feat = args.feat, owner = args.owner, power = args.power})
	if args.effect == "dig" then
		-- Create a digger projectile.
		self.gravity = Vector()
		self.model = args.model
		self.physics = "rigid"
		self:fire{collision = true, feat = args.feat, owner = args.owner, timer = 10}
		self.orig_rotation = self.rotation
		self.orig_velocity = self.velocity
		self.power = 1 + 0.1 * args.power
	elseif args.effect == "dragon breath" then
		-- Create a breathe of fire.
		Effect:play{effect = "dragonbreath1", object = args.owner}
		self.time = 3
		self.timer = Timer{delay = 0.5, func = function(t, secs)
			-- Damage creatures in the flame.
			local pos = args.owner.position
			local dir = args.owner.rotation * Vector(0,0,-1)
			local objs = Object:find{point = pos, radius = 10}
			for k,v in pairs(objs) do
				if dir:dot((v.position - pos):normalize()) > 0.8 then
					v:inflict_modifier("burning", 3)
					v:damaged{amount = 5, type = "fire"}
				end
			end
			-- Stop after the time is out.
			self.time = self.time - secs
			if self.time <= 0 then
				t:disable()
			end
		end}
	elseif args.effect == "firewall" then
		-- Create a firewall.
		-- When the feat is performed, a projectile is created here. When the
		-- projectile hits something, we return here and create the flames.
		if not args.position then
			-- Create a projectile.
			self.gravity = Vector()
			self.model = args.model
			self.physics = "rigid"
			self:fire{collision = true, feat = args.feat, owner = args.owner}
		else
			-- Create a flame.
			self.model = "firewall1"
			self.collision_mask = 0
			self.position = args.position
			self.realized = true
			self.timer = Timer{delay = 0.1, func = function(t, secs)
				self.power = self.power - secs
				if self.power > 0 then
					local objs = Object:find{point = self.position + Vector(0,1), radius = 1.3}
					for k,v in pairs(objs) do
						v:inflict_modifier("burning", 3)
					end
				else
					self:detach()
					t:disable()
				end
			end}
		end
	elseif args.effect == "magic missile" then
		-- Create a magic missile.
		self.gravity = Vector()
		self.model = args.model
		self.physics = "rigid"
		self.speed = 3
		self:fire{collision = true, feat = args.feat, owner = args.owner, speed = self.speed}
		self.timer = Timer{delay = 0, func = function(t, secs)
			-- Adjust rotation.
			-- Controlling is done by copying the rotation from the caster.
			self.rotation = self.owner.rotation * self.owner.tilt
			-- Adjust velocity.
			-- Velocity is smoothed but approaches the target value quickly.
			self.velocity = (self.velocity + self.rotation * Vector(0,0,-self.speed)) * 0.5
		end}
	else
		-- Create a normal projectile.
		self.gravity = Vector()
		self.model = args.model
		self.physics = "rigid"
		self:fire{collision = true, feat = args.feat, owner = args.owner}
	end
	return self
end

--- Handles physics contacts.
-- @param self Object.
-- @param result Contact result.
Spell.contact_cb = function(self, result)
	local apply = function()
		self.feat:apply{
			attacker = self.owner,
			point = result.point,
			projectile = self,
			target = result.object,
			tile = result.tile}
	end
	-- Make sure that applicable.
	if not self.realized then return end
	if result.object == self.owner then return end
	-- Type specific effect.
	if self.effect == "dig" then
		-- Dig.
		-- The effect is applied until the dig effect doesn't have power left.
		-- If a non-diggable obstacle is hit, apply() sets the power to zero.
		if not result.tile then
			self.power = 0
		else
			self.power = self.power - 1
		end
		self.rotation = self.orig_rotation
		self.velocity = self.orig_velocity
		apply()
		if self.power < 1 then self:detach() end
	elseif self.effect == "firewall" then
		-- Firewall.
		if not self.burning then
			-- Create the wall.
			-- The projectile has hit something here so we create the actual flames.
			self:detach()
			local p = Utils:find_empty_ground(self.position)
			if not p then return end
			local makewall = function(ctr, dir)
				for i=-2,2 do
					local w = ctr + dir * i * Voxel.tile_size
					local t = (w * Voxel.tile_scale + Vector(0,0.5,0)):floor()
					if Voxel:get_tile(t) == 0 and Voxel:get_tile(t - Vector(0,1)) ~= 0 then
						Spell{burning = true, effect = "firewall", position = w, power = self.power}
					end
				end
			end
			local d = (p - self.owner.position):abs()
			makewall(p, (d.x < d.z) and Vector(1) or Vector(0,0,1))
		elseif result.object then
			-- Inflict the burning modifier until the wall times out.
			-- The actual flames have touched something here.
			result.object:inflict_modifier("burning", 3)
		end
	elseif self.effect == "magic missile" then
		-- Magic missile.
		-- Stop the timer, apply damage and vanish.
		self.timer:disable()
		self.timer = nil
		apply()
		self:detach()
	else
		-- Normal projectile.
		-- Apply to the first obstacle and vanish.
		apply()
		self:detach()
	end
end

--- Saves the object to the database.
-- @param self Object.
Spell.save = function(self)
end

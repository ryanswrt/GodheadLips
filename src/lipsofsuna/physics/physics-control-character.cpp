/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
 *
 * Lips of Suna is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Lips of Suna is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Lips of Suna. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \addtogroup LIPhy Physics
 * @{
 * \addtogroup LIPhyPrivate Private
 * @{
 */

#include "physics-object.h"
#include "physics-private.h"

#define LIPHY_CHARACTER_FLIGHT_FACTOR 0.3f
#define LIPHY_CHARACTER_RISING_LIMIT 5.0f 
#define LIPHY_CHARACTER_GROUND_DAMPING 1.0f
#define LIPHY_CHARACTER_HOVER_DETECTION_TIME 0.2f
#define LIPHY_CHARACTER_HOVER_DETECTION_THRESHOLD 0.5f
#define LIPHY_CHARACTER_HOVER_RESOLUTION_TIME 1.0f

static void private_calculate_control (
	int              ground,
	float            current_speed,
	float            target_speed,
	const btVector3* position,
	const btVector3* direction,
	btVector3*       velocity_result,
	btVector3*       acceleration_result);

/*****************************************************************************/

LIPhyCharacterAction::LIPhyCharacterAction (
	LIPhyObject* object)
{
	this->object = object;
	this->timer = 0.0f;
	this->ground = 0;
	this->hover = 0.0f;
}

void LIPhyCharacterAction::updateAction (
	btCollisionWorld* world, btScalar delta)
{
	int ground;
	float damp0;
	float damp1;
	float speed;
	LIMatAabb bounds;
	btTransform transform;
	btCollisionObject* object = this->object->control->get_object ();

	/* Get the transformation of the bottom of the character. */
	liphy_object_get_bounds_internal (this->object, &bounds);
	transform = object->getWorldTransform ();
	transform = btTransform (btQuaternion::getIdentity (), btVector3 (
		0.5f * (bounds.min.x + bounds.max.x), bounds.min.y,
		0.5f * (bounds.min.z + bounds.max.z))) * transform;

	/* Calculate the movement axes of the character. */
	/* We use the gravity vector as the down axis because the facing direction,
	   includes the tilting component, which would case the character to lift
	   off distrubingly when walking while tilted. */
	btVector3 pos = transform * btVector3 (0.0f, 0.0f, 0.0f);
	btVector3 right = transform * btVector3 (1.0f, 0.0f, 0.0f) - pos;
	btVector3 forward = transform * btVector3 (0.0f, 0.0f, -1.0f) - pos;
	LIMatVector down1 = limat_vector_normalize (this->object->config.gravity);
	btVector3 down = btVector3 (down1.x, down1.y, down1.z);
	if (limat_vector_get_length (down1) < LIMAT_EPSILON)
		down = transform * btVector3 (0.0f, -1.0f, 0.0f) - pos;
	forward = right.cross(down);
	right = down.cross(forward);

	/* Check for ground. */
	this->timer += delta;
	if (this->timer >= 0.05f)
	{
		LIMatVector check0 = { pos[0], pos[1] + 0.1f, pos[2] };
		LIMatVector check1 = { pos[0], pos[1] - 0.6f, pos[2] };
		ground = liphy_physics_cast_ray (this->object->physics, &check0, &check1,
			this->object->config.collision_group, this->object->config.collision_mask,
			&this->object, 1, NULL);
		this->ground = ground;
		this->timer = 0.0f;
	}
	else
		ground = this->ground;

	/* Get velocity components. */
	btVector3 vel = ((btRigidBody*) object)->getLinearVelocity ();
	btVector3 accel = btVector3 (0.0f, 0.0f, 0.0f);
	float dotx = vel.dot (right);
	float doty = vel.dot (-down);
	float dotz = vel.dot (-forward);
	btVector3 velx = dotx * right;
	btVector3 vely = doty * -down;
	btVector3 velz = dotz * -forward;

	/* Hovering hack. */
	/* The body is often large enough for a part of it to be able to get on
	   obstacles while ground check ray cast indicates it being in air. This
	   leads to the character appearing as is hovering. To stop prevent it,
	   we grant ground contact status if we detect hovering. */
	if (!ground && LIMAT_ABS (vely[1]) < LIPHY_CHARACTER_HOVER_DETECTION_THRESHOLD)
		this->hover += delta;
	else
		this->hover = 0.0f;
	if (this->hover >= LIPHY_CHARACTER_HOVER_DETECTION_TIME)
		this->hover = -LIPHY_CHARACTER_HOVER_RESOLUTION_TIME;
	if (this->hover < 0.0f)
		this->ground = 1;

	/* Damp when moving upwards too fast. */
	/* Without this the player would shoot upwards from any slopes. */
	/* FIXME: Doesn't work for non-vertical gravity. */
	damp0 = 1.0f - LIMAT_CLAMP (vel[1], 0.0f, LIPHY_CHARACTER_RISING_LIMIT) /
		LIPHY_CHARACTER_RISING_LIMIT;

	/* Damp when not moving. */
	/* Without this the character would slide a lot after releasing controls. */
	damp1 = (1.0f - LIPHY_CHARACTER_GROUND_DAMPING) * delta;

	/* Walking. */
	speed = this->object->config.movement * this->object->config.speed;
	speed *= 1.0f - this->object->config.friction_liquid * this->object->submerged;
	if (speed != 0.0f && vely[1] > -5.0)
	{
		private_calculate_control (ground, -dotz, speed, &pos, &forward, &velz, &accel);
		velz *= damp0;
	}
	else if (ground)
		velz *= damp1;

	/* Strafing. */
	speed = this->object->config.strafing * this->object->config.speed;
	speed *= 1.0f - this->object->config.friction_liquid * this->object->submerged;
	if (speed != 0.0f && vely[1] > -5.0)
	{
		private_calculate_control (ground, dotx, speed, &pos, &right, &velx, &accel);
		velx *= damp0;
	}
	else if (ground)
		velx *= damp1;

	/* Sum modified component velocities. */
	float mass = this->object->config.mass;
	((btRigidBody*) object)->setLinearVelocity (velx + vely + velz);
	((btRigidBody*) object)->applyImpulse (accel * mass * delta, btVector3 (0.0f, 0.0f, 0.0f));
}

void LIPhyCharacterAction::debugDraw (
	btIDebugDraw* debug)
{
}

LIPhyCharacterControl::LIPhyCharacterControl (LIPhyObject* object, btCollisionShape* shape) :
	LIPhyControlRigid (object, shape), action (object)
{
	object->physics->dynamics->addAction (&this->action);
	this->body.setActivationState (DISABLE_DEACTIVATION);
	this->body.setAngularFactor (0.0f);
}

LIPhyCharacterControl::~LIPhyCharacterControl ()
{
	object->physics->dynamics->removeAction (&this->action);
	this->body.setAngularFactor (1.0f);
}

bool LIPhyCharacterControl::get_ground ()
{
	return this->action.ground;
}

/*****************************************************************************/

static void private_calculate_control (
	int              ground,
	float            current_speed,
	float            target_speed,
	const btVector3* position,
	const btVector3* direction,
	btVector3*       velocity_result,
	btVector3*       acceleration_result)
{
	float accel_factor;
	float speed_factor;

	/* Set velocity. */
	/* Fully acceleration-based movement doesn't allow slope climbing. If the
	   character is on the ground, we force some of the target velocity to aid
	   it at slopes. We need to be careful not to hack the velocity too much or
	   else the character will be able to push heavy objects effortlessly. */
	if (ground)
	{
		speed_factor = limat_smoothstep (current_speed, 0.0f, 7.0f);
		speed_factor = limat_mix (0.5f, 0.3f, speed_factor);
		if ((target_speed > 0.0f && current_speed < speed_factor * target_speed) ||
			(target_speed < 0.0f && current_speed > speed_factor * target_speed))
			*velocity_result = *direction * target_speed * speed_factor;
	}

	/* Set acceleration. */
	/* Acceleration is only set when the character isn't already moving faster
	   than the target speed. The acceleration speed is chosen so that the
	   character reaches the full speed quickly but isn't able to push heavy
	   objects too easily. */
	if ((target_speed < 0.0f || current_speed < target_speed) &&
	    (target_speed > 0.0f || current_speed > target_speed))
	{
		accel_factor = limat_smoothstep (target_speed, 1.0f, 7.0f);
		accel_factor = limat_mix (6.0f, 2.0f, accel_factor);
		*acceleration_result += *direction * target_speed * accel_factor;
	}
}

/** @} */
/** @} */

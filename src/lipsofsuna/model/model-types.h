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

#ifndef __MODEL_TYPES_H__
#define __MODEL_TYPES_H__

#include <lipsofsuna/math.h>

enum
{
	LIMDL_LIGHT_FLAG_SHADOW = 0x01
};

enum _LIMdlNodeType
{
	LIMDL_NODE_BONE,
	LIMDL_NODE_EMPTY,
	LIMDL_NODE_LIGHT,
};

typedef uint16_t LIMdlIndex;
typedef enum _LIMdlNodeType LIMdlNodeType;
typedef struct _LIMdlBone LIMdlBone;
typedef struct _LIMdlHair LIMdlHair;
typedef struct _LIMdlHairNode LIMdlHairNode;
typedef struct _LIMdlHairs LIMdlHairs;
typedef struct _LIMdlLight LIMdlLight;
typedef struct _LIMdlModel LIMdlModel;
typedef struct _LIMdlNode LIMdlNode;
typedef void (*LIMdlNodeTransformFunc)(void*, LIMdlNode*, LIMatTransform*);
typedef struct _LIMdlParticle LIMdlParticle;
typedef struct _LIMdlParticleFrame LIMdlParticleFrame;
typedef struct _LIMdlParticleSystem LIMdlParticleSystem;
typedef struct _LIMdlPose LIMdlPose;
typedef struct _LIMdlPoseChannel LIMdlPoseChannel;
typedef int LIMdlPoseChannelState;
typedef struct _LIMdlPoseFade LIMdlPoseFade;
typedef struct _LIMdlWeightGroup LIMdlWeightGroup;

#endif

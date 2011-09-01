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

#ifndef __ENGINE_MODEL_H__
#define __ENGINE_MODEL_H__

#include <lipsofsuna/model.h>
#include <lipsofsuna/script.h>
#include "engine.h"
#include "engine-types.h"

struct _LIEngModel
{
	int id;
	LIEngEngine* engine;
	LIMdlModel* model;
	LIScrData* script;
};

LIAPICALL (LIEngModel*, lieng_model_new, (
	LIEngEngine* engine));

LIAPICALL (LIEngModel*, lieng_model_new_copy, (
	LIEngModel* model));

LIAPICALL (LIEngModel*, lieng_model_new_model, (
	LIEngEngine* engine,
	LIMdlModel*  model));

LIAPICALL (void, lieng_model_free, (
	LIEngModel* self));

LIAPICALL (void, lieng_model_calculate_bounds, (
	LIEngModel* self));

LIAPICALL (void, lieng_model_changed, (
	LIEngModel* self));

LIAPICALL (int, lieng_model_load, (
	LIEngModel* self,
	const char* name,
	int         mesh));

#endif

/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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
 * \addtogroup LILodTool LodTool
 * @{
 */

#include "lipsofsuna/model.h"
#include "lipsofsuna/system.h"

int main (int argc, char** argv)
{
	int i;
	LIMdlBuilder* builder;
	LIMdlModel* model;

	if (!argc || !strcmp (argv[1], "--help") || !strcmp (argv[1], "-h"))
	{
		printf ("Usage: %s [lmdl...]\n", argv[0]);
		return 0;
	}

	for (i = 1 ; i < argc ; i++)
	{
		/* Load the model. */
		model = limdl_model_new_from_file (argv[i], 1);
		if (model == NULL)
		{
			lisys_error_report ();
			continue;
		}

		/* Check for existing LOD. */
		if (!model->lod.array[0].indices.count)
		{
			printf ("      Unneeded %s\n", argv[i]);
			continue;
		}
		if (model->lod.count > 1)
		{
			printf ("%3d%%: Existing %s\n", 100 - 100 *
				model->lod.array[model->lod.count - 1].indices.count /
				model->lod.array[0].indices.count, argv[i]);
			limdl_model_free (model);
			continue;
		}

		/* Build the detail levels. */
		builder = limdl_builder_new (model);
		if (builder == NULL)
		{
			lisys_error_report ();
			limdl_model_free (model);
			continue;
		}
		limdl_builder_calculate_lod (builder, 5, 0.05f);
		limdl_builder_finish (builder);
		limdl_builder_free (builder);

		/* Save the modified model. */
		printf ("%3d%%: Built    %s\n", 100 - 100 *
			model->lod.array[model->lod.count - 1].indices.count /
			model->lod.array[0].indices.count, argv[i]);
		limdl_model_write_file (model, argv[i]);
		limdl_model_free (model);
	}

	return 0;
}

/** @} */

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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtPassword Password
 * @{
 */

#include "sha1.h"
#include "pbkdf2.h"
#include "ext-module.h"

static void Program_hash_password (LIScrArgs* args)
{
	int i;
	int j;
	uint8_t key[SHA1_DIGEST_LENGTH];
	char hex[2 * SHA1_DIGEST_LENGTH + 1];
	int rounds = 5000;
	const char* password = "";
	const char* salt = "";
	const char* const hexchars = "0123456789abcdef";

	/* Get the password. */
	if (!liscr_args_geti_string (args, 0, &password))
		return;
	if (!liscr_args_geti_string (args, 1, &salt))
		return;
	liscr_args_geti_int (args, 2, &rounds);

	/* Calculate the password hash. */
	memset (key, 0, SHA1_DIGEST_LENGTH);
	if (pkcs5_pbkdf2 (password, strlen (password), salt, strlen (salt), key, SHA1_DIGEST_LENGTH, rounds) != 0)
		return;

	/* Convert to hexadecimal and cut to 128-bit. */
	for (i = j = 0 ; i < 16 ; i++)
	{
		hex[j++] = hexchars[(int)(key[i] >> 4)];
		hex[j++] = hexchars[(int)(key[i] & 0xF)];
	}
	hex[j] = '\0';
	liscr_args_seti_string (args, hex);
}

static void Program_random_salt (LIScrArgs* args)
{
	int i;
	int length = 32;
	char* string;
	LIAlgRandom random;

	if (liscr_args_geti_int (args, 0, &length))
		length = LIMAT_MAX (length, 1);

	string = lisys_calloc (length + 1, sizeof (char));
	if (string == NULL)
		return;
	lialg_random_init (&random, lisys_time (NULL));
	for (i = 0 ; i < length ; i++)
		string[i] = lialg_random_range (&random, (int) '!', (int) '~');
	string[i] = '\0';
	liscr_args_seti_string (args, string);
	lisys_free (string);
}

/*****************************************************************************/

void liext_script_password (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_hash_password", Program_hash_password);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_random_salt", Program_random_salt);
}

/** @} */
/** @} */

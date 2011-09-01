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
 * \addtogroup LISys System
 * @{
 * \addtogroup LISysUser User
 * @{
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include "system-error.h"
#include "system-user.h"
#ifdef HAVE_GRP_H
#include <grp.h>
#endif
#ifdef HAVE_PWD_H
#include <pwd.h>
#endif

/**
 * \brief Gets the name of a user.
 *
 * \param uid User id.
 * \return User name or NULL.
 */
char*
lisys_user_get_name (int uid)
{
#ifdef HAVE_PWD_H
	struct passwd* pwd;

	pwd = getpwuid (uid);
	if (pwd == NULL)
	{
		lisys_error_set (errno, NULL);
		return NULL;
	}
	ret = strdup (pwd->pw_name);
	if (ret == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}

	return ret;
#else
	lisys_error_set (ENOTSUP, NULL);
	return NULL;
#endif
}

/**
 * \brief Gets the name of a group.
 *
 * \param gid Group id.
 * \return Group name or NULL.
 */
char*
lisys_group_get_name (int gid)
{
#ifdef HAVE_GRP_H
	struct group* grp;

	grp = getgrgid (gid);
	if (grp == NULL)
	{
		lisys_error_set (errno, NULL);
		return NULL;
	}
	ret = strdup (grp->gr_name);
	if (ret == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}

	return ret;
#else
	lisys_error_set (ENOTSUP, NULL);
	return NULL;
#endif
}

/** @} */
/** @} */

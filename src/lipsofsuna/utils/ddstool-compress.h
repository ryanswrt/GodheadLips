/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifdef __cplusplus
extern "C" {
#endif

void
liimg_compress_compress (const void* pixels,
                         int         width,
                         int         height,
                         int         type,
                         void*       result);

void
liimg_compress_uncompress (const void* pixels,
                           int         width,
                           int         height,
                           int         type,
                           void*       result);

int
liimg_compress_storage (int width,
                        int height,
                        int type);

#ifdef __cplusplus
}
#endif


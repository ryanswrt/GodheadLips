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

#ifndef __SYSTEM_COMPILER_H__
#define __SYSTEM_COMPILER_H__

#ifdef WIN32
 #ifdef __cplusplus
  #define LIAPIEXPORT(ret, name, args) extern "C" __declspec(dllexport) ret name args
  #define LIAPIIMPORT(ret, name, args) extern "C" __declspec(dllimport) ret name args
 #else
  #define LIAPIEXPORT(ret, name, args) __declspec(dllexport) ret name args
  #define LIAPIIMPORT(ret, name, args) __declspec(dllimport) ret name args
 #endif
#else
 #ifdef __cplusplus
  #define LIAPIEXPORT(ret, name, args) extern "C" ret name args
  #define LIAPIIMPORT(ret, name, args) extern "C" ret name args
 #else
  #define LIAPIEXPORT(ret, name, args) ret name args
  #define LIAPIIMPORT(ret, name, args) ret name args
 #endif
#endif
#ifdef DLL_EXPORT
 #define LIAPICALL(ret, name, args) LIAPIIMPORT(ret, name, args)
#else
 #define LIAPICALL(ret, name, args) LIAPIEXPORT(ret, name, args)
#endif

#if __GNUC__ >= 4
#define LISYS_ATTR_CONST __attribute__((const))
#define LISYS_ATTR_FORMAT(i, j) __attribute__((format (printf, i, j)))
#define LISYS_ATTR_NORETURN __attribute__((noreturn))
#define LISYS_ATTR_SENTINEL __attribute__((sentinel))
#else
#define LISYS_ATTR_CONST
#define LISYS_ATTR_FORMAT(i, j)
#define LISYS_ATTR_NORETURN
#define LISYS_ATTR_SENTINEL
#endif

#endif


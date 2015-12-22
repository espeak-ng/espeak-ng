/*
 * Copyright (C) 2007 to 2009 by Jonathan Duddington
 * email: jonsd@users.sourceforge.net
 * Copyright (C) 2015 Reece H. Dunn
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see: <http://www.gnu.org/licenses/>.
 */

#ifndef DEBUG_H
#define DEBUG_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef DEBUG_ENABLED
#define ENTER(text) debug_enter(text)
#define SHOW(format, ...) debug_show(format, __VA_ARGS__);
#define SHOW_TIME(text) debug_time(text);
extern void debug_enter(const char *text);
extern void debug_show(const char *format, ...);
extern void debug_time(const char *text);

#else

#ifdef NO_VARIADIC_MACROS
#define SHOW(format)   // VC6 doesn't allow "..."
#else
#define SHOW(format, ...)
#endif
#define SHOW_TIME(text)
#define ENTER(text)
#endif

#ifdef __cplusplus
}
#endif

#endif

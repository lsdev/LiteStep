// Hello Example
// Copyright (C) 2006 Litestep Development Team
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#if !defined(TRACE_H)
#define TRACE_H

#include <cassert>

#if defined(_DEBUG)
#define ASSERT(x) assert(x)
#else
#define ASSERT(x) ((void) 0)
#endif

#if defined(_DEBUG)
#define TRACE0(a)                      Trace((a))
#define TRACE1(a, b)                   Trace((a), (b))
#define TRACE2(a, b, c)                Trace((a), (b), (c))
#define TRACE3(a, b, c, d)             Trace((a), (b), (c), (d))
#define TRACE4(a, b, c, d, e)          Trace((a), (b), (c), (d), (e))
#define TRACE5(a, b, c, d, e, f)       Trace((a), (b), (c), (d), (e), (f))
#define TRACE6(a, b, c, d, e, f, g)    Trace((a), (b), (c), (d), (e), (f), (g))
#define TRACE7(a, b, c, d, e, f, g, h) Trace((a), (b), (c), (d), (e), (f), (g), (h))
#else
#define TRACE0(a)                      ((void) 0)
#define TRACE1(a, b)                   ((void) 0)
#define TRACE2(a, b, c)                ((void) 0)
#define TRACE3(a, b, c, d)             ((void) 0)
#define TRACE4(a, b, c, d, e)          ((void) 0)
#define TRACE5(a, b, c, d, e, f)       ((void) 0)
#define TRACE6(a, b, c, d, e, f, g)    ((void) 0)
#define TRACE7(a, b, c, d, e, f, g, h) ((void) 0)
#endif

// Output a formatted trace message
void Trace(const char *format, ...);

#endif

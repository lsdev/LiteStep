// Clock Example
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

#include <stdarg.h>
#include <stdio.h>
#include <windows.h>

#define TRACE_MESSAGE_LEN 1024

void Trace(const char *format, ...)
{
    char message[TRACE_MESSAGE_LEN];
    va_list vaList;
    
    // Format message
    va_start(vaList, format);
    _vsnprintf(message, TRACE_MESSAGE_LEN, format, vaList);
    va_end(vaList);
    
    // Output it
    OutputDebugString(message);
}

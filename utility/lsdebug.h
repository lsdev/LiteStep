/*
This is a part of the LiteStep Shell Source code.

Copyright (C) 1997-2002 The LiteStep Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/ 
/****************************************************************************
****************************************************************************/
#ifndef __LSDEBUG_H
#define __LSDEBUG_H

#ifdef __cplusplus

#include "common.h"
#include <stdio.h>

#ifdef _LSDEBUG
#ifndef _LSDEBUGBUFFER
static char debugBuffer[4096];
#define _LSDEBUGBUFFER
#endif // _LSDEBUGBUFFER

// Compiler warning Macros
#define chSTR(x) #x
#define chSTR2(x) chSTR(x)
#define COMPILE_WARN(desc) message(__FILE__ "(" chSTR2(__LINE__) ":") #desc)

// _LSDEBUG0
static void lssprintf(LPSTR buffer, LPCSTR file, int line)
{
	sprintf(buffer, "%s:%i", file, line);
}

// _LSDEBUG1
static void lssprintf(LPSTR buffer, LPCSTR file, int line, int msg)
{
	sprintf(buffer, "%s:%i %i", file, line, msg);
}

static void lssprintf(LPSTR buffer, LPCSTR file, int line, LPCSTR msg)
{
	sprintf(buffer, "%s:%i %s", file, line, msg);
}

static void lssprintf(LPSTR buffer, LPCSTR file, int line, long msg)
{
	sprintf(buffer, "%s:%i %li", file, line, msg);
}

// _LSDEBUG2
static void lssprintf(LPSTR buffer, LPCSTR file, int line, LPCSTR text, int msg)
{
	sprintf(buffer, "%s:%i %s = %i", file, line, text, msg);
}

static void lssprintf(LPSTR buffer, LPCSTR file, int line, LPCSTR text, LPCSTR msg)
{
	sprintf(buffer, "%s:%i %s = %s", file, line, text, msg);
}

static void lssprintf(LPSTR buffer, LPCSTR file, int line, LPCSTR text, long msg)
{
	sprintf(buffer, "%s:%i %s = %li", file, line, text, msg);
}

static void lssprintf(LPSTR buffer, LPCSTR file, int line, int text, LPCSTR msg)
{
	sprintf(buffer, "%s:%i %i = %s", file, line, text, msg);
}

// _LSDEBUGPRINTMSG
static void lssprintfmsg(LPSTR buffer, LPCSTR file, int line, LPCSTR text, int msg)
{
	sprintf(buffer, "%s:%i %s = %X", file, line, text, msg);
}

#define _LSDEBUG0 lssprintf(debugBuffer, __FILE__, __LINE__); \
  OutputDebugString(debugBuffer);

#define _LSDEBUG1(msg) lssprintf(debugBuffer, __FILE__, __LINE__, msg); \
  OutputDebugString(debugBuffer);

#define _LSDEBUG2(text, msg) lssprintf(debugBuffer, __FILE__, __LINE__, text, msg); \
  OutputDebugString(debugBuffer);

#define _LSDEBUGPRINTMSG(text, msg) lssprintfmsg(debugBuffer, __FILE__, __LINE__, text, msg); \
  OutputDebugString(debugBuffer);

// LiteStep Debug Function Start
#define _LSDEBUGFUNS(msg) msg { char szLSDebugMsg[] = #msg; _LSDEBUG2("Entering", szLSDebugMsg) 
// LiteStep Debug Function End
#define _LSDEBUGFUNE _LSDEBUG2("Exiting", szLSDebugMsg) }

//
static void lsgetlasterror()
{
	LPVOID lpMsgBuf;
	FormatMessage(
	    FORMAT_MESSAGE_ALLOCATE_BUFFER |
	    FORMAT_MESSAGE_FROM_SYSTEM |
	    FORMAT_MESSAGE_IGNORE_INSERTS,
	    NULL,
	    GetLastError(),
	    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),   // Default language
	    (LPSTR) & lpMsgBuf,
	    0,
	    NULL
	);
	_LSDEBUG2("Last Error", (LPCSTR)lpMsgBuf)
	// Free the buffer.
	LocalFree(lpMsgBuf);
}
#define _LSDEBUGLASTERR lsgetlasterror();

static void lsgetlasterror(DWORD error)
{
	LPVOID lpMsgBuf;
	FormatMessage(
	    FORMAT_MESSAGE_ALLOCATE_BUFFER |
	    FORMAT_MESSAGE_FROM_SYSTEM |
	    FORMAT_MESSAGE_IGNORE_INSERTS,
	    NULL,
	    error,
	    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),   // Default language
	    (LPSTR) & lpMsgBuf,
	    0,
	    NULL
	);
	_LSDEBUG2("Last Error", (LPCSTR)lpMsgBuf)
	// Free the buffer.
	LocalFree(lpMsgBuf);
}
#define _LSDEBUGLASTERR2(error) lsgetlasterror(error);

#else // !_LSDEBBUG

#define _LSDEBUG0
#define _LSDEBUG1(msg)
#define _LSDEBUG2(text, msg)
#define _LSDEBUGPRINTMSG(text, msg)

#define _LSDEBUGFUNS(msg) msg {
#define _LSDEBUGFUNE }

#define _LSDEBUGLASTERR
#define _LSDEBUGLASTERR2

#endif // _LSDEBUG

#endif // _cplusplus

#endif // __LSDEBUG_H

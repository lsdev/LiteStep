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
#ifndef __LSAPIDEFINES_H
#define __LSAPIDEFINES_H

/*
* IMPORTANT:
*   The exports and definitions included
*   with LSAPI_PRIVATE are for INTERNAL USE ONLY!
*
*   They MUST NOT be used by any module.
*   Their names and/or functionality may be changed at ANY time!
*
*/

#include <windows.h>

//---------------------------------------------------------
// MATCH.CPP DEFINES
//---------------------------------------------------------
#define MATCH_PATTERN  6    /* bad pattern */
#define MATCH_LITERAL  5    /* match failure on literal match */
#define MATCH_RANGE    4    /* match failure on [..] construct */
#define MATCH_ABORT    3    /* premature end of text string */
#define MATCH_END      2    /* premature end of pattern string */
#define MATCH_VALID    1    /* valid match */

/* pattern defines */
#define PATTERN_VALID  0    /* valid pattern */
#define PATTERN_ESC   -1    /* literal escape at end of pattern */
#define PATTERN_RANGE -2    /* malformed range in [..] construct */
#define PATTERN_CLOSE -3    /* no end bracket in [..] construct */
#define PATTERN_EMPTY -4    /* [..] contstruct is empty */


//---------------------------------------------------------
// MESSAGE DEFINES
//---------------------------------------------------------
#define LM_SHUTDOWN					8889
#define LM_REPAINT					8890
#define LM_BRINGTOFRONT				8891
#define LM_SAVEDATA					8892
#define LM_RESTOREDATA				8893
#define LM_POPUP					9182
#define LM_HIDEPOPUP				9183
#define LM_FIRSTDESKTOPPAINT		9184 // Deprecated
#define LM_LSSELECT					9185
#define LM_SETTASKBARONTOP			9186 // Deprecated
#define LM_SAVESYSTRAY				9210 // Deprecated
#define LM_RESTORESYSTRAY			9211 // Deprecated
#define LM_CHECKFORAPPBAR			9212 // Deprecated
#define LM_SENDSYSTRAY				9213
#define LM_SYSTRAY					9214
#define LM_SYSTRAYREADY				9215

// Shell Hook Messages (obsolete!)
#define LM_SHELLMESSAGE				9219
#define LM_ACCESSSTATE				9220
#define LM_ACTIVESHELLWIN			LM_ACCESSSTATE + 1
#define LM_APPCOMMANDX				LM_ACCESSSTATE + 2
#define LM_MINMAXWIN				LM_ACCESSSTATE + 3
#define LM_LANGUAGEX				LM_ACCESSSTATE + 4
#define LM_REDRAWX					LM_ACCESSSTATE + 5
#define LM_TASKMANX					LM_ACCESSSTATE + 6
#define LM_ACTIVEWIN				LM_ACCESSSTATE + 7
#define LM_ADDWINDOW				LM_ACCESSSTATE + 8
#define LM_REMOVEWINDOW				LM_ACCESSSTATE + 9 //9229

#define LM_RECYCLE					9260
#define LM_REGISTERMESSAGE			9263
#define LM_UNREGISTERMESSAGE		9264
#define LM_GETREVID					9265
#define LM_UNLOADMODULE				9266
#define LM_RELOADMODULE				9267
#define LM_REGISTERHOOKMESSAGE		9268
#define LM_UNREGISTERHOOKMESSAGE	9269
#define LM_SHADETOGGLE				9300
#define LM_REFRESH					9305

// Threaded Module Messages
#ifdef LSAPI_PRIVATE
#define LM_THREAD_BANGCOMMAND		9310
#define LM_THREADREADY				9311
#define LM_THREADFINISHED			9312
#endif

// VWM Messages
#define LM_VWMUP					9350
#define LM_VWMDOWN					9351
#define LM_VWMLEFT					9352
#define LM_VWMRIGHT					9353
#define LM_VWMNAV					9354
#define LM_SWITCHTON				9355
#define LM_ISSTICKY					9356
#define LM_STICK					9357
#define LM_UNSTICK					9358
#define LM_LISTDESKTOPS				9359
#define LM_DESKTOPINFO				9360
#define LM_GETDESKTOPOF				9361

// Core Messages
#ifdef LSAPI_PRIVATE
#define LM_GETLSOBJECT				9400 // Deprecated
#define LM_WINDOWLIST				9401 // Deprecated
#define LM_DATASTORE				9410 // Deprecated
#define LM_MESSAGEMANAGER			9411 // Deprecated
#define LM_BANGCOMMAND				9420
#define LM_ENUMREVIDS               9430
#define LM_ENUMMODULES              9431
#endif

// shell hook messages
#define LM_WINDOWCREATED			9501
#define LM_WINDOWDESTROYED			9502
#define LM_ACTIVATESHELLWINDOW		9503
#define LM_WINDOWACTIVATED			9504
#define LM_GETMINRECT				9505
#define LM_REDRAW					9506
#define LM_TASKMAN					9507
#define LM_LANGUAGE					9508
#define LM_ACCESSIBILITYSTATE		9511
#define LM_APPCOMMAND				9512

//---------------------------------------------------------
// CONSTANT DEFINES
//---------------------------------------------------------
#define MAX_BANGCOMMAND				64
#define MAX_BANGARGS				256
#define MAX_RCCOMMAND				64
#define MAX_LINE_LENGTH				4096
#define MAX_PATH_LENGTH				1024

#define WHITESPACEW					L" \t\n\r"
#define WHITESPACEA					 " \t\n\r"

#define RESERVEDCHARSW				L"\"\'[];$"
#define RESERVEDCHARSA				 "\"\'[];$"

#ifdef _UNICODE
#define WHITESPACE WHITESPACEW
#define RESERVEDCHARS RESERVEDCHARSW
#else
#define WHITESPACE WHITESPACEA
#define RESERVEDCHARS RESERVEDCHARSA
#endif

#ifdef LSAPI_PRIVATE
#define HIDEmagicDWord 0x59474541
#endif

#ifndef magicDWord
#define magicDWord 0x49474541
#endif // magicDWord

//---------------------------------------------------------
// BANG COMMAND DEFINES
//---------------------------------------------------------
typedef void (__cdecl* BangCommand)(HWND hSender, LPCSTR pszArgs);
typedef void (__cdecl* BangCommandEx)(HWND hSender, LPCSTR pszCommand, LPCSTR pszArgs);

typedef struct _LMBANGCOMMANDA
{
	UINT cbSize;
	HWND hWnd;
	CHAR szCommand[MAX_BANGCOMMAND];
	CHAR szArgs[MAX_BANGARGS];
}
LMBANGCOMMANDA, *PLMBANGCOMMANDA;

typedef struct _LMBANGCOMMANDW
{
	UINT cbSize;
	HWND hWnd;
	WCHAR wzCommand[MAX_BANGCOMMAND];
	WCHAR wzArgs[MAX_BANGARGS];
}
LMBANGCOMMANDW, *PLMBANGCOMMANDW;

#ifdef _UNICODE
#define LMBANGCOMMAND    LMBANGCOMMANDW
#define PLMBANGCOMMAND   PLMBANGCOMMANDW
#else // _UNICODE
#define LMBANGCOMMAND    LMBANGCOMMANDA
#define PLMBANGCOMMAND   PLMBANGCOMMANDA
#endif // _UNICODE

//---------------------------------------------------------
// VWM DEFINES
//---------------------------------------------------------
typedef struct LSDESKTOPINFO
{
	int size;
	char name[32];
	HICON icon;
	BOOL isCurrent;
	int number;
}
LSDESKTOPINFO;


//---------------------------------------------------------
// LM_RECYCLE DEFINES
//---------------------------------------------------------
#define LR_RECYCLE		0
#define LR_LOGOFF		1
#define	LR_QUIT			2
#define LR_MSSHUTDOWN	3


//---------------------------------------------------------
// LM_UN/RELOADMODULE DEFINES
//---------------------------------------------------------
#define LMM_HINSTANCE 0x1000


//---------------------------------------------------------
// LSLOG LOG LEVEL CONSTANTS
//---------------------------------------------------------
#define LOG_ERROR 1
#define LOG_WARNING 2
#define LOG_NOTICE 3
#define LOG_DEBUG 4


//---------------------------------------------------------
// ENUMLSDATA DEFINES
//---------------------------------------------------------
#define ELD_BANGS   1
#define ELD_MODULES 2
#define ELD_REVIDS  3

// ELD_MODULES: possible dwFlags values
#define LS_MODULE_THREADED  0x0001

typedef BOOL (__stdcall* LSENUMBANGSPROC)(LPCSTR, LPARAM);
typedef BOOL (__stdcall* LSENUMREVIDSPROC)(LPCSTR, LPARAM);
typedef BOOL (__stdcall* LSENUMMODULESPROC)(LPCSTR, DWORD, LPARAM);


#endif // __LSAPIDEFINES_H

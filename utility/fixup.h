//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the LiteStep Shell source code.
//
// Copyright (C) 1997-2012  LiteStep Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This file contains workarounds for different compilers and PSDK versions
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#if !defined(FIXUP_H)
#define FIXUP_H

#include "debug.hpp"

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Configuration
//
#if defined(__GNUC__)
#  define FIXUP_MINGW
#endif

#if defined(_MSC_VER)
// Generic MSVC fixup for all PSDK versions
#  define FIXUP_MSVC
#endif

#if !defined(OS_NT)
#  define FIXUP_OLD_HEADERS
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Missing constants and typedefs
//
#if defined(FIXUP_MINGW)

enum RESTRICTIONS
{
    REST_NOLOCALMACHINERUN     = 0x40000046,
    REST_NOCURRENTUSERRUN      = 0x40000047,
    REST_NOLOCALMACHINERUNONCE = 0x40000048,
    REST_NOCURRENTUSERRUNONCE  = 0x40000049
};

typedef struct _SHELLHOOKINFO
{
    HWND hwnd;
    RECT rc;
} SHELLHOOKINFO, *PSHELLHOOKINFO, FAR *LPSHELLHOOKINFO;

#endif // FIXUP_MINGW


#if defined(FIXUP_OLD_HEADERS)

// IsOS constants
#  define OS_WINDOWS                 0
#  define OS_NT                      1
#  define OS_WIN2000ORGREATER        7
#  define OS_XPORGREATER            18
#  define OS_WOW6432                30

// Misc constants
#  define ABM_SETSTATE              0x0000000a
#  define HSHELL_HIGHBIT            0x8000
#  define SETWALLPAPER_DEFAULT      ((LPWSTR)-1)

#  define REGSTR_PATH_RUN_POLICY    REGSTR_PATH_POLICIES _T("\\Explorer\\Run")

#  if !defined(SM_SERVERR2)
#    define SM_SERVERR2     89
#  endif

// Shell types
typedef LPITEMIDLIST                PIDLIST_ABSOLUTE;
typedef LPCITEMIDLIST               PCIDLIST_ABSOLUTE;

typedef GUID                        KNOWNFOLDERID;
typedef const KNOWNFOLDERID&        REFKNOWNFOLDERID;

// Known Folder IDs
const KNOWNFOLDERID FOLDERID_QuickLaunch =
{0x52a4f021, 0x7b75, 0x48a9, {0x9f, 0x6b, 0x4b, 0x87, 0xa2, 0x10, 0xbc, 0x8f}};

// MMSystem.h
#define SND_SYSTEM 0x00200000L

#endif // FIXUP_OLD_HEADERS


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Miscellaneous
//
#if defined(FIXUP_MINGW)

// The MinGW version doesn't work with const references
#undef  UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(P) (void)(P)

#endif // FIXUP_MINGW


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// DynamicFunction
//
#if defined(FIXUP_OLD_HEADERS)

template <class Function>
class DynamicFunction
{
    Function m_pFunction;
    
public:
    DynamicFunction(LPCSTR pszDll, LPCSTR pszExport)
    {
        m_pFunction = \
            (Function)GetProcAddress(GetModuleHandle(pszDll), pszExport);
        
        ASSERT(m_pFunction != NULL);
    }
    
    operator Function() const
    {
        return m_pFunction;
    }
};

// The idea of these DEFINE_DYNFUNC macros is to reduce fixing missing
// functions to two lines: a typedef and a DEFINE_DYNFUNC
#define DEFINE_DYNFUNC(name, dll)               const DynamicFunction<name##_t> name(dll, #name)
#define DEFINE_DYNFUNC_ORD(name, dll, ordinal)  const DynamicFunction<name##_t> name(dll, (LPCSTR)ordinal)
#define DEFINE_DYNFUNC_STR(name, dll, export)   const DynamicFunction<name##_t> name(dll, export)

#endif // FIXUP_OLD_HEADERS


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Missing functions
//
#if defined(FIXUP_OLD_HEADERS)

typedef LPVOID  (WINAPI* SHLockShared_t)(HANDLE hData, DWORD dwProcessId);
typedef BOOL    (WINAPI* SHUnlockShared_t)(LPVOID lpData);
typedef BOOL    (WINAPI* IsOS_t)(DWORD);


DEFINE_DYNFUNC_ORD(SHLockShared,        "shlwapi.dll", 8);
DEFINE_DYNFUNC_ORD(SHUnlockShared,      "shlwapi.dll", 9);
DEFINE_DYNFUNC_ORD(IsOS,                "shlwapi.dll", 437);

#endif // FIXUP_OLD_HEADERS


#if defined(FIXUP_MINGW)

typedef BOOL             (WINAPI* SHFindFiles_t)(PCIDLIST_ABSOLUTE pidlFolder, PCIDLIST_ABSOLUTE pidlSaveFile);
typedef PIDLIST_ABSOLUTE (WINAPI* ILCreateFromPath_t)(LPCSTR pszPath);

DEFINE_DYNFUNC    (SHFindFiles,         "shell32.dll");
DEFINE_DYNFUNC_STR(ILCreateFromPath,    "shell32.dll", "ILCreateFromPathA");

#endif // FIXUP_MINGW


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Fix {Get,Set}{Window,Class}LongPtr warnings
//
// The #defines in the original headers will always produce warnings, even
// if the functions are used correctly. The only way to avoid that is to
// redefine the functions for 32-Bit code.
//
#if defined(FIXUP_MSVC) && !defined(_WIN64)
#undef GetWindowLongPtrA
#undef GetWindowLongPtrW
#undef SetWindowLongPtrA
#undef SetWindowLongPtrW
#undef SetClassLongPtrA
#undef SetClassLongPtrW

inline LONG_PTR WINAPI GetWindowLongPtrA(HWND hWnd, int nIndex)
{
    return GetWindowLongA(hWnd, nIndex);
}

inline LONG_PTR WINAPI GetWindowLongPtrW(HWND hWnd, int nIndex)
{
    return GetWindowLongW(hWnd, nIndex);
}

inline LONG_PTR WINAPI SetWindowLongPtrA(HWND hWnd, int nIndex, LONG_PTR dwNewLong)
{
    return SetWindowLongA(hWnd, nIndex, static_cast<LONG>(dwNewLong));
}

inline LONG_PTR WINAPI SetWindowLongPtrW(HWND hWnd, int nIndex, LONG_PTR dwNewLong)
{
    return SetWindowLongW(hWnd, nIndex, static_cast<LONG>(dwNewLong));
}

inline ULONG_PTR WINAPI SetClassLongPtrA(HWND hWnd, int nIndex, LONG_PTR dwNewLong)
{
    return SetClassLongA(hWnd, nIndex, static_cast<LONG>(dwNewLong));
}

inline ULONG_PTR WINAPI SetClassLongPtrW(HWND hWnd, int nIndex, LONG_PTR dwNewLong)
{
    return SetClassLongW(hWnd, nIndex, static_cast<LONG>(dwNewLong));
}
#endif // defined(_MSC_VER) && !defined(_WIN64)


#endif // FIXUP_H

#ifndef LSSDK_LSAPI_H
#define LSSDK_LSAPI_H

#include <windows.h>
#include <ShlObj.h>

// General Constants
#define MAX_LINE_LENGTH 4096
#define MAX_RCCOMMAND   64
#define MAX_BANGCOMMAND 64
#define MAGIC_DWORD     0x49474541

// LM_RELOADMODULE/LM_UNLOADMODULE
#define LMM_PATH      0x0000
#define LMM_HINSTANCE 0x1000

// LM_SYSTRAYINFOEVENT
#define TRAYEVENT_GETICONPOS  1
#define TRAYEVENT_GETICONSIZE 2

// EnumLSData
#define ELD_BANGS       1
#define ELD_MODULES     2
#define ELD_REVIDS      3
#define ELD_BANGS_V2    4
#define ELD_PERFORMANCE 5

// EnumModulesProc
#define LS_MODULE_THREADED 0x0001

// LSLog, LSLogPrintf
#define LOG_ERROR   1
#define LOG_WARNING 2
#define LOG_NOTICE  3
#define LOG_DEBUG   4

// is_valid_pattern
#define PATTERN_VALID  0
#define PATTERN_ESC   -1
#define PATTERN_RANGE -2
#define PATTERN_CLOSE -3
#define PATTERN_EMPTY -4

// matche
#define MATCH_VALID   1
#define MATCH_END     2
#define MATCH_ABORT   3
#define MATCH_RANGE   4
#define MATCH_LITERAL 5
#define MATCH_PATTERN 6

// LM_SYSTRAY
#define TRAY_MAX_TIP_LENGTH       128
#define TRAY_MAX_INFO_LENGTH      256
#define TRAY_MAX_INFOTITLE_LENGTH 64

typedef struct LSNOTIFYICONDATAA {
    DWORD cbSize;
    HWND hWnd;
    UINT uID;
    UINT uFlags;
    UINT uCallbackMessage;
    HICON hIcon;
    CHAR szTip[TRAY_MAX_TIP_LENGTH];
    DWORD dwState;
    DWORD dwStateMask;
    CHAR szInfo[TRAY_MAX_INFO_LENGTH];
    union {
        UINT uTimeout;
        UINT uVersion;
    } DUMMYUNIONNAME;
    CHAR szInfoTitle[TRAY_MAX_INFOTITLE_LENGTH];
    DWORD dwInfoFlags;
    GUID guidItem;
    HICON hBalloonIcon;
} *LPLSNOTIFYICONDATAA;

typedef struct LSNOTIFYICONDATAW {
    DWORD cbSize;
    HWND hWnd;
    UINT uID;
    UINT uFlags;
    UINT uCallbackMessage;
    HICON hIcon;
    WCHAR szTip[TRAY_MAX_TIP_LENGTH];
    DWORD dwState;
    DWORD dwStateMask;
    WCHAR szInfo[TRAY_MAX_INFO_LENGTH];
    union {
        UINT uTimeout;
        UINT uVersion;
    } DUMMYUNIONNAME;
    WCHAR szInfoTitle[TRAY_MAX_INFOTITLE_LENGTH];
    DWORD dwInfoFlags;
    GUID guidItem;
    HICON hBalloonIcon;
} *LPLSNOTIFYICONDATAW;

#if defined(_UNICODE)
#   define LSNOTIFYICONDATA LSNOTIFYICONDATAW
#   define LPLSNOTIFYICONDATA LPLSNOTIFYICONDATAW
#else
#   define LSNOTIFYICONDATA LSNOTIFYICONDATAA
#   define LPLSNOTIFYICONDATA LPLSNOTIFYICONDATAA
#endif

// LM_SYSTRAYINFOEVENT
#define TRAYEVENT_GETICONPOS  1
#define TRAYEVENT_GETICONSIZE 2

typedef struct _SYSTRAYINFOEVENT {
    DWORD cbSize;
    DWORD dwEvent;
    HWND hWnd;
    UINT uID;
    GUID guidItem;
} SYSTRAYINFOEVENT, *LPSYSTRAYINFOEVENT;

// Messages
#define LM_SAVEDATA                   8892  // Module -> Core
#define LM_RESTOREDATA                8893  // Module -> Core
#define LM_SYSTRAYA                   9214  // Core   -> Module
#define LM_SYSTRAYREADY               9215  // Module -> Core
#define LM_SYSTRAYINFOEVENT           9216  // Core   -> Module
#define LM_REGISTERMESSAGE            9263  // Module -> Core
#define LM_UNREGISTERMESSAGE          9264  // Module -> Core
#define LM_GETREVIDA                  9265  // Core   -> Module
#define LM_UNLOADMODULEA              9266  // Module -> Core
#define LM_RELOADMODULEA              9267  // Module -> Core
#define LM_REGISTERHOOKMESSAGE        9268  // Module -> Core
#define LM_UNREGISTERHOOKMESSAGE      9269  // Module -> Core
#define LM_REFRESH                    9305  // Core   -> Module
#define LM_WINDOWCREATED              9501  // Core   -> Module
#define LM_WINDOWDESTROYED            9502  // Core   -> Module
#define LM_ACTIVATESHELLWINDOW        9503  // Core   -> Module
#define LM_WINDOWACTIVATED            9504  // Core   -> Module
#define LM_GETMINRECT                 9505  // Core   -> Module
#define LM_REDRAW                     9506  // Core   -> Module
#define LM_TASKMAN                    9507  // Core   -> Module
#define LM_LANGUAGE                   9508  // Core   -> Module
#define LM_ACCESSIBILITYSTATE         9511  // Core   -> Module
#define LM_APPCOMMAND                 9512  // Core   -> Module
#define LM_WINDOWREPLACED             9513  // Core   -> Module
#define LM_WINDOWREPLACING            9514  // Core   -> Module
#define LM_MONITORCHANGED             9516  // Core   -> Module
#define LM_FULLSCREENACTIVATED        32768 // Core   -> Module
#define LM_FULLSCREENDEACTIVATED      32769 // Core   -> Module
#define LM_TASK_SETPROGRESSSTATE      33024 // Core   -> Module
#define LM_TASK_SETPROGRESSVALUE      33025 // Core   -> Module
#define LM_TASK_MARKASACTIVE          33026 // Core   -> Module
#define LM_TASK_REGISTERTAB           33027 // Core   -> Module
#define LM_TASK_UNREGISTERTAB         33028 // Core   -> Module
#define LM_TASK_SETACTIVETAB          33029 // Core   -> Module
#define LM_TASK_SETTABORDER           33030 // Core   -> Module
#define LM_TASK_SETTABPROPERTIES      33031 // Core   -> Module
#define LM_TASK_SETOVERLAYICON        33032 // Core   -> Module
#define LM_TASK_SETOVERLAYICONDESC    33033 // Core   -> Module
#define LM_TASK_SETTHUMBNAILTOOLTIP   33034 // Core   -> Module
#define LM_TASK_SETTHUMBNAILCLIP      33035 // Core   -> Module
#define LM_TASK_THUMBBARADDBUTTONS    33036 // Core   -> Module
#define LM_TASK_THUMBBARUPDATEBUTTONS 33037 // Core   -> Module
#define LM_TASK_THUMBBARSETIMAGELIST  33038 // Core   -> Module
#define LM_UNLOADMODULEW              33281 // Module -> Core
#define LM_RELOADMODULEW              33282 // Module -> Core
#define LM_GETREVIDW                  33283 // Core   -> Module
#define LM_SYSTRAYW                   33285 // Core   -> Module

#if defined(_UNICODE)
#   define LM_UNLOADMODULE LM_UNLOADMODULEW
#   define LM_RELOADMODULE LM_RELOADMODULEW
#   define LM_GETREVID LM_GETREVIDW
#   define LM_SYSTRAY LM_SYSTRAYW
#else
#   define LM_UNLOADMODULE LM_UNLOADMODULEA
#   define LM_RELOADMODULE LM_RELOADMODULEA
#   define LM_GETREVID LM_GETREVIDA
#   define LM_SYSTRAY LM_SYSTRAYA
#endif

// Min and max progress values sent by LM_TASKSETPROGRESSVALUE
#define TASKSETPROGRESSVALUE_MIN  0
#define TASKSETPROGRESSVALUE_MAX  0xFFFE

// Sent by LM_TASKADDBUTTONS and LM_TASKUPDATEBUTTONS
typedef struct THUMBBUTTONLIST {
    UINT cButtons;
    LPTHUMBBUTTON pButton;
} *LPTHUMBBUTTONLIST;

// Callback Function Pointers
typedef VOID (__cdecl * BANGCOMMANDPROCA)(HWND hwndOwner, LPCSTR pszArgs);
typedef VOID (__cdecl * BANGCOMMANDPROCW)(HWND hwndOwner, LPCWSTR pszArgs);
typedef VOID (__cdecl * BANGCOMMANDPROCEXA)(HWND hwndOwner, LPCSTR pszBangCommandName, LPCSTR pszArgs);
typedef VOID (__cdecl * BANGCOMMANDPROCEXW)(HWND hwndOwner, LPCWSTR pszBangCommandName, LPCWSTR pszArgs);
typedef BOOL (__stdcall * ENUMBANGSPROCA)(LPCSTR pszBangCommandName, LPARAM lParam);
typedef BOOL (__stdcall * ENUMBANGSPROCW)(LPCWSTR pszBangCommandName, LPARAM lParam);
typedef BOOL (__stdcall * ENUMMODULESPROCA)(LPCSTR pszPath, DWORD fdwFlags, LPARAM lParam);
typedef BOOL (__stdcall * ENUMMODULESPROCW)(LPCWSTR pszPath, DWORD fdwFlags, LPARAM lParam);
typedef BOOL (__stdcall * ENUMREVIDSPROCA)(LPCSTR pszRevID, LPARAM lParam);
typedef BOOL (__stdcall * ENUMREVIDSPROCW)(LPCWSTR pszRevID, LPARAM lParam);
typedef BOOL (__stdcall * ENUMBANGSV2PROCA)(HINSTANCE hinstModule, LPCSTR pszBangCommandName, LPARAM lParam);
typedef BOOL (__stdcall * ENUMBANGSV2PROCW)(HINSTANCE hinstModule, LPCWSTR pszBangCommandName, LPARAM lParam);
typedef BOOL (__stdcall * ENUMPERFORMANCEPROCA)(LPCSTR pszPath, DWORD dwLoadTime, LPARAM lParam);
typedef BOOL (__stdcall * ENUMPERFORMANCEPROCW)(LPCWSTR pszPath, DWORD dwLoadTime, LPARAM lParam);

#if defined(_UNICODE)
#   define BANGCOMMANDPROC BANGCOMMANDPROCW
#   define BANGCOMMANDPROCEX BANGCOMMANDPROCEXW
#   define ENUMBANGSPROC ENUMBANGSPROCW
#   define ENUMMODULESPROC ENUMMODULESPROCW
#   define ENUMREVIDSPROC ENUMREVIDSPROCW
#   define ENUMBANGSV2PROC ENUMBANGSV2PROCW
#   define ENUMPERFORMANCEPROC ENUMPERFORMANCEPROCW
#else
#   define BANGCOMMANDPROC BANGCOMMANDPROCA
#   define BANGCOMMANDPROCEX BANGCOMMANDPROCEXA
#   define ENUMBANGSPROC ENUMBANGSPROCA
#   define ENUMMODULESPROC ENUMMODULESPROCA
#   define ENUMREVIDSPROC ENUMREVIDSPROCA
#   define ENUMBANGSV2PROC ENUMBANGSV2PROCA
#   define ENUMPERFORMANCEPROC ENUMPERFORMANCEPROCA
#endif

// Functions
#define EXTERN_CDECL(type) EXTERN_C DECLSPEC_IMPORT type __cdecl
#define EXTERN_STDCALL(type) EXTERN_C DECLSPEC_IMPORT type __stdcall

EXTERN_CDECL(BOOL) AddBangCommandA(LPCSTR pszBangCommandName, BANGCOMMANDPROCA pfnCallback);
EXTERN_CDECL(BOOL) AddBangCommandW(LPCWSTR pszBangCommandName, BANGCOMMANDPROCW pfnCallback);
EXTERN_CDECL(BOOL) AddBangCommandExA(LPCSTR pszBangCommandName, BANGCOMMANDPROCEXA pfnCallback);
EXTERN_CDECL(BOOL) AddBangCommandExW(LPCWSTR pszBangCommandName, BANGCOMMANDPROCEXW pfnCallback);
EXTERN_CDECL(HBITMAP) BitmapFromIcon(HICON hIcon);
EXTERN_CDECL(HRGN) BitmapToRegion(HBITMAP hbmBitmap, COLORREF crTransparent, COLORREF crTolerance, INT xOffset, INT yOffset);
EXTERN_CDECL(VOID) CommandParseA(LPCSTR pszString, LPSTR pszCommandToken, LPSTR pszCommandArgs, UINT cchCommandToken, UINT cchCommandArgs);
EXTERN_CDECL(VOID) CommandParseW(LPCWSTR pszString, LPWSTR pszCommandToken, LPWSTR pszCommandArgs, UINT cchCommandToken, UINT cchCommandArgs);
EXTERN_CDECL(INT) CommandTokenizeA(LPCSTR pszString, LPSTR * ppszBuffers, UINT cBuffers, LPSTR pszExtraBuffer);
EXTERN_CDECL(INT) CommandTokenizeW(LPCWSTR pszString, LPWSTR * ppszBuffers, UINT cBuffers, LPWSTR pszExtraBuffer);
EXTERN_CDECL(HRESULT) EnumLSDataA(UINT uType, FARPROC pfnCallback, LPARAM lParam);
EXTERN_CDECL(HRESULT) EnumLSDataW(UINT uType, FARPROC pfnCallback, LPARAM lParam);
EXTERN_CDECL(VOID) Frame3D(HDC hdcDst, RECT rcFrame, COLORREF crTop, COLORREF crBottom, INT nWidth);
EXTERN_CDECL(HWND) GetLitestepWnd();
EXTERN_CDECL(VOID) GetLSBitmapSize(HBITMAP hbmBitmap, INT *pnWidth, INT *pnHeight);
EXTERN_CDECL(BOOL) GetRCBoolA(LPCSTR pszKeyName, BOOL fValue);
EXTERN_CDECL(BOOL) GetRCBoolW(LPCWSTR pszKeyName, BOOL fValue);
EXTERN_CDECL(BOOL) GetRCBoolDefA(LPCSTR pszKeyName, BOOL fDefault);
EXTERN_CDECL(BOOL) GetRCBoolDefW(LPCWSTR pszKeyName, BOOL fDefault);
EXTERN_CDECL(COLORREF) GetRCColorA(LPCSTR pszKeyName, COLORREF crDefault);
EXTERN_CDECL(COLORREF) GetRCColorW(LPCWSTR pszKeyName, COLORREF crDefault);
EXTERN_CDECL(INT) GetRCCoordinate(LPCSTR pszKeyName, INT nDefault, INT nLimit);
EXTERN_CDECL(DOUBLE) GetRCDoubleA(LPCSTR pszKeyName, DOUBLE fDefault);
EXTERN_CDECL(DOUBLE) GetRCDoubleW(LPCWSTR pszKeyName, DOUBLE fDefault);
EXTERN_CDECL(FLOAT) GetRCFloatA(LPCSTR pszKeyName, FLOAT fDefault);
EXTERN_CDECL(FLOAT) GetRCFloatW(LPCWSTR pszKeyName, FLOAT fDefault);
EXTERN_CDECL(INT) GetRCIntA(LPCSTR pszKeyName, INT nDefault);
EXTERN_CDECL(INT) GetRCIntW(LPCWSTR pszKeyName, INT nDefault);
EXTERN_CDECL(INT64) GetRCInt64A(LPCSTR pszKeyName, INT64 nDefault);
EXTERN_CDECL(INT64) GetRCInt64W(LPCWSTR pszKeyName, INT64 nDefault);
EXTERN_CDECL(BOOL) GetRCLineA(LPCSTR pszKeyName, LPSTR pszBuffer, UINT cchBuffer, LPCSTR pszDefault);
EXTERN_CDECL(BOOL) GetRCLineW(LPCWSTR pszKeyName, LPWSTR pszBuffer, UINT cchBuffer, LPCWSTR pszDefault);
EXTERN_CDECL(BOOL) GetRCStringA(LPCSTR pszKeyName, LPSTR pszBuffer, LPCSTR pszDefault, UINT cchBuffer);
EXTERN_CDECL(BOOL) GetRCStringW(LPCWSTR pszKeyName, LPWSTR pszBuffer, LPCWSTR pszDefault, UINT cchBuffer);
EXTERN_CDECL(VOID) GetResStrA(HINSTANCE hInstance, UINT uID, LPSTR pszBuffer, UINT cchBuffer, LPCSTR pszDefault);
EXTERN_CDECL(VOID) GetResStrW(HINSTANCE hInstance, UINT uID, LPWSTR pszBuffer, UINT cchBuffer, LPCWSTR pszDefault);
EXTERN_CDECL(VOID) GetResStrExA(HINSTANCE hInstance, UINT uID, LPSTR pszBuffer, UINT cchBuffer, LPCSTR pszDefault, ...);
EXTERN_CDECL(VOID) GetResStrExW(HINSTANCE hInstance, UINT uID, LPWSTR pszBuffer, UINT cchBuffer, LPCWSTR pszDefault, ...);
EXTERN_CDECL(BOOL) GetTokenA(LPCSTR pszString, LPSTR pszBuffer, LPCSTR *ppszNext, BOOL fBrackets);
EXTERN_CDECL(BOOL) GetTokenW(LPCWSTR pszString, LPWSTR pszBuffer, LPCWSTR *ppszNext, BOOL fBrackets);
EXTERN_CDECL(BOOL) is_valid_patternA(LPCSTR pszPattern, INT *pnError);
EXTERN_CDECL(BOOL) is_valid_patternW(LPCWSTR pszPattern, INT *pnError);
EXTERN_CDECL(BOOL) LCClose(LPVOID pFile);
EXTERN_CDECL(LPVOID) LCOpenA(LPCSTR pszPath);
EXTERN_CDECL(LPVOID) LCOpenW(LPCWSTR pszPath);
EXTERN_CDECL(BOOL) LCReadNextCommandA(LPVOID pFile, LPSTR pszBuffer, UINT cchBuffer);
EXTERN_CDECL(BOOL) LCReadNextCommandW(LPVOID pFile, LPWSTR pszBuffer, UINT cchBuffer);
EXTERN_CDECL(BOOL) LCReadNextConfigA(LPVOID pFile, LPCSTR pszKeyName, LPSTR pszBuffer, UINT cchBuffer);
EXTERN_CDECL(BOOL) LCReadNextConfigW(LPVOID pFile, LPCWSTR pszKeyName, LPWSTR pszBuffer, UINT cchBuffer);
EXTERN_CDECL(BOOL) LCReadNextLineA(LPVOID pFile, LPSTR pszBuffer, UINT cchBuffer);
EXTERN_CDECL(BOOL) LCReadNextLineW(LPVOID pFile, LPWSTR pszBuffer, UINT cchBuffer);
EXTERN_CDECL(INT) LCTokenizeA(LPCSTR pszString, LPSTR *ppszBuffers, UINT cBuffers, LPSTR pszExtraBuffer);
EXTERN_CDECL(INT) LCTokenizeW(LPCWSTR pszString, LPWSTR *ppszBuffers, UINT cBuffers, LPWSTR pszExtraBuffer);
EXTERN_CDECL(HICON) LoadLSIconA(LPCSTR pszPath, LPVOID pReserved);
EXTERN_CDECL(HICON) LoadLSIconW(LPCWSTR pszPath, LPVOID pReserved);
EXTERN_CDECL(HBITMAP) LoadLSImageA(LPCSTR pszPath, LPVOID pReserved);
EXTERN_CDECL(HBITMAP) LoadLSImageW(LPCWSTR pszPath, LPVOID pReserved);
EXTERN_CDECL(HINSTANCE) LSExecuteA(HWND hwndOwner, LPCSTR pszCommandLine, INT nShowCmd);
EXTERN_CDECL(HINSTANCE) LSExecuteW(HWND hwndOwner, LPCWSTR pszCommandLine, INT nShowCmd);
EXTERN_CDECL(HINSTANCE) LSExecuteExA(HWND hwndOwner, LPCSTR pszOperation, LPCSTR pszCommand, LPCSTR pszArgs, LPCSTR pszDirectory, INT nShowCmd);
EXTERN_CDECL(HINSTANCE) LSExecuteExW(HWND hwndOwner, LPCWSTR pszOperation, LPCWSTR pszCommand, LPCWSTR pszArgs, LPCWSTR pszDirectory, INT nShowCmd);
EXTERN_STDCALL(BOOL) LSGetImagePathA(LPSTR pszBuffer, UINT cchBuffer);
EXTERN_STDCALL(BOOL) LSGetImagePathW(LPWSTR pszBuffer, UINT cchBuffer);
EXTERN_STDCALL(BOOL) LSGetLitestepPathA(LPSTR pszBuffer, UINT cchBuffer);
EXTERN_STDCALL(BOOL) LSGetLitestepPathW(LPWSTR pszBuffer, UINT cchBuffer);
EXTERN_CDECL(BOOL) LSEnumDisplayMonitors(HDC, LPCRECT, MONITORENUMPROC, LPARAM); // See Win32 EnumDisplayMonitors
EXTERN_CDECL(BOOL) LSEnumDisplayDevices(LPCSTR, DWORD, PDISPLAY_DEVICEA, DWORD); // See Win32 EnumDisplayDevices
EXTERN_CDECL(BOOL) LSGetMonitorInfo(HMONITOR, LPMONITORINFO);                    // See Win32 GetMonitorInfo
EXTERN_CDECL(INT) LSGetSystemMetrics(INT);                                       // See Win32 GetSystemMetrics
EXTERN_CDECL(BOOL) LSGetVariableA(LPCSTR pszKeyName, LPSTR pszBuffer);
EXTERN_CDECL(BOOL) LSGetVariableW(LPCWSTR pszKeyName, LPWSTR pszBuffer);
EXTERN_CDECL(BOOL) LSGetVariableExA(LPCSTR pszKeyName, LPSTR pszBuffer, UINT cchBuffer);
EXTERN_CDECL(BOOL) LSGetVariableExW(LPCWSTR pszKeyName, LPWSTR pszBuffer, UINT cchBuffer);
EXTERN_STDCALL(BOOL) LSLog(INT nLevel, LPCSTR pszModule, LPCSTR pszMessage);
EXTERN_CDECL(BOOL) LSLogPrintf(INT nLevel, LPCSTR pszModule, LPCSTR pszFormat, ...);
EXTERN_CDECL(HMONITOR) LSMonitorFromPoint(POINT, DWORD);                         // See Win32 MonitorFromPoint
EXTERN_CDECL(HMONITOR) LSMonitorFromRect(LPCRECT, DWORD);                        // See Win32 MonitorFromRect
EXTERN_CDECL(HMONITOR) LSMonitorFromWindow(HWND, DWORD);                         // See Win32 MonitorFromWindow
EXTERN_CDECL(BOOL) LSSetVariableA(LPCSTR pszKeyName, LPCSTR pszValue);
EXTERN_CDECL(BOOL) LSSetVariableW(LPCWSTR pszKeyName, LPCWSTR pszValue);
EXTERN_CDECL(BOOL) matchA(LPCSTR pszPattern, LPCSTR pszText);
EXTERN_CDECL(BOOL) matchW(LPCWSTR pszPattern, LPCWSTR pszText);
EXTERN_CDECL(INT) matcheA(LPCSTR pszPattern, LPCSTR pszText);
EXTERN_CDECL(INT) matcheW(LPCWSTR pszPattern, LPCWSTR pszText);
EXTERN_CDECL(BOOL) ParseBangCommandA(HWND hwndOwner, LPCSTR pszBangCommandName, LPCSTR pszArgs);
EXTERN_CDECL(BOOL) ParseBangCommandW(HWND hwndOwner, LPCWSTR pszBangCommandName, LPCWSTR pszArgs);
EXTERN_CDECL(INT) ParseCoordinate(LPCSTR pszString, INT nDefault, INT nLimit);
EXTERN_CDECL(BOOL) RemoveBangCommandA(LPCSTR pszBangCommandName);
EXTERN_CDECL(BOOL) RemoveBangCommandW(LPCWSTR pszBangCommandName);
EXTERN_CDECL(VOID) SetDesktopArea(INT nLeft, INT nTop, INT nRight, INT nBottom);
EXTERN_CDECL(VOID) TransparentBltLS(HDC hdcDest, INT nXDest, INT nYDest, INT nWidth, INT nHeight, HDC hdcSrc, INT nXSrc, INT nYSrc, COLORREF crTransparent);
EXTERN_CDECL(VOID) VarExpansionA(LPSTR pszBuffer, LPCSTR pszString);
EXTERN_CDECL(VOID) VarExpansionW(LPWSTR pszBuffer, LPCWSTR pszString);
EXTERN_CDECL(VOID) VarExpansionExA(LPSTR pszBuffer, LPCSTR pszString, UINT cchBuffer);
EXTERN_CDECL(VOID) VarExpansionExW(LPWSTR pszBuffer, LPCWSTR pszString, UINT cchBuffer);

#if defined(_UNICODE)
#   define AddBangCommand AddBangCommandW
#   define AddBangCommandEx AddBangCommandExW
#   define CommandParse CommandParseW
#   define CommandTokenize CommandTokenizeW
#   define EnumLSData EnumLSDataW
#   define GetRCBool GetRCBoolW
#   define GetRCBoolDef GetRCBoolDefW
#   define GetRCColor GetRCColorW
#   define GetRCDouble GetRCDoubleW
#   define GetRCFloat GetRCFloatW
#   define GetRCInt GetRCIntW
#   define GetRCInt64 GetRCInt64W
#   define GetRCLine GetRCLineW
#   define GetRCString GetRCStringW
#   define GetResStr GetResStrW
#   define GetResStrEx GetResStrExW
#   define GetToken GetTokenW
#   define is_valid_pattern is_valid_patternW
#   define LCOpen LCOpenW
#   define LCReadNextCommand LCReadNextCommandW
#   define LCReadNextConfig LCReadNextConfigW
#   define LCReadNextLine LCReadNextLineW
#   define LCTokenize LCTokenizeW
#   define LoadLSIcon LoadLSIconW
#   define LoadLSImage LoadLSImageW
#   define LSExecute LSExecuteW
#   define LSExecuteEx LSExecuteExW
#   define LSGetImagePath LSGetImagePathW
#   define LSGetLitestepPath LSGetLitestepPathW
#   define LSGetVariable LSGetVariableW
#   define LSGetVariableEx LSGetVariableExW
#   define LSSetVariable LSSetVariableW
#   define match matchW
#   define matche matcheW
#   define ParseBangCommand ParseBangCommandW
#   define RemoveBangCommand RemoveBangCommandW
#   define VarExpansion VarExpansionW
#   define VarExpansionEx VarExpansionExW
#else
#   define AddBangCommand AddBangCommandA
#   define AddBangCommandEx AddBangCommandExA
#   define CommandParse CommandParseA
#   define CommandTokenize CommandTokenizeA
#   define EnumLSData EnumLSDataA
#   define GetRCBool GetRCBoolA
#   define GetRCBoolDef GetRCBoolDefA
#   define GetRCColor GetRCColorA
#   define GetRCDouble GetRCDoubleA
#   define GetRCFloat GetRCFloatA
#   define GetRCInt GetRCIntA
#   define GetRCInt64 GetRCInt64A
#   define GetRCLine GetRCLineA
#   define GetRCString GetRCStringA
#   define GetResStr GetResStrA
#   define GetResStrEx GetResStrExA
#   define GetToken GetTokenA
#   define is_valid_pattern is_valid_patternA
#   define LCOpen LCOpenA
#   define LCReadNextCommand LCReadNextCommandA
#   define LCReadNextConfig LCReadNextConfigA
#   define LCReadNextLine LCReadNextLineA
#   define LCTokenize LCTokenizeA
#   define LoadLSIcon LoadLSIconA
#   define LoadLSImage LoadLSImageA
#   define LSExecute LSExecuteA
#   define LSExecuteEx LSExecuteExA
#   define LSGetImagePath LSGetImagePathA
#   define LSGetLitestepPath LSGetLitestepPathA
#   define LSGetVariable LSGetVariableA
#   define LSGetVariableEx LSGetVariableExA
#   define LSSetVariable LSSetVariableA
#   define match matchA
#   define matche matcheA
#   define ParseBangCommand ParseBangCommandA
#   define RemoveBangCommand RemoveBangCommandA
#   define VarExpansion VarExpansionA
#   define VarExpansionEx VarExpansionExA
#endif

#endif // LSSDK_LSAPI_H

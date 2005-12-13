/*
This is a part of the LiteStep Shell Source code.

Copyright (C) 1997-2006 The LiteStep Development Team

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

/**
 * @file
 */

#ifndef __TRAYSERVICE_H
#define __TRAYSERVICE_H

#include "../utility/common.h"
#include "../utility/IService.h"
#include <shellapi.h>
#include <vector>


/** Size of tool tip text buffer */
#define TRAY_MAX_TIP_LENGTH 128

/** Size of balloon tool tip text buffer */
#define TRAY_MAX_INFO_LENGTH 256

/** Size of balloon tool tip title text buffer */
#define TRAY_MAX_INFOTITLE_LENGTH 64


/**
 * Tray icon data sent from the core to modules. Refer to the Platform SDK
 * documentation for <code>NOTIFYICONDATA</code> for additional information.
 */
typedef struct
{
    DWORD cbSize;                                 /**< Size of structure in bytes. */
    HWND hWnd;                                    /**< Handle to the window that receives notifications for this tray icon. */
    UINT uID;                                     /**< Application-defined identifier for this tray icon. */
    UINT uFlags;                                  /**< Flags that indicate which members are valid. */
    UINT uCallbackMessage;                        /**< Application-defined message identifier for notifications. */
    HICON hIcon;                                  /**< Handle of icon to be displayed in the tray. */
    CHAR szTip[TRAY_MAX_TIP_LENGTH];              /**< Tool tip text. */
    DWORD dwState;                                /**< State of this tray icon. */
    DWORD dwStateMask;                            /**< Mask that indicates which state bits are retrieved or modified. */
    CHAR szInfo[TRAY_MAX_INFO_LENGTH];            /**< Balloon tool tip text. */
    union
    {
        UINT uTimeout;                            /**< Timeout for balloon tool tip. */
        UINT uVersion;                            /**< Shell version for <code>NIM_SETVERSION</code>. */
    };
    CHAR szInfoTitle[TRAY_MAX_INFOTITLE_LENGTH];  /**< Title for balloon tool tip. */
    DWORD dwInfoFlags;                            /**< Flags that control appearance and behavior of balloon tool tip. */
    GUID guidItem;                                /**< Reserved. */
}
LSNOTIFYICONDATA;

/** Vector of tray icons */
typedef std::vector<LSNOTIFYICONDATA*> IconVector;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// NOTIFYICONDATA variants
//

/**
 * Native tray icon data structure for Windows 95 and 98.
 */
typedef struct
{
    DWORD cbSize;          /**< Size of structure in bytes. */
    HWND hWnd;             /**< Handle to the window that receives notifications for this tray icon. */
    UINT uID;              /**< Application-defined identifier for this tray icon. */
    UINT uFlags;           /**< Flags that indicate which members are valid. */
    UINT uCallbackMessage; /**< Application-defined message identifier for notifications. */
    HICON hIcon;           /**< Handle of icon to be displayed in the tray. */
    CHAR szTip[64];        /**< Tool tip text. */
} NID_4A;

/**
 * Native tray icon data structure for Windows NT 4.0.
 */
typedef struct
{
    DWORD cbSize;          /**< Size of structure in bytes. */
    HWND hWnd;             /**< Handle to the window that receives notifications for this tray icon. */
    UINT uID;              /**< Application-defined identifier for this tray icon. */
    UINT uFlags;           /**< Flags that indicate which members are valid. */
    UINT uCallbackMessage; /**< Application-defined message identifier for notifications. */
    HICON hIcon;           /**< Handle of icon to be displayed in the tray. */
    WCHAR szTip[64];       /**< Tool tip text. */
} NID_4W;

/**
 * Native tray icon data structure for Windows Me and Internet Explorer 5.
 */
typedef struct
{
    DWORD cbSize;          /**< Size of structure in bytes. */
    HWND hWnd;             /**< Handle to the window that receives notifications for this tray icon. */
    UINT uID;              /**< Application-defined identifier for this tray icon. */
    UINT uFlags;           /**< Flags that indicate which members are valid. */
    UINT uCallbackMessage; /**< Application-defined message identifier for notifications. */
    HICON hIcon;           /**< Handle of icon to be displayed in the tray. */
    CHAR szTip[128];       /**< Tool tip text. */
    DWORD dwState;         /**< State of this tray icon. */
    DWORD dwStateMask;     /**< Mask that indicates which state bits are retrieved or modified. */
    CHAR szInfo[256];      /**< Balloon tool tip text. */
    union
    {
        UINT uTimeout;     /**< Timeout for balloon tool tip. */
        UINT uVersion;     /**< Shell version for <code>NIM_SETVERSION</code>. */
    };
    CHAR szInfoTitle[64];  /**< Title for balloon tool tip. */
    DWORD dwInfoFlags;     /**< Flags that control appearance and behavior of balloon tool tip. */
} NID_5A;

/**
 * Native tray icon data structure for Windows 2000.
 */
typedef struct
{
    DWORD cbSize;          /**< Size of structure in bytes. */
    HWND hWnd;             /**< Handle to the window that receives notifications for this tray icon. */
    UINT uID;              /**< Application-defined identifier for this tray icon. */
    UINT uFlags;           /**< Flags that indicate which members are valid. */
    UINT uCallbackMessage; /**< Application-defined message identifier for notifications. */
    HICON hIcon;           /**< Handle of icon to be displayed in the tray. */
    WCHAR szTip[128];      /**< Tool tip text. */
    DWORD dwState;         /**< State of this tray icon. */
    DWORD dwStateMask;     /**< Mask that indicates which state bits are retrieved or modified. */
    WCHAR szInfo[256];     /**< Balloon tool tip text. */
    union
    {
        UINT uTimeout;     /**< Timeout for balloon tool tip. */
        UINT uVersion;     /**< Shell version for <code>NIM_SETVERSION</code>. */
    };
    WCHAR szInfoTitle[64]; /**< Title for balloon tool tip. */
    DWORD dwInfoFlags;     /**< Flags that control appearance and behavior of balloon tool tip. */
} NID_5W;

/**
 * Native tray icon data structure for Internet Explorer 6.
 */
typedef struct
{
    DWORD cbSize;          /**< Size of structure in bytes. */
    HWND hWnd;             /**< Handle to the window that receives notifications for this tray icon. */
    UINT uID;              /**< Application-defined identifier for this tray icon. */
    UINT uFlags;           /**< Flags that indicate which members are valid. */
    UINT uCallbackMessage; /**< Application-defined message identifier for notifications. */
    HICON hIcon;           /**< Handle of icon to be displayed in the tray. */
    CHAR szTip[128];       /**< Tool tip text. */
    DWORD dwState;         /**< State of this tray icon. */
    DWORD dwStateMask;     /**< Mask that indicates which state bits are retrieved or modified. */
    CHAR szInfo[256];      /**< Balloon tool tip text. */
    union
    {
        UINT uTimeout;     /**< Timeout for balloon tool tip. */
        UINT uVersion;     /**< Shell version for <code>NIM_SETVERSION</code>. */
    };
    CHAR szInfoTitle[64];  /**< Title for balloon tool tip. */
    DWORD dwInfoFlags;     /**< Flags that control appearance and behavior of balloon tool tip. */
    GUID guidItem;         /**< Reserved. */
} NID_6A;

/**
 * Native tray icon data structure for Windows XP.
 */
typedef struct
{
    DWORD cbSize;          /**< Size of structure in bytes. */
    HWND hWnd;             /**< Handle to the window that receives notifications for this tray icon. */
    UINT uID;              /**< Application-defined identifier for this tray icon. */
    UINT uFlags;           /**< Flags that indicate which members are valid. */
    UINT uCallbackMessage; /**< Application-defined message identifier for notifications. */
    HICON hIcon;           /**< Handle of icon to be displayed in the tray. */
    WCHAR szTip[128];      /**< Tool tip text. */
    DWORD dwState;         /**< State of this tray icon. */
    DWORD dwStateMask;     /**< Mask that indicates which state bits are retrieved or modified. */
    WCHAR szInfo[256];     /**< Balloon tool tip text. */
    union
    {
        UINT uTimeout;     /**< Timeout for balloon tool tip. */
        UINT uVersion;     /**< Shell version for <code>NIM_SETVERSION</code>. */
    };
    WCHAR szInfoTitle[64]; /**< Title for balloon tool tip. */
    DWORD dwInfoFlags;     /**< Flags that control appearance and behavior of balloon tool tip. */
    GUID guidItem;         /**< Reserved. */
} NID_6W;

/**
 * Union of tray icon data structure pointers.
 */
union NID_PTR
{
    NID_4A* w9X; /**< Windows 95/98    */
    NID_4W* NT4; /**< Windows NT 4     */
    NID_5A* IE5; /**< Windows Me, IE 5 */
    NID_5W* w2K; /**< Windows 2000     */
    NID_6A* IE6; /**< IE 6             */
    NID_6W* wXP; /**< Windows XP       */
};


/** Size of {@link NID_4A} in bytes. */
const unsigned short NID_4A_SIZE = sizeof(NID_4A); // 9x

/** Size of {@link NID_4W} in bytes. */
const unsigned short NID_4W_SIZE = sizeof(NID_4W); // NT 4.0

/** Size of {@link NID_5A} in bytes. */
const unsigned short NID_5A_SIZE = sizeof(NID_5A); // IE 5 (ME?)

/** Size of {@link NID_5W} in bytes. */
const unsigned short NID_5W_SIZE = sizeof(NID_5W); // IE 5 (2K)

/** Size of {@link NID_6A} in bytes. */
const unsigned short NID_6A_SIZE = sizeof(NID_6A); // IE 6

/** Size of {@link NID_6W} in bytes. */
const unsigned short NID_6W_SIZE = sizeof(NID_6W); // IE 6 (XP)


#ifndef NIF_STATE
#define NIF_STATE       0x00000008
#define NIF_INFO        0x00000010
#define NIF_GUID        0x00000020 // not supported
#define NIS_HIDDEN		0x00000001
#define NIS_SHAREDICON  0x00000002
#define NIM_SETFOCUS    0x00000003 // relayed to systray module
#define NIM_SETVERSION  0x00000004 // relayed to systray module
#endif

/**
 * Undocumented data structure that the system sends to <code>Shell_TrayWnd</code>
 * in response to a call to <code>Shell_NotifyIcon</code>.
 */
typedef struct _SHELLTRAYDATA
{
	DWORD dwUnknown;    /**< Unknown. */
	DWORD dwMessage;    /**< Tray message identifier (<code>NIM_</code>*). */
    NOTIFYICONDATA nid; /**< System-specific tray icon data. */
} SHELLTRAYDATA;

/**
 * Undocumented data structure that the system sends to <code>Shell_TrayWnd</code>
 * in response to a call to <code>SHAppBarMessage</code>.
 */
typedef struct _SHELLAPPBARDATA
{
    APPBARDATA abd;   /**< Application bar data. */
    DWORD dwMessage;  /**< Application bar message identifier (<code>ABM_</code>*). */
    DWORD dwDontKnow; /**< Unknown. */
    DWORD dwNoClue;   /**< Unknown. */
} SHELLAPPBARDATA;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// TrayService
//
// This is the tray service handler. It keeps track of all systray icons and
// loads ShellService-objects. If an icon is added/removed/modified/... it
// notifies all listeners (usually the systray module) via LM_SYSTRAY.
//

/**
 * Manages system tray icon and shell service objects. System tray modules
 * can register to receive notification of events through the
 * <code>LM_SYSTRAY</code> message.
 */
class TrayService : public IService
{
public:

    /**
     * Constructor.
     */
    TrayService();
    
    /**
     * Destructor.
     */
    ~TrayService();
    
    /**
     * Starts the system tray service.
     */
    virtual HRESULT Start();
    
    /**
     * Stops the system tray service.
     */
    virtual HRESULT Stop();
    
    /**
     * Handles appbar notifications from the shell.
     */
    BOOL HandleAppBarMessage(SHELLAPPBARDATA* pData);
    
    /**
     * Handles system tray notifications from the shell.
     */
    BOOL HandleNotification(SHELLTRAYDATA* pstd);
    
    /**
     * Resends all icon data.
     */
    HWND SendSystemTray() const;
    
    /**
     * Handles window messages.
     *
     * @param  hWnd    window handle
     * @param  uMsg    message number
     * @param  wParam  message-specific parameter
     * @param  lParam  message-specific parameter
     * @return message-specific result
     */
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    
private:

    /**
     * Creates Shell_TrayWnd and TrayNotifyWnd windows.
     *
     * @return <code>true</code> if successful or <code>false</code> if an error occurs
     */
    bool _CreateWindows();
    
    /**
     * Destroys Shell_TrayWnd and TrayNotifyWnd windows.
     */
    void _DestroyWindows();
    
    /**
     * Loads shell service objects specified in the registry and adds them
     * to an internal list.
     */
    void _LoadShellServiceObjects();
    
    /**
     * Unloads shell service objects and removes them from the list.
     */
    void _UnloadShellServiceObjects();
    
    /**
     * Adds a tray icon the list. Called in response to a <code>NIM_ADD</code>
     * message from the system.
     *
     * @param  nid  tray icon data
     * @return <code>true</code> if successful or <code>false</code> if an error occurs
     */
    bool _AddIcon(const NOTIFYICONDATA& nid);
    
    /**
     * Modifies an existing tray icon in the list. Called in response to a
     * <code>NIM_MODIFY</code> message from the system.
     *
     * @param  plnid  pointer to existing tray icon data
     * @param  nid    new tray icon data
     * @return <code>true</code> if successful or <code>false</code> if an error occurs
     */
    bool _ModifyIcon(LSNOTIFYICONDATA* plnid, const NOTIFYICONDATA& nid);
    
    /**
     * Removes a tray icon from the list. Called in response to a
     * <code>NIM_DELETE</code> from the system.
     *
     * @param  itIcon  iterator pointing at tray icon to remove
     * @return <code>true</code> if successful or <code>false</code> if an error occurs
     */
    bool _RemoveIcon(IconVector::iterator itIcon);
    
    /**
     * Copies the icon image from one tray icon to another.
     *
     * @param  lnidTarget  tray icon to receive the copy
     * @param  nidSource   tray icon data with icon to copy
     * @return <code>true</code> if successful or <code>false</code> if an error occurs
     */
    bool _CopyIconHandle(LSNOTIFYICONDATA& lnidTarget,
        const NOTIFYICONDATA& nidSource) const;
    
    /**
     * Sends out notificiation of a system tray event.
     *
     * @param  dwMessage  tray message number (<code>NIM_</code>*)
     * @param  plnid      tray icon data
     * @return <code>true</code> if successful or <code>false</code> if an error occurs
     */
    bool _Notify(DWORD dwMessage, LSNOTIFYICONDATA* plnid) const;
    
    /**
     * Copies attributes from one tray icon data structure to another
     * paying attention to difference between shell versions.
     *
     * @param  lnidTarget  tray icon to receive information
     * @param  nidSource   tray icon data to copy
     */
    void _CopyVersionSpecifics(LSNOTIFYICONDATA& lnidTarget,
                               const NOTIFYICONDATA& nidSource) const;
    
    /**
     * Finds a tray icon in the list based on its window handle and
     * identifier and returns an iterator that points to it.
     *
     * @param  hWnd  handle of owner window
     * @param  uId   tray icon identifier
     * @return iterator that points to tray icon. If not found, iterator points past
     *         the end of the list.
     */
    IconVector::iterator _FindIcon(HWND hWnd, UINT uId);
    
    /**
     * Finds a tray icon in the list and returns an iterator that points
     * to it.
     *
     * @param  nid  tray icon data
     * @return iterator that points to tray icon. If not found, iterator points past
     *         the end of the list.
     */
    inline IconVector::iterator _FindIcon(const NOTIFYICONDATA& nid)
    {
        return _FindIcon(nid.hWnd, nid.uID);
    }
    
    /**
     * Returns <code>true</code> if strings in tray icon data are Unicode.
     */
    inline bool _IsUnicode(const NOTIFYICONDATA& nid) const
    {
        return (nid.cbSize != NID_4A_SIZE && nid.cbSize != NID_5A_SIZE && 
            nid.cbSize != NID_6A_SIZE);
    }
    
    /**
     * Returns <code>true</code> if a given tray icon is hidden.
     */
    inline bool _IsHidden(const LSNOTIFYICONDATA& lnid) const
    {
        return ((lnid.uFlags & NIF_STATE) &&
            ((lnid.dwState & lnid.dwStateMask) & NIS_HIDDEN));
    }
    
    /**
     * Returns <code>true</code> if the given tray icon's icon image is
     * shared.
     */
    inline bool _IsShared(const LSNOTIFYICONDATA& lnid) const
    {
        return ((lnid.uFlags & NIF_STATE) &&
            ((lnid.dwState & lnid.dwStateMask) & NIS_SHAREDICON));
    }
    
    /**
     * Returns <code>true</code> if the given tray icon's icon image is
     * shared.
     */
    inline bool _IsShared(const NOTIFYICONDATA& nid) const
    {
        NID_6W* pnid = (NID_6W*)&nid;
        return ((pnid->uFlags & NIF_STATE) &&
            (pnid->cbSize != NID_4A_SIZE) && (pnid->cbSize != NID_4W_SIZE) &&
            ((pnid->dwState & pnid->dwStateMask) & NIS_SHAREDICON));
    }
    
    /**
     * Returns <code>true</code> if the given tray icon is valid.
     */
    inline bool _IsValidIcon(const LSNOTIFYICONDATA* plnid) const
    {
        ASSERT_ISNOTNULL(plnid);
        return ((plnid->uFlags & NIF_ICON) && !_IsHidden(*plnid));
    }
    
    /**
     * Converts a wide character string to ANSI.
     */
    int _ConvertWideToAnsi(char* pszOutput, size_t cchOutput,
        const wchar_t* pwzInput, size_t cchInputMax) const;
    
    /**
     * Copies an ANSI string into an ANSI string buffer.
     */
    bool _StringCopy(LPSTR pszDest, size_t cchDest, LPCSTR pszSrc) const;
    
    /**
     * Copies a wide character string into an ANSI string buffer.
     */
    bool _StringCopy(LPSTR pszDest, size_t cchDest, LPCWSTR pwzSrc) const;
    
    /** <code>true</code> if running under Windows 2000 or later */
    bool m_bWin2000;
    
    /** Handle of the TrayNotifyWnd window */
    HWND m_hNotifyWnd;
    
    /** Handle of the Shell_TrayWnd window */
    HWND m_hTrayWnd;
    
    /** Handle of Litestep's main window */
    HWND m_hLiteStep;
    
    /** Handle of our instance */
    HINSTANCE m_hInstance;
    
    /** List of loaded shell service objects */
    std::vector<struct IOleCommandTarget*> m_ssoList;
    
    /** List of tray icons */
    IconVector m_siVector;
};


#endif //!defined __TRAYSERVICE_H

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

#ifndef __MODULE_H
#define __MODULE_H

#include "../utility/common.h"
#include <string>

typedef int (*ModuleInitExFunc) (HWND, HINSTANCE, LPCSTR);
typedef int (*ModuleQuitFunc) (HINSTANCE);


/**
 * Dynamically-loadable module.
 */
class Module
{
private:

    /** Instance handle of module's DLL */
    HINSTANCE m_hInstance;
    
    /** Thread handle */
    HANDLE m_hThread;
    
    /** Litestep's main window handle */
    HWND m_hMainWindow;
    
    /** Thread ID */
    DWORD m_dwThreadID;
    
    /** Path to module's DLL */
    std::basic_string<TCHAR> m_tzLocation;
    
    /** Path to Litestep's root directory */
    std::basic_string<TCHAR> m_tzAppPath;
    
    /** Pointer to <code>initModuleEx</code> function */
    ModuleInitExFunc m_pInitEx;
    
    /** Pointer to <code>quitModule</code> function */
    ModuleQuitFunc m_pQuit;
    
    /** Flags used to load module */
    DWORD m_dwFlags;
    
    /** Event that is triggered when a threaded module completes initialization */
    HANDLE m_hInitEvent;
    
    /** Event that is triggered to tell a threaded module to shut down */
    HANDLE m_hQuitEvent;

public:

    /**
     * Constructor.
     *
     * @param  sLocation  path to the module's DLL
     * @param  dwFlags    set of flags that control how the module is loaded
     */
    Module(const std::string& sLocation, DWORD dwFlags);
    
    /**
     * Destructor.
     */
    virtual ~Module();
    
    /**
     * Loads and initializes the module. If the module is loaded in its own thread then
     * initialization is done asynchronously. Use the event handle returned by
     * <code>GetInitEvent</code> to wait for initialization to complete. The parameters
     * are passed to the module's <code>initModuleEx</code> function.
     *
     * @param  hMainWindow  handle to Litestep's main window
     * @param  sAppPath     path to Litestep's root directory
     * @return <code>true</code> if successful or <code>false</code> otherwise
     */
    bool Init(HWND hMainWindow, const std::string& sAppPath);
    
    /**
     * Shuts down the module and unloads it. If the module was loaded in its own thread then
     * shutdown is done asynchronously. Use event handle returned by <code>GetQuitEvent</code>
     * to wait for shutdown to complete.
     */
    void Quit();
    
    /**
     * Entry point for the module's main thread.
     *
     * @param  dllModPtr  pointer to <code>this</code>
     * @return unused
     */
    static UINT __stdcall ThreadProc(void* dllModPtr);
    
    /**
     * Handles messages sent to the module's main thread.
     *
     * @param  msg  message information
     */
    void HandleThreadMessage(MSG &msg);
    
    /**
     * Returns this module's DLL instance handle.
     */
    HINSTANCE GetInstance() const { return m_hInstance; }
    
    /**
     * Returns this module's thread handle. Returns <code>NULL</code> if the
     * module was not loaded in its own thread.
     */
    HANDLE GetThread() const { return m_hThread; }
    
    /**
     * Returns an event that is set once this module has been initialized.
     */
    HANDLE GetInitEvent() const { return m_hInitEvent; }
    
    /**
     * Returns an event that is set once this module has shut down.
     */
    HANDLE GetQuitEvent() const { return m_hQuitEvent; }
    
    /**
     * Returns the path to this module's DLL.
     */
    LPCTSTR GetLocation() const { return m_tzLocation.c_str(); }
    
    /**
     * Returns the set of flags used to load this module.
     */
    DWORD GetFlags() const { return m_dwFlags; }
    
    /**
     * Returns <code>TRUE</code> if Litestep provides the message pump for this module's
     * main thread.
     */
    BOOL HasMessagePump() const { return !(m_dwFlags & LS_MODULE_NOTPUMPED); }
    
    /**
     * Returns a pointer to this module's <code>quitModule</code> function.
     */
    ModuleQuitFunc GetQuit() const { return m_pQuit; }
    
    /**
     * Returns a pointer to this module's <code>initModuleEx</code> function.
     */
    ModuleInitExFunc GetInitEx() const { return m_pInitEx; }

private:

    /**
     * Loads this module's DLL.
     *
     * @return <code>true</code> if successful or
     *         <code>false</code> if an error occurs
     */
    bool _LoadDll();
    
    /**
     * Calls this module's <code>initModuleEx</code> function and catches any exceptions
     * that are thrown.
     *
     * @return return value from <code>initModuleEx</code>
     */
    int CallInit();
    
    /**
     * Calls this module's <code>quitModule</code> function and catches any exception
     * that are thrown.
     */
    void CallQuit();
};


#endif // __MODULE_H

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2013  LiteStep Development Team
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
#if !defined(MODULE_H)
#define MODULE_H

#include "../lsapi/lsapidefines.h"
#include "../utility/common.h"
#include <string>


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
    
    /** LiteStep's main window handle */
    HWND m_hMainWindow;
    
    /** Thread ID */
    DWORD m_dwThreadID;
    
    /** Path to module's DLL */
    std::basic_string<TCHAR> m_tzLocation;
    
    /** Path to LiteStep's root directory */
    std::basic_string<TCHAR> m_tzAppPath;
    
    /** Pointer to <code>initModuleEx</code> function */
    initModuleExProc m_pInitEx;
    
    /** Pointer to <code>quitModule</code> function */
    quitModuleProc m_pQuit;
    
    /** Flags used to load module */
    DWORD m_dwFlags;

    /** The amount of time it took to load the module */
    DWORD m_dwLoadTime;
    
    /**
     * Event that is triggered when a threaded module completes initialization
     */
    HANDLE m_hInitEvent;
    HANDLE m_hInitCopyEvent;
    
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
     * Loads and initializes the module. If the module is loaded in its own
     * thread then initialization is done asynchronously. Use the event handle
     * returned by <code>GetInitEvent</code> to wait for initialization to
     * complete. The parameters are passed to the module's
     * <code>initModuleEx</code> function.
     *
     * @param  hMainWindow  handle to LiteStep's main window
     * @param  sAppPath     path to LiteStep's root directory
     * @return <code>true</code> if successful or <code>false</code> otherwise
     */
    bool Init(HWND hMainWindow, const std::string& sAppPath);
    
    /**
     * Shuts down the module and unloads it. If the module was loaded in its
     * own thread then shutdown is done asynchronously. Use event handle
     * returned by <code>GetQuitEvent</code> to wait for shutdown to complete.
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
    static void HandleThreadMessage(MSG &msg);
    
    /**
     * Returns this module's DLL instance handle.
     */
    HINSTANCE GetInstance() const
    {
        return m_hInstance;
    }
    
    /**
     * Returns this module's thread handle. Returns <code>NULL</code> if the
     * module was not loaded in its own thread.
     */
    HANDLE GetThread() const
    {
        return m_hThread;
    }
    
    /**
     * Returns this module's thread handle. Returns <code>NULL</code> if the
     * module was not loaded in its own thread.
     *
     * Caller is responsible for calling CloseHandle() on the return value.
     * Caller must NOT call CloseHandle() until the thread has an exit signal.
     */
    HANDLE TakeThread()
    {
        HANDLE hTemp = m_hThread;
        m_hThread = NULL;
        
        return hTemp;
    }
    
    /**
     * Returns an event that is set once this module has been initialized.
     */
    HANDLE GetInitEvent() const
    {
        return m_hInitEvent;
    }
    
    /**
     * Returns an event that is set once this module has been initialized.
     *
     * Caller is responsible for calling CloseHandle() on the return value.
     * Caller must NOT call CloseHandle() until the event has been set to
     * signaled.
     */
    HANDLE TakeInitEvent()
    {
        HANDLE hTemp = m_hInitEvent;
        m_hInitEvent = NULL;
        
        return hTemp;
    }
    
    /**
     * Returns the path to this module's DLL.
     */
    LPCTSTR GetLocation() const
    {
        return m_tzLocation.c_str();
    }
    
    /**
     * Returns the set of flags used to load this module.
     */
    DWORD GetFlags() const
    {
        return m_dwFlags;
    }

    /**
     * Returns how long this module took to load.
     */
    DWORD GetLoadTime() const
    {
        return m_dwLoadTime;
    }

    /**
     * Returns a pointer to this module's <code>quitModule</code> function.
     */
    quitModuleProc GetQuit() const
    {
        return m_pQuit;
    }
    
    /**
     * Returns a pointer to this module's <code>initModuleEx</code> function.
     */
    initModuleExProc GetInitEx() const
    {
        return m_pInitEx;
    }
    
private:
    /**
     * Loads this module's DLL.
     *
     * @return <code>true</code> if successful or
     *         <code>false</code> if an error occurs
     */
    bool _LoadDll();
    
    /**
     * Calls this module's <code>initModuleEx</code> function.
     *
     * @return return value from <code>initModuleEx</code>
     */
    int CallInit();
    
    /**
     * Calls this module's <code>quitModule</code> function.
     */
    void CallQuit();
};


#endif // MODULE_H

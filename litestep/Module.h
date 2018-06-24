//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2015  LiteStep Development Team
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
	/** Flags used to load module */
	DWORD m_dwFlags;

	/** Path to module's DLL */
	std::basic_string<WCHAR> m_wzLocation;

	/** The amount of time it took to load the module */
	DWORD m_dwLoadTime;

protected:
	void SetLoadTime(DWORD dwLoadTime) 
	{
		m_dwLoadTime = dwLoadTime;
	}

public:
    /**
     * Constructor.
     *
     * @param  sLocation  path to the module's DLL
     * @param  dwFlags    set of flags that control how the module is loaded
     */
	Module(const std::wstring& sLocation, DWORD dwFlags) 
	{
		m_wzLocation = sLocation;
		m_dwFlags = dwFlags;
		m_dwLoadTime = 0;
	}

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
    virtual bool Init(HWND hMainWindow, const std::wstring& sAppPath) = 0;

    /**
     * Shuts down the module and unloads it. If the module was loaded in its
     * own thread then shutdown is done asynchronously. Use event handle
     * returned by <code>GetQuitEvent</code> to wait for shutdown to complete.
     */
    virtual void Quit() = 0;

    /**
     * Returns this module's DLL instance handle.
     */
	virtual HINSTANCE  GetInstance() const = 0;

    /**
     * Returns this module's thread handle. Returns <code>NULL</code> if the
     * module was not loaded in its own thread.
     */
	virtual HANDLE GetThread() const { return nullptr; }
    
    /**
     * Returns this module's thread handle. Returns <code>NULL</code> if the
     * module was not loaded in its own thread.
     *
     * Caller is responsible for calling CloseHandle() on the return value.
     * Caller must NOT call CloseHandle() until the thread has an exit signal.
     */
	virtual HANDLE TakeThread() { return nullptr; };
    
    /**
     * Returns an event that is set once this module has been initialized.
     */
	virtual HANDLE GetInitEvent() const { return nullptr; }

    /**
     * Returns an event that is set once this module has been initialized.
     *
     * Caller is responsible for calling CloseHandle() on the return value.
     * Caller must NOT call CloseHandle() until the event has been set to
     * signaled.
     */
	virtual HANDLE TakeInitEvent() { return nullptr; }

    /**
     * Returns the set of flags used to load this module.
     */
    DWORD GetFlags() const
    {
        return m_dwFlags;
    }

	/**
	* Returns the path to this module's DLL.
	*/
	LPCWCHAR GetLocation() const
	{
		return m_wzLocation.c_str();
	}

	/**
	* Returns how long this module took to load.
	*/
	DWORD GetLoadTime() const
	{
		return m_dwLoadTime;
	}

	/**
	 * Module factory 
	 */
	static Module * CreateInstance(const std::wstring& sLocation, DWORD dwFlags);
};

#endif // MODULE_H

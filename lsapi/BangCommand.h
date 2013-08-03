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
#if !defined(BANGCOMMAND_H)
#define BANGCOMMAND_H

#include "../utility/base.h"
#include "lsapidefines.h"
#include <string>
#include <functional>


/**
 * Encapsulates a bang command.
 */
class Bang : public CountedBase
{
public:
    /**
     * Constructs a bang command.
     *
     * @param  dwThread    thread that owns this bang command
     * @param  pfnBang     callback function
     * @param  pwzCommand  bang command name
     */
    Bang(DWORD dwThread, BangCommandA pfnBang, LPCWSTR pwzCommand);

    /**
     * Constructs a bang command.
     *
     * @param  dwThread    thread that owns this bang command
     * @param  pfnBang     callback function
     * @param  pwzCommand  bang command name
     */
    Bang(DWORD dwThread, BangCommandW pfnBang, LPCWSTR pwzCommand);

    /**
     * Constructs a bang command whose callback function takes the bang
     * command name as a parameter.
     *
     * @param  dwThread    thread that owns this bang command
     * @param  pfnBang     callback function which takes the bang command
     *                     name as a parameter
     * @param  pszCommand  bang command name
     */
    Bang(DWORD dwThread, BangCommandExA pfnBang, LPCWSTR pwzCommand);

    /**
     * Constructs a bang command whose callback function takes the bang
     * command name as a parameter.
     *
     * @param  dwThread    thread that owns this bang command
     * @param  pfnBang     callback function which takes the bang command
     *                     name as a parameter
     * @param  pwzCommand  bang command name
     */
    Bang(DWORD dwThread, BangCommandExW pfnBang, LPCWSTR pwzCommand);
    
    /**
     * Destructor.
     */
    ~Bang();
    
    /**
     * Executes this bang command. This bang command is scheduled for
     * execution on the thread that owns it, unless the current thread
     * owns it in which case it is executed immediately.
     *
     * @param  hCaller    window handle belonging to caller. The bang
     *                    typically uses this as an owner for dialog boxes.
     * @param  pwzParams  parameters for the bang command
     */
    void Execute(HWND hCaller, LPCWSTR pwzParams);
    
    HINSTANCE GetModule() const;
    
private:
    /** Thread that owns this bang command */
    DWORD m_dwThreadID;
    
    /**
     * <code>true</code> if the bang command name is passed to the callback
     * function
     */
    bool m_bEX;

    /** The address of the callback function, inside the module */
    LPVOID m_pAddress;
    
    /** Callback function */
    std::function<void (HWND hwndOwner, LPCWSTR pwzArgs)> m_bBang;
    
    /** Callback function that takes the bang command name as a parameter */
    std::function<void (HWND hwndOwner, LPCWSTR pwzBangCommandName, LPCWSTR pwzArgs)> m_bBangEX;
    
    /** Name of this bang command */
    std::wstring m_sCommand;
};

#endif // BANGCOMMAND_H

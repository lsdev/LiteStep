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
     * @param  pszCommand  bang command name
     */
    Bang(DWORD dwThread, BangCommand pfnBang, LPCSTR pszCommand);
    
    /**
     * Constructs a bang command whose callback function takes the bang
     * command name as a parameter.
     *
     * @param  dwThread    thread that owns this bang command
     * @param  pfnBang     callback function which takes the bang command
     *                     name as a parameter
     * @param  pszCommand  bang command name
     */
    Bang(DWORD dwThread, BangCommandEx pfnBang, LPCSTR pszCommand);
    
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
     * @param  pszParams  parameters for the bang command
     */
    void Execute(HWND hCaller, LPCSTR pszParams);
    
    HINSTANCE GetModule() const;
    
private:
    /** Thread that owns this bang command */
    DWORD m_dwThreadID;
    
    /**
     * <code>true</code> if the bang command name is passed to the callback
     * function
     */
    bool m_bEX;
    
    /** Callback function */
    BangCommand m_bBang;
    
    /** Callback function that takes the bang command name as a parameter */
    BangCommandEx m_bBangEX;
    
    /** Name of this bang command */
    std::string m_szCommand;
};

#endif // BANGCOMMAND_H

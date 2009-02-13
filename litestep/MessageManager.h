//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2009  LiteStep Development Team
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
#if !defined(MESSAGEMANAGER_H)
#define MESSAGEMANAGER_H

#include "../utility/common.h"
#include "../utility/criticalsection.h"

#include <map>
#include <set>


/**
 * Manages association of messages with windows.
 *
 * Communication within LiteStep is done by registering a window for a set
 * of window messages using <code>LM_REGISTERMESSAGE</code>. Whenever
 * LiteStep's main window (GetLitestepWnd) receives a message it doesn't
 * handle, that message is resent to all windows that registered for it.
 */
class MessageManager
{
public:
    /**
     * Constructor.
     */
    MessageManager();
    
    /**
     * Destructor.
     */
    ~MessageManager();
    
    /** Set of window handles */
    typedef std::set<HWND> windowSetT;
    
private:
    /** Maps message numbers to sets of window handles */
    typedef std::map<UINT, windowSetT> messageMapT;
    
    /** Message map */
    messageMapT m_MessageMap;
    
    /** Critical section used to serialize access to data members */
    mutable CriticalSection m_cs;
    
public:
    /**
     * Registers a window as a handler for a message.
     *
     * @param  window   handle of window that will process the message
     * @param  message  message number
     */
    void AddMessage(HWND window, UINT message);
    
    /**
     * Registers a window as a handler for multiple messages.
     *
     * @param  window     handle of window that will process the message
     * @param  pMessages  <code>NULL</code>-terminated array of message numbers
     */
    void AddMessages(HWND window, UINT *pMessages);
    
    /**
     * Unregisters a window as a handler for a message.
     *
     * @param  window   window's handle
     * @param  message  message number
     */
    void RemoveMessage(HWND window, UINT message);
    
    /**
     * Unregisters a window as a handler for multiple messages.
     *
     * @param  window     window's handle
     * @param  pMessages  <code>NULL</code>-terminated array of message numbers
     */
    void RemoveMessages(HWND window, UINT *pMessages);
    
    /**
     * Clears all registrations from the message map.
     */
    void ClearMessages();
    
    /**
     * Sends a message to all windows that have registered for it. Does
     * not return until all windows have processed the message.
     *
     * @param   message  message number
     * @param   wParam   message parameter
     * @param   lParam   message parameter
     * @return  bitwise OR of the results from sending the message to each
     *          registered window
     */
    LRESULT SendMessage(UINT message, WPARAM wParam, LPARAM lParam);
    
    /**
     * Posts a message to all windows that have registered for it. Returns
     * as soon as the messages have been placed in the message queue. Does
     * not wait until windows have processed them.
     *
     * @param   message  message number
     * @param   wParam   message parameter
     * @param   lParam   message parameter
     * @return  <code>TRUE</code> if the operation succeeds or
     *          <code>FALSE</code> otherwise
     */
    BOOL PostMessage(UINT message, WPARAM wParam, LPARAM lParam);
    
    /**
     * Returns <code>TRUE</code> if at least one window is registered for
     * the specified message.
     *
     * @param  message  message number
     * @return <code>TRUE</code> if at least one window is registered for the
     *         message, <code>FALSE</code> otherwise
     */
    BOOL HandlerExists(UINT message);
    
    /**
     * Fills in a window set with the handles of all the windows that are
     * registered for a message.
     *
     * @param  uMsg        message number
     * @param  setWindows  window set to receive results
     * @return <code>true</code> if at least one window is registered for the
     *         message, <code>false</code> otherwise
     */
    bool GetWindowsForMessage(UINT uMsg, windowSetT& setWindows) const;
};


#endif // MESSAGEMANAGER_H

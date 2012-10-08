//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
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
#include "MessageManager.h"


MessageManager::MessageManager()
{
    // do nothing
}


MessageManager::~MessageManager()
{
    // do nothing
}


void MessageManager::AddMessage(HWND window, UINT message)
{
    Lock lock(m_cs);
    m_MessageMap[message].insert(window);
}


void MessageManager::AddMessages(HWND window, UINT *pMessages)
{
    Lock lock(m_cs);
    
    if (pMessages != NULL)
    {
        while (*pMessages != 0)
        {
            AddMessage(window, *pMessages++);
        }
    }
}


void MessageManager::RemoveMessage(HWND window, UINT message)
{
    //
    // This function is the main reason why everything here is locked...
    // You wouldn't want this function to mess around while another thread is in
    // SendMessage for example.
    //
    Lock lock(m_cs);
    
    messageMapT::iterator it = m_MessageMap.find(message);
    
    if (it != m_MessageMap.end())
    {
        it->second.erase(window);
        
        if (it->second.empty())
        {
            m_MessageMap.erase(it);
        }
    }
}


void MessageManager::RemoveMessages(HWND window, UINT *pMessages)
{
    Lock lock(m_cs);
    
    if (pMessages != NULL)
    {
        while (*pMessages != 0)
        {
            RemoveMessage(window, *pMessages++);
        }
    }
}


void MessageManager::ClearMessages(void)
{
    Lock lock(m_cs);
    m_MessageMap.clear();
}


LRESULT MessageManager::SendMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    Lock lock(m_cs);
    LRESULT lResult = 0;
    
    messageMapT::iterator it = m_MessageMap.find(message);
    
    if (it != m_MessageMap.end())
    {
        // Make a copy since modules may unregister messages in their message
        // handlers
        windowSetT windowSet(it->second);
        
        for (windowSetT::const_iterator winIt = windowSet.begin();
            winIt != windowSet.end(); ++winIt)
        {
            lResult |= ::SendMessage(*winIt, message, wParam, lParam);
        }
    }
    
    return lResult;
}


BOOL MessageManager::PostMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    Lock lock(m_cs);
    BOOL bResult = TRUE;
    
    messageMapT::iterator it = m_MessageMap.find(message);
    
    if (it != m_MessageMap.end())
    {
        windowSetT::iterator winIt;
        
        for (winIt = it->second.begin();
            winIt != it->second.end() && bResult; ++winIt)
        {
            bResult = ::PostMessage(*winIt, message, wParam, lParam);
        }
    }
    
    return bResult;
}


BOOL MessageManager::HandlerExists(UINT message)
{
    Lock lock(m_cs);
    return (m_MessageMap.find(message) != m_MessageMap.end()) ? TRUE : FALSE;
}


bool MessageManager::GetWindowsForMessage(UINT uMsg, windowSetT& setWindows) const
{
    Lock lock(m_cs);
    bool bResult = false;
    
    messageMapT::const_iterator iter = m_MessageMap.find(uMsg);
    
    if (iter != m_MessageMap.end())
    {
        setWindows = iter->second;
        bResult = true;
    }
    
    return bResult;
}

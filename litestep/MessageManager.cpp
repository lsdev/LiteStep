/*
This is a part of the LiteStep Shell Source code.

Copyright (C) 1997-2002 The LiteStep Development Team

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
/****************************************************************************
****************************************************************************/
#include "MessageManager.h"


void MessageManager::AddMessage(HWND window, UINT message)
{
	m_MessageMap[message].insert(window);
}

void MessageManager::AddMessages(HWND window, UINT *pMessages)
{
	if (pMessages != NULL)
	{
		while (*pMessages != 0)
		{
			AddMessage(window, *pMessages);
			++pMessages;
		}
	}
}

void MessageManager::RemoveMessage(HWND window, UINT message)
{
	messageMapT::iterator it;
	it = m_MessageMap.find(message);
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
	if (pMessages != NULL)
	{
		while (*pMessages != 0)
		{
			RemoveMessage(window, *pMessages);
			++pMessages;
		}
	}

}

void MessageManager::ClearMessages(void)
{
	m_MessageMap.clear();
}

LRESULT MessageManager::SendMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = 0;

	messageMapT::iterator it = m_MessageMap.find(message);
	if (it != m_MessageMap.end())
	{
		windowSetT::iterator winIt;
		for (winIt = it->second.begin(); winIt != it->second.end(); ++winIt)
		{
			lResult |= ::SendMessage(*winIt, message, wParam, lParam);
		}
	}

	return lResult;
}

BOOL MessageManager::PostMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	BOOL bResult = TRUE;

	messageMapT::iterator it = m_MessageMap.find(message);
	if (it != m_MessageMap.end())
	{
		windowSetT::iterator winIt;
		for (winIt = it->second.begin(); (winIt != it->second.end() && bResult); ++winIt)
		{
			bResult = ::PostMessage(*winIt, message, wParam, lParam);
		}
	}

	return bResult;
}

void MessageManager::GetRevID(UINT message, WPARAM wParam, LPARAM lParam)
{
	messageMapT::iterator it;
	it = m_MessageMap.find(message);
	if (it != m_MessageMap.end())
	{
		char buffer[1024];
		windowSetT::iterator winIt;

		for (winIt = it->second.begin(); winIt != it->second.end(); ++winIt)
		{
			buffer[0] = '\0';
			::SendMessage((HWND) * winIt, message, wParam, (LPARAM)buffer);
			::SendMessage((HWND)lParam, message, 0, (LPARAM)buffer);
		}

	}

}

BOOL MessageManager::HandlerExists(UINT message)
{
	return (m_MessageMap.find(message) != m_MessageMap.end()) ? TRUE : FALSE;
}


/*
void MessageManager::GetWindowsForMessage(UINT message, vector<HWND> &rvHwnd)
{
	messageMapT::iterator it;
	rvHwnd.clear();
	it = m_MessageMap.find(message);
	if (it != m_MessageMap.end())
	{
		insert_iterator<vector<HWND> > iIt(rvHwnd, rvHwnd.begin());
		copy(it->second.begin(), it->second.end(), iIt);
	}
}


void MessageManager::GetMessagesForWindow(HWND hwnd, vector<HWND> &rvMessage)
{
	messageMapT::iterator it;
	rvMessage.clear();
	for (it = m_MessageMap.begin(); it != m_MessageMap.end(); ++it)
	{
		windowSetT::iterator winIt = it->second.find(hwnd);
		if (winIt != it->second.end())
		{
			rvMessage.push_back(*winIt);
		}
	}
}
*/

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2007  Litestep Development Team
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
#include "TrayNotifyIcon.h"
#include "../utility/safestr.h"

//
// ReadMe
//
// The Notification Icons are an interesting beast.  Especially when
// we try to implement a persistent store of them.  Notification Icons
// contain a number of different member data types, each with a different
// lifetime.
//
// The persistent non-volatile members are:
//
// uID and hWnd
//
// Those two members are used to identify a specific Notification Icon
// and are use to communicate back with the owner of the Notification Icon.
//
// The persistent volatile members are:
//
// uFlags, uCallbackMessage, hIcon, szTip, dwState
//
// The only required valid member is hIcon.  The Notification Icon will
// not be displayed until hIcon has been populated.  However, the calling
// application will see a success during NIM_ADD even when hIcon is NULL
// with the exception if NIS_SHAREDICON is set then hIcon must be populated
// for NIM_ADD to return success.
//
// dwState/dwStateMask may also keep the Notification Icon from being
// displayed when set to NIS_HIDDEN.
//
// The rest of the members are non-persistent and are not stored.  They
// are passed on to the systray modules one time and are not repopulated
// on enumeration of the Notification Icons.
//

NotifyIcon::NotifyIcon(const NID_XX& nidSource)
    :m_hWnd(nidSource.hWnd)
    ,m_uID(nidSource.uID)
    ,m_uFlags(0)
    ,m_uCallbackMessage(0)
    ,m_hIcon(NULL)
    ,m_dwState(0)
{
    m_szTip[0] = 0;
    Update(nidSource);
}

NotifyIcon::~NotifyIcon()
{
    if(NULL != m_hIcon)// && !IsShared())
    {
        DestroyIcon(m_hIcon);
    }
}


void NotifyIcon::Update(const NID_XX& nidSource)
{
    //
    // Copy persistent values only
    //

    // callback message
    copy_message(&nidSource);

    // icon
    copy_icon(&nidSource);

    // tool tip string
    copy_tip(&nidSource);

    // state values
    copy_state(&nidSource);
}


void NotifyIcon::copy_message(PCNID_XX pnidSource)
{
    if(NIF_MESSAGE & pnidSource->uFlags)
    {
        m_uCallbackMessage = pnidSource->uCallbackMessage;
        m_uFlags |= NIF_MESSAGE;
    }
}

void NotifyIcon::copy_icon(PCNID_XX pnidSource)
{
    if(NIF_ICON & pnidSource->uFlags)
    {
        if(m_hIcon)// && !IsShared())
        {
            DestroyIcon(m_hIcon);
        }

        m_hIcon = CopyIcon(pnidSource->hIcon);

        if(!m_hIcon)
        {
            m_uFlags &= ~NIF_ICON;
        }
        else
        {
            m_uFlags |= NIF_ICON;
        }
    }
}

void NotifyIcon::copy_tip(PCNID_XX pnidSource)
{
    if(NIF_TIP & pnidSource->uFlags)
    {
        switch(pnidSource->cbSize)
        {
        case NID_7W_SIZE:
        case NID_6W_SIZE:
        case NID_5W_SIZE:
            {
                LPCWSTR pwzSrc = ((NID_5W*)(pnidSource))->szTip;

                int nReturn = WideCharToMultiByte(CP_ACP, 0, pwzSrc, 128,
                    m_szTip, TRAY_MAX_TIP_LENGTH, NULL, NULL);

                if (!nReturn)
                {
                    m_szTip[0] = 0;
                }
            }
            break;

        case NID_4W_SIZE:
            {
                LPCWSTR pwzSrc = ((NID_4W*)(pnidSource))->szTip;

                int nReturn = WideCharToMultiByte(CP_ACP, 0, pwzSrc, 64,
                    m_szTip, TRAY_MAX_TIP_LENGTH, NULL, NULL);

                if (!nReturn)
                {
                    m_szTip[0] = 0;
                }
            }
            break;

        case NID_6A_SIZE:
        case NID_5A_SIZE:
            {
                LPCSTR pwzSrc = ((NID_5A*)(pnidSource))->szTip;

                HRESULT hr = StringCchCopy(m_szTip, 128, pwzSrc);

                if(FAILED(hr))
                {
                    m_szTip[0] = 0;
                }
            }
            break;

        case NID_4A_SIZE:
            {
                LPCSTR pwzSrc = ((NID_4A*)(pnidSource))->szTip;

                HRESULT hr = StringCchCopy(m_szTip, 64, pwzSrc);

                if(FAILED(hr))
                {
                    m_szTip[0] = 0;
                }
            }
            break;
        
        default:
            break;
        }

        if(0 == m_szTip[0])
        {
            m_uFlags &= ~NIF_TIP;
        }
        else
        {
            m_uFlags |= NIF_TIP;
        }
    }
}

void NotifyIcon::copy_state(PCNID_XX pnidSource)
{
    switch (pnidSource->cbSize)
    {
    case NID_7W_SIZE:
    case NID_6W_SIZE:
    case NID_5W_SIZE:
        {
            NID_5W * pnid = (NID_5W*)pnidSource;

            if(NIF_STATE & pnid->uFlags)
            {
                update_state(pnid->dwState, pnid->dwStateMask);
            }
        }
        break;

    case NID_6A_SIZE:
    case NID_5A_SIZE:
        {
            NID_5A * pnid = (NID_5A*)pnidSource;

            if(NIF_STATE & pnid->uFlags)
            {
                update_state(pnid->dwState, pnid->dwStateMask);
            }
        }
        break;

    default:
        break;
    }
}

void NotifyIcon::update_state(DWORD dwState, DWORD dwMask)
{
    DWORD dwTemp = (m_dwState & ~dwMask) \
        | (dwState & dwMask);

    m_dwState = dwTemp;

    if(0 != m_dwState)
    {
        m_uFlags |= NIF_STATE;
    }
    else
    {
        m_uFlags &= ~NIF_STATE;
    }
}

void NotifyIcon::CopyLSNID(LSNOTIFYICONDATA * plsnid, UINT uFlagMask) const
{
    plsnid->cbSize = sizeof(LSNOTIFYICONDATA);
    plsnid->hWnd = m_hWnd;
    plsnid->uID = m_uID;
    plsnid->uFlags = 0;

    if(NIF_MESSAGE & m_uFlags & uFlagMask)
    {
        plsnid->uCallbackMessage = m_uCallbackMessage;
        plsnid->uFlags |= NIF_MESSAGE;
    }

    if(NIF_ICON & m_uFlags & uFlagMask)
    {
        // Use our copy of the icon
        plsnid->hIcon = m_hIcon;
        plsnid->uFlags |= NIF_ICON;
    }

    if(NIF_TIP & m_uFlags & uFlagMask)
    {
        // Use our copy of the string
        HRESULT hr = StringCchCopy(plsnid->szTip, TRAY_MAX_TIP_LENGTH, m_szTip);

        if(SUCCEEDED(hr))
        {
            plsnid->uFlags |= NIF_TIP;
        }
    }
}

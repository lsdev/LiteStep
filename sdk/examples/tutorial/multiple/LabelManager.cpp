// Tutorial
// Copyright (C) 2006 Litestep Development Team
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "common.h"
#include "Label.h"
#include "LabelManager.h"
#include "msgcrack.h"
#include "utility.h"


// Litestep messages that the manager window handles
UINT gMessages[] = { LM_GETREVID, 0 };

// Label manager window class
#define WINDOW_CLASS "TutorialLabelManager"


LabelManager::LabelManager()
{
    for (int i = 0; i < MAX_LABELS; i++)
    {
        mLabels[i] = NULL;
    }
    
    mNumLabels = 0;
    mInstance = NULL;
    mWindow = NULL;
}


LabelManager::~LabelManager()
{
    Destroy();
}


bool LabelManager::Create(HINSTANCE hInstance)
{
    // Save the DLL instance handle since we'll need it later
    mInstance = hInstance;
    
    // Create the window
    mWindow = CreateWindowEx(WS_EX_TOOLWINDOW,
        WINDOW_CLASS, NULL,
        WS_POPUP,
        0, 0,
        0, 0,
        NULL, NULL,
        hInstance, this);
    
    if (!mWindow)
    {
        TRACE2("%s: CreateWindowEx failed, GetLastError returns %d", MODULE_NAME, GetLastError());
        return false;
    }
    
    // Register for Litestep messages that we're interested in
    SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM) mWindow, (LPARAM) gMessages);
    
    // Create the labels
    CreateLabels();
    
    return true;
}


void LabelManager::CreateLabel(LPCTSTR name)
{
    if (mNumLabels == MAX_LABELS)
    {
        // Ignore attempts to create more labels than we can handle
        return;
    }
    
    if (FindLabel(name) != NULL)
    {
        // Ignore attempts to create the same label twice
        return;
    }
    
    // Create the label
    Label *label = new Label;
    
    if (!label->Create(name, mInstance))
    {
        TRACE2("%s: Couldn't create label: %s", MODULE_NAME, name);
        delete label;
        return;
    }
    
    // Add it to the list
    mLabels[mNumLabels++] = label;
}


void LabelManager::CreateLabels()
{
    void *file = LCOpen(NULL);
    
    if (file)
    {
        TCHAR line[MAX_LINE_LENGTH];
        
        // Read all the *Tutorial lines from the configuration file
        while (LCReadNextConfig(file, "*Tutorial", line, MAX_LINE_LENGTH))
        {
            TCHAR key[MAX_LINE_LENGTH];
            TCHAR value[MAX_LINE_LENGTH];
            LPTSTR buffers[2];
            
            buffers[0] = key;
            buffers[1] = value;
            
            // LCReadNextConfig returns the entire line, so we have to parse it
            // to get the label name
            if (LCTokenize(line, buffers, 2, NULL) >= 2)
            {
                CreateLabel(value);
            }
        }
        
        LCClose(file);
    }
}


void LabelManager::Destroy()
{
    // Destroy all the labels
    for (int i = 0; i < mNumLabels; i++)
    {
        if (mLabels[i])
        {
            mLabels[i]->Destroy();
            delete mLabels[i];
            mLabels[i] = NULL;
        }
    }
    
    mNumLabels = 0;
    
    if (mWindow)
    {
        // Unregister Litestep messages
        SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM) mWindow, (LPARAM) gMessages);
        
        // Destroy the window
        DestroyWindow(mWindow);
    }
}


void LabelManager::DestroyLabel(LPCTSTR name)
{
    // Find the label
    for (int i = 0; i < mNumLabels; i++)
    {
        if (StrCompareIgnoreCase(name, mLabels[i]->GetName()) == 0)
        {
            // Destroy the label
            mLabels[i]->Destroy();
            delete mLabels[i];
            
            // Remove the label from the list
            for (int j = i; j < mNumLabels; j++)
            {
                mLabels[j] = mLabels[j + 1];
            }
            
            mLabels[--mNumLabels] = NULL;
        }
    }
}


Label *LabelManager::FindLabel(LPCTSTR name)
{
    for (int i = 0; i < mNumLabels; i++)
    {
        if (StrCompareIgnoreCase(name, mLabels[i]->GetName()) == 0)
        {
            return mLabels[i];
        }
    }
    
    return NULL;
}


int LabelManager::OnGetRevID(LPTSTR buffer)
{
    // This is the string displayed in Litestep's about box
    StrPrintf(buffer, MAX_REVID, "%s %s", MODULE_NAME, MODULE_VERSION);
    return StrLen(buffer);
}


void LabelManager::RegisterWindowClass(HINSTANCE hInstance)
{
    WNDCLASSEX wc;
    
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WindowProcProxy;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(LabelManager *);
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = WINDOW_CLASS;
    wc.hIconSm = NULL;
    
    if (!RegisterClassEx(&wc))
    {
        TRACE2("%s: RegisterClassEx failed, GetLastError returns %d", MODULE_NAME, GetLastError());
    }
}


void LabelManager::UnregisterWindowClass(HINSTANCE hInstance)
{
    UnregisterClass(WINDOW_CLASS, hInstance);
}


LRESULT LabelManager::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        DISPATCH_MSG(LM_GETREVID, wParam, lParam, OnGetRevID);
    }
    
    return DefWindowProc(mWindow, message, wParam, lParam);
}


LRESULT LabelManager::WindowProcProxy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Get a 'this' pointer from the window data
    LabelManager *manager = (LabelManager *) GetWindowLongPtr(hWnd, GWLP_THIS);
    
    if (message == WM_NCCREATE)
    {
        // WM_NCCREATE is our first chance to get our 'this' pointer, which
        // was passed in to CreateWindowEx
        manager = (LabelManager *) ((LPCREATESTRUCT) lParam)->lpCreateParams;
        
        if (manager)
        {
            // Create is still inside CreateWindowEx, so mWindow hasn't been
            // assigned. Assign it here so we can use it in the WM_[NC]CREATE
            // handler.
            manager->mWindow = hWnd;
            
            // Put a 'this' pointer in the window data so we can get it later
            SetWindowLongPtr(hWnd, GWLP_THIS, (LONG_PTR) manager);
        }
    }
    
    if (manager)
    {
        // Relay the message
        return manager->WindowProc(message, wParam, lParam);
    }
    
    // Any messages sent before WM_NCCREATE wind up here
    return DefWindowProc(hWnd, message, wParam, lParam);
}

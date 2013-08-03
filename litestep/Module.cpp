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
#include "module.h"
#include "../lsapi/ThreadedBangCommand.h"
#include "../utility/macros.h"
#include "../utility/core.hpp"
#include "../utility/stringutility.h"

#include <process.h>


Module::Module(const std::wstring& sLocation, DWORD dwFlags)
{
    m_hInstance =  nullptr;
    m_hThread = nullptr;
    m_pInit = nullptr;
    m_hInitEvent = nullptr;
    m_hInitCopyEvent = nullptr;
    m_pQuit = nullptr;
    m_dwFlags = dwFlags;
    m_dwLoadTime = 0;
    m_wzLocation = sLocation;
}


static WORD GetModuleArchitecture(LPCWSTR wzModuleName)
{
    WORD wRet = 0;
    HANDLE hFile = CreateFileW(wzModuleName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        HANDLE hFileMapping = CreateFileMappingW(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
        if (hFileMapping != nullptr)
        {
            LPVOID lpFileBase = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
            if (lpFileBase != nullptr)
            {
                PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)lpFileBase;
                PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((LPBYTE)pDosHeader + pDosHeader->e_lfanew);
                wRet = pNTHeader->OptionalHeader.Magic;
                UnmapViewOfFile(lpFileBase);
            }
            CloseHandle(hFileMapping);
        }
        CloseHandle(hFile);
    }
    return wRet;
}


bool Module::_LoadDll()
{
    bool bReturn = false;
    
    if (!m_hInstance)
    {
        // Modules like popup2 like to call SetErrorMode. While that may not be
        // good style, there is little we can do about it. However, LoadLibrary
        // usually produces helpful error messages such as
        // "msvcp70.dll not found" which are not displayed if a module has
        // disabled them via SetErrorMode. We force their display here.
        // First, make Windows display all errors
        UINT uOldMode = SetErrorMode(0);

        if ((m_hInstance = LoadLibraryW(m_wzLocation.c_str())) != nullptr)
        {
            m_pInit = (initModuleProc)GetProcAddress(
                m_hInstance, "initModuleW");

            if (m_pInit != nullptr) // Might be a legacy module, check for initModuleEx
            {
                initModuleProcA pInit = (initModuleProcA)GetProcAddress(
                    m_hInstance, "initModuleEx");

                if (!pInit) // Might be a BC module, check for underscore
                {
                    pInit = (initModuleProcA)GetProcAddress(
                        m_hInstance, "_initModuleEx");
                }

                if (pInit)
                {
                    m_pInit = [pInit] (HWND hWnd, HINSTANCE hInst, LPCWSTR pwzPath) -> int {
                        char szPath[MAX_PATH];
                        WideCharToMultiByte(CP_ACP, 0, pwzPath, -1,
                            szPath, sizeof(szPath), "", nullptr);
                        return pInit(hWnd, hInst, szPath);
                    };
                }
            }
            
            m_pQuit = (quitModuleProc)GetProcAddress(
                m_hInstance, "quitModule");
            
            if (!m_pQuit)   // Might be a BC module, check for underscore
            {
                m_pQuit = (quitModuleProc)GetProcAddress(
                    m_hInstance, "_quitModule");
            }
            
            if (!m_pInit)
            {
                RESOURCE_STR(nullptr, IDS_INITMODULEEXNOTFOUND_ERROR,
                    L"Error: Could not find initModule().\n"
                    L"\n"
                    L"Please confirm that the dll is a LiteStep module,\n"
                    L"and check with the author for updates.");
            }
            else if (!m_pQuit)
            {
                RESOURCE_STR(nullptr, IDS_QUITMODULENOTFOUND_ERROR,
                    L"Error: Could not find quitModule().\n"
                    L"\n"
                    L"Please confirm that the dll is a LiteStep module.");
            }
            else
            {
                bReturn = true;
            }
        }
        else
        {
            HRESULT hrError = HrGetLastError();

#if defined(_WIN64)
            if (GetModuleArchitecture(m_wzLocation.c_str()) == IMAGE_NT_OPTIONAL_HDR32_MAGIC)
            {
                RESOURCE_STR(nullptr, IDS_MODULEWRONGARCH64_ERROR,
                    L"Error: Could not load module.\n"
                    L"\n"
                    L"The module seems to compiled for 32-bit LiteStep. This is a 64-bit version of LiteStep, which can only load 64-bit modules.");
            }
#else
            if (GetModuleArchitecture(m_wzLocation.c_str()) == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
            {
                RESOURCE_STR(nullptr, IDS_MODULEWRONGARCH32_ERROR,
                    L"Error: Could not load module.\n"
                    L"\n"
                    L"The module seems to compiled for 64-bit LiteStep. This is a 32-bit version of LiteStep, which can only load 32-bit modules.");
            }
#endif
            else if (PathFileExistsW(m_wzLocation.c_str()))
            {
                RESOURCE_STR(nullptr, IDS_MODULEDEPENDENCY_ERROR,
                    L"Error: Could not load module.\n"
                    L"\n"
                    L"This is likely a case of a missing C Run-Time Library"
                    L"or other dependency."
                    L"\n"
                    L"Error Information:"
                    L"\n");

                size_t nLen = 0;
                StringCchLengthW(resourceTextBuffer, _countof(resourceTextBuffer), &nLen);
                DescriptionFromHR(hrError, resourceTextBuffer + nLen, _countof(resourceTextBuffer) - nLen);
            }
            else
            {
                RESOURCE_STR(nullptr, IDS_MODULENOTFOUND_ERROR,
                    L"Error: Could not locate module.\n"
                    L"\n"
                    L"Please check your configuration.");
            }
        }

        // Second, restore the old state
        SetErrorMode(uOldMode);
        
        if (!bReturn)
        {
            LPCWSTR pwzFileName = PathFindFileNameW(m_wzLocation.c_str());
            
            RESOURCE_MSGBOX_F(pwzFileName, MB_ICONERROR);
            
            if (m_hInstance)
            {
                FreeLibrary(m_hInstance);
                m_hInstance = nullptr;
            }
        }
    }
    
    return bReturn;
}


Module::~Module()
{
    if (m_dwFlags & LS_MODULE_THREADED)
    {
        // Note:
        // - This is problematic because these handles may currently
        //   be used in _WaitForModules(), and by closing the handle
        //   here before it is done being used, we may cause invalid
        //   behavior of that function, with potential lockups.
        // - The solution within our current structure, is to allow
        //   an external procedure to take ownership of these handles.
        //   Meaning, whoever calls _WaitForModules would be required
        //   to free these objects when done with them, instead of us
        //   releasing them here. (See: TakeThread() TakeInitEvent())
        // - The correct solution is re-writing our module threading
        //   behavior, which we will be doing on the trunk code.
        if (m_hInitEvent)
        {
            CloseHandle(m_hInitEvent);
            m_hInitEvent = NULL;
        }
        
        if (m_hThread)
        {
            CloseHandle(m_hThread);
            m_hThread = NULL;
        }
    }
    
    if (m_hInstance)
    {
        FreeLibrary(m_hInstance);
        m_hInstance = NULL;
    }
}


bool Module::Init(HWND hMainWindow, const std::wstring& sAppPath)
{
    ASSERT(NULL == m_hInstance);
    
    DWORD dwStartTime = 0;
    __int64 iStartTime, iEndTime, iFrequency;

    if (QueryPerformanceCounter((LARGE_INTEGER*)&iStartTime) == FALSE)
    {
        dwStartTime = GetTickCount();
    }
    
    // delaying the LoadLibrary call until this point is necessary to make
    // grdtransparent work (it hooks LoadLibrary)
    if (_LoadDll())
    {
        ASSERT(nullptr != m_pInit);
        ASSERT(nullptr != m_pQuit);
        
        m_hMainWindow = hMainWindow;
        m_wzAppPath = sAppPath;
        
        if (m_dwFlags & LS_MODULE_THREADED)
        {
            SECURITY_ATTRIBUTES sa;
            
            sa.nLength = sizeof(SECURITY_ATTRIBUTES);
            sa.lpSecurityDescriptor = nullptr;
            sa.bInheritHandle = FALSE;
            
            m_hInitCopyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
            m_hInitEvent = m_hInitCopyEvent;
            // using _beginthreadex instead of CreateThread because modules
            // might use CRT functions
            m_hThread = (HANDLE)_beginthreadex(&sa, 0, Module::ThreadProc,
                this, 0, (UINT*)&m_dwThreadID);
        }
        else
        {
            CallInit();
        }
        
        if (QueryPerformanceCounter((LARGE_INTEGER*)&iEndTime) == FALSE ||
            QueryPerformanceFrequency((LARGE_INTEGER*)&iFrequency) == FALSE)
        {
            m_dwLoadTime = GetTickCount() - dwStartTime;
        }
        else
        {
            m_dwLoadTime = (DWORD)((iEndTime - iStartTime)*1000/iFrequency);
        }
        
        return true;
    }
    
    return false;
}


int Module::CallInit()
{
    ASSERT(m_pInit != nullptr);
    return m_pInit(m_hMainWindow, m_hInstance, m_wzAppPath.c_str());
}


void Module::CallQuit()
{
    ASSERT(m_pQuit != NULL);
    m_pQuit(m_hInstance);
}


void Module::Quit()
{
    if (m_hInstance)
    {
        if (m_dwFlags & LS_MODULE_THREADED)
        {
            PostThreadMessage(m_dwThreadID, WM_DESTROY, 0, (LPARAM)this);
        }
        else
        {
            CallQuit();
        }
    }
}


UINT __stdcall Module::ThreadProc(void* dllModPtr)
{
    Module* dllMod = (Module*)dllModPtr;
    
#if defined(MSVC_DEBUG)
    LPCTSTR pszFileName = PathFindFileName(dllMod->m_wzLocation.c_str());
    DbgSetCurrentThreadName(WCSTOMBS(pszFileName));
#endif
    
    dllMod->CallInit();
    
    // We must use a copy of our event, and hope no one has closed it before
    // waiting for it to be signaled.  See: TakeThread() member function.
    SetEvent(dllMod->m_hInitCopyEvent);
    
    MSG msg;
    
    while (GetMessage(&msg, 0, 0, 0) > 0)
    {
        if (msg.hwnd == NULL)
        {
            // Thread message
            HandleThreadMessage(msg);
        }
        else
        {
            // Window message
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    return 0;
}


void Module::HandleThreadMessage(MSG &msg)
{
    switch (msg.message)
    {
    case LM_THREAD_BANGCOMMAND:
        {
            ThreadedBangCommand * pInfo = (ThreadedBangCommand*)msg.wParam;
            
            if (pInfo != NULL)
            {
                pInfo->Execute();
                pInfo->Release(); //check BangCommand.cpp for the reason
            }
        }
        break;
        
    case WM_DESTROY:
        {
            Module *dll_mod = (Module*)msg.lParam;
            
            if (dll_mod)
            {
                dll_mod->CallQuit();
                PostQuitMessage(0);
            }
        }
        break;
        
    default:
        {
            // do nothing
        }
        break;
    }
}

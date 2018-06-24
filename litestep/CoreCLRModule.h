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
#if !defined(CORECLRMODULE_H)
#define CORECLRMODULE_H

#include "../utility/Base.h"
#include "Module.h"

typedef void (*initModule)(HWND hWndMain, TCHAR * appPath);
typedef void (*quitModule)();

struct ICLRRuntimeHost4;

class CoreCLRModule : public Module
{
private:
    HINSTANCE   m_Instance;
    DWORD       m_domainId;
    initModule  m_pfInitModule;
    quitModule  m_pfQuitModule;

public:
    CoreCLRModule(const std::wstring& sLocation, DWORD dwFlags);
    ~CoreCLRModule();

    bool Init(HWND hMainWindow, const std::wstring& sAppPath);
    void Quit();
    HINSTANCE  GetInstance() const;

private:
    friend class CoreCLRManager;
};



class CoreCLRManager : CountedBase
{
private:
    HINSTANCE           m_coreCLRModule;
    TCHAR*              m_coreClrRootPath;
    TCHAR*              m_trustedAssemblies;
    ICLRRuntimeHost4*   m_pCorRuntimeHost;

public:
    CoreCLRManager();
    ~CoreCLRManager();

    bool LoadRuntime();
    HRESULT LoadModule(CoreCLRModule * module, HWND hWnd, TCHAR * appPath);
    HRESULT UnloadModule(CoreCLRModule * module);

private:
    bool _LoadCLR();
    bool _InitializeRuntime();
    void _GenerateTrustedAssemblyList();
    void _AddFilesToTrustedAssembliesList(TCHAR* location);
    bool _ContainsTrustedAssembly(TCHAR * fileName, const TCHAR** fileExts);
    void _RemoveExtensionAndNi(TCHAR * fileName);
};

#endif


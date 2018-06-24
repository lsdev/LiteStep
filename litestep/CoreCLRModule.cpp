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
#include "CoreCLRModule.h"
#include "mscoree_core.h"
#include "../utility/core.hpp"
#include "../utility/safeutility.h"
#include <string>       

#define CORECLRDLL          _T("coreclr.dll")
#define DEFAULT_CLRVERSION  _T("2.0.0")
#define MAX_CORECLR_PATH    100 * MAX_PATH 

static CoreCLRManager g_clrManager;

CoreCLRModule::CoreCLRModule(const std::wstring & sLocation, DWORD dwFlags)
    : Module(sLocation, dwFlags)
{
    m_Instance = nullptr;
    m_pfInitModule = nullptr;
    m_pfQuitModule = nullptr;
    m_domainId = 0;
}


CoreCLRModule::~CoreCLRModule()
{
    m_Instance = nullptr;
    m_pfInitModule = nullptr;
    m_pfQuitModule = nullptr;
    m_domainId = 0;
}


bool CoreCLRModule::Init(HWND hMainWindow, const std::wstring & sAppPath)
{
    HRESULT hr = E_FAIL;
    
    if (g_clrManager.LoadRuntime())
    {
        TCHAR wzAppPath[MAX_PATH];
        StringCchCopy(wzAppPath, MAX_PATH, sAppPath.c_str());

        hr = g_clrManager.LoadModule(this, hMainWindow, wzAppPath);
        if (SUCCEEDED(hr))
            m_Instance = HINSTANCE();
    }

    return SUCCEEDED(hr);
}


void CoreCLRModule::Quit()
{
    g_clrManager.UnloadModule(this);
}


HINSTANCE CoreCLRModule::GetInstance() const
{
    return m_Instance;
}


CoreCLRManager::CoreCLRManager() : CountedBase()
{
    m_pCorRuntimeHost = nullptr;
    m_coreCLRModule = nullptr;
    m_coreClrRootPath = nullptr;
    m_trustedAssemblies = nullptr;
}


CoreCLRManager::~CoreCLRManager()
{
    SafeDelete(m_coreClrRootPath);
    SafeDelete(m_trustedAssemblies);

    if (m_pCorRuntimeHost)
    {
        m_pCorRuntimeHost->Stop();
        SafeRelease(m_pCorRuntimeHost);
    }
    
    // Free the module would be done for completeness, but in fact CoreCLR.dll 
    // was pinned earlier so this call won't actually free it. The pinning is
    // done because CoreCLR does not support unloading. 
    FreeLibrary(m_coreCLRModule);
}


bool CoreCLRManager::LoadRuntime()
{
    return _LoadCLR() && _InitializeRuntime();
}


HRESULT CoreCLRManager::LoadModule(CoreCLRModule * module, HWND hWnd, TCHAR * location)
{
    if (m_coreCLRModule == nullptr || m_pCorRuntimeHost == nullptr)
        return E_FAIL;

    TCHAR moduleRootPath[MAX_PATH];
    StringCchCopy(moduleRootPath, MAX_PATH, module->GetLocation());
    PathRemoveFileSpec(moduleRootPath);

    TCHAR appPath[MAX_PATH * 2];
    StringCchPrintf(appPath, MAX_PATH * 2, _T("%s;%s"), moduleRootPath, m_coreClrRootPath);

    TCHAR appNiPath[MAX_PATH];
    StringCchPrintf(appNiPath, MAX_PATH, _T("%s\\NI;"), moduleRootPath);

    TCHAR appLocalWinmetadata[MAX_PATH];
    StringCchPrintf(appLocalWinmetadata, MAX_PATH, _T("%s\\WinMetadata;"), moduleRootPath);

    TCHAR appNativeDllPath[MAX_PATH * 50];
    StringCchPrintf(appNativeDllPath, MAX_PATH * 50, _T("%s;%s;"), moduleRootPath, location);

    TCHAR assemblyName[MAX_PATH];
    StringCchCopy(assemblyName, MAX_PATH, PathFindFileName(module->GetLocation()));
    _RemoveExtensionAndNi(assemblyName);

    const TCHAR *property_keys[] = {
        _T("TRUSTED_PLATFORM_ASSEMBLIES"),
        _T("APP_PATHS"),
        _T("APP_NI_PATHS"),
        _T("NATIVE_DLL_SEARCH_DIRECTORIES"),
        _T("APP_LOCAL_WINMETADATA")
    };
    const TCHAR *property_values[] = {
        // TRUSTED_PLATFORM_ASSEMBLIES
        m_trustedAssemblies,
        // APP_PATHS
        appPath,
        // APP_NI_PATHS
        appNiPath,
        // NATIVE_DLL_SEARCH_DIRECTORIES
        appNativeDllPath,
        // APP_LOCAL_WINMETADATA
        appLocalWinmetadata
    };
   
    HRESULT hr = m_pCorRuntimeHost->CreateAppDomainWithManager(
        assemblyName,
        APPDOMAIN_ENABLE_PLATFORM_SPECIFIC_APPS |			// Enable platform-specific assemblies to run
        APPDOMAIN_ENABLE_PINVOKE_AND_CLASSIC_COMINTEROP |	// Allow PInvoking from non-TPA assemblies
        APPDOMAIN_DISABLE_TRANSPARENCY_ENFORCEMENT,         // Entirely disables transparency checks ,
        NULL,
        NULL,
        sizeof(property_keys) / sizeof(TCHAR *),
        property_keys,
        property_values,
        &module->m_domainId
    );
    if (SUCCEEDED(hr))
    {
        TCHAR className[MAX_PATH];
        StringCchPrintf(className, MAX_PATH, _T("%s.LSModule"), assemblyName);

        hr = m_pCorRuntimeHost->CreateDelegate(module->m_domainId, assemblyName, className, _T("initModule"), (INT_PTR*)&module->m_pfInitModule);
        if (SUCCEEDED(hr))
        {
            module->m_pfInitModule(hWnd, appPath);
            AddRef();
        }
    }

    return hr;
}


HRESULT CoreCLRManager::UnloadModule(CoreCLRModule * module)
{
    if (m_coreCLRModule == nullptr || m_pCorRuntimeHost == nullptr)
        return E_FAIL;

    TCHAR assemblyName[MAX_PATH];
    StringCchCopy(assemblyName, MAX_PATH, PathFindFileName(module->GetLocation()));
    _RemoveExtensionAndNi(assemblyName);

    TCHAR className[MAX_PATH];
    StringCchPrintf(className, MAX_PATH, _T("%s.LSModule"), assemblyName);

    HRESULT hr = m_pCorRuntimeHost->CreateDelegate(module->m_domainId, assemblyName, className, _T("quitModule"), (INT_PTR*)&module->m_pfQuitModule);
    if (SUCCEEDED(hr))
    {
        module->m_pfQuitModule();
    }

    DWORD exitCode;
    hr = m_pCorRuntimeHost->UnloadAppDomain2(module->m_domainId, true, (int *)&exitCode);

    Release();
    return hr;
}


bool CoreCLRManager::_LoadCLR()
{
    // IF we are already loaded skip this
    if (m_coreCLRModule != nullptr)
        return true;
    
    SafeDelete(m_coreClrRootPath);
    m_coreClrRootPath = new TCHAR[MAX_PATH];
    TCHAR coreClrDllPath[MAX_PATH];

    // Try to load the version set in the config if it was set
    TCHAR clrVersion[MAX_PATH], buffer[MAX_PATH];
    GetRCStringW(_T("LSCoreCLRVersion"), clrVersion, DEFAULT_CLRVERSION, MAX_PATH);

    DWORD chars = ExpandEnvironmentStrings(_T("%PROGRAMFILES%\\dotnet\\shared\\Microsoft.NETCore.App"), buffer, MAX_PATH);
    if (chars > 1)
    {
        StringCchPrintf(m_coreClrRootPath, MAX_PATH, _T("%s\\%s\\"), buffer, clrVersion);
        StringCchPrintf(coreClrDllPath, MAX_PATH, _T("%s%s"), m_coreClrRootPath, CORECLRDLL);
        m_coreCLRModule = LoadLibraryEx(coreClrDllPath, NULL, 0);
    }

    if (m_coreCLRModule)
    {
        // Pin the module - CoreCLR.dll does not support being unloaded.
        HMODULE dummy_coreCLRModule;
        if (::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN, coreClrDllPath, &dummy_coreCLRModule))
            return true;

        // Pin failed so unload the library
        FreeLibrary(m_coreCLRModule);
    }

    m_coreCLRModule = nullptr;
    return false;
}


bool CoreCLRManager::_InitializeRuntime()
{
    if (m_pCorRuntimeHost)
        return true;

    do
    {
        FnGetCLRRuntimeHost pfnGetCLRRuntimeHost = (FnGetCLRRuntimeHost)::GetProcAddress(m_coreCLRModule, "GetCLRRuntimeHost");
        if (pfnGetCLRRuntimeHost == nullptr)
            break;

        HRESULT hr = pfnGetCLRRuntimeHost(IID_ICLRRuntimeHost2, (IUnknown**)&m_pCorRuntimeHost);
        if (FAILED(hr))
            break;

        hr = m_pCorRuntimeHost->SetStartupFlags(
            static_cast<STARTUP_FLAGS>(
                STARTUP_FLAGS::STARTUP_LOADER_OPTIMIZATION_SINGLE_DOMAIN |
                STARTUP_FLAGS::STARTUP_SINGLE_APPDOMAIN |
                STARTUP_FLAGS::STARTUP_CONCURRENT_GC));
        if (FAILED(hr))
            break;

        hr = m_pCorRuntimeHost->Start();
        if (FAILED(hr))
            break;

        _GenerateTrustedAssemblyList();
        return true;

    } while (false);

    SafeRelease(m_pCorRuntimeHost);
    return false;
}


void CoreCLRManager::_GenerateTrustedAssemblyList()
{
    //
    // TRUSTED_PLATFORM_ASSEMBLIES
    // "Trusted Platform Assemblies" are prioritized by the loader and always loaded with full trust.
    // A common pattern is to include any assemblies next to CoreCLR.dll as platform assemblies.
    // More sophisticated hosts may also include their own Framework extensions (such as AppDomain managers)
    // in this list.
    //
    SafeDelete(m_trustedAssemblies);

    m_trustedAssemblies = new TCHAR[MAX_CORECLR_PATH];
    memset(m_trustedAssemblies, '\0', MAX_CORECLR_PATH);

    // Try looking at the %CORE_LIBRARIES% variable
    size_t outSize;
    TCHAR coreLibrariesPath[MAX_PATH];
    _tgetenv_s(&outSize, coreLibrariesPath, MAX_PATH, _T("CORE_LIBRARIES"));
    if (outSize > 0)
    {
        if (coreLibrariesPath[outSize - 1] != _T('\\'))
            StringCchCat(coreLibrariesPath, MAX_PATH, _T("\\"));

        _AddFilesToTrustedAssembliesList(coreLibrariesPath);
    }

    // Now look into the directory we loaded the CoreCLR.dll from 
    _AddFilesToTrustedAssembliesList(m_coreClrRootPath);
}


void CoreCLRManager::_AddFilesToTrustedAssembliesList(TCHAR * location)
{
    const TCHAR * exts[] = {
        _T("*.ni.dll"),
        _T("*.dll"),
        _T("*.ni.exe"),
        _T("*.exe"),
        _T("*.ni.winmd"),
        _T("*.winmd")
    };
    int extlen = sizeof(exts)/sizeof(TCHAR);
    int tpaSize = MAX_CORECLR_PATH;
    TCHAR asmPath[MAX_PATH];

    for (int i = 0; i < extlen; i++)
    {
        memset(asmPath, '\0', MAX_PATH);
        StringCchPrintf(asmPath, MAX_PATH, _T("%s%s"), location, exts[i]);

        WIN32_FIND_DATA data;
        HANDLE findHandle = FindFirstFile(asmPath, &data);
        if (findHandle != INVALID_HANDLE_VALUE) 
        {
            do {
                if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
                {
                    // It seems that CoreCLR doesn't always use the first instance of an assembly on the TPA list (ni's may be preferred
                    // over il, even if they appear later). So, only include the first instance of a simple assembly name to allow
                    // users the opportunity to override Framework assemblies by placing dlls in %CORE_LIBRARIES%

                    CharLower(data.cFileName);

                    TCHAR fileNameWithoutExtension[MAX_PATH];
                    StringCchCopy(fileNameWithoutExtension, MAX_PATH, data.cFileName);

                    _RemoveExtensionAndNi(fileNameWithoutExtension);

                    // Add to the list if not already on it
                    if (!_ContainsTrustedAssembly(fileNameWithoutExtension, exts))
                    {
                        StringCchPrintf(asmPath, MAX_PATH, _T("%s%s;"), location, data.cFileName);

                        // Check to see if TPA list needs expanded
                        int buffLen = _tcsclen(asmPath) + _tcsclen(m_trustedAssemblies);
                        if (buffLen > tpaSize)
                        {
                            tpaSize = buffLen + 1;

                            TCHAR* newTPAList = new TCHAR[tpaSize];
                            StringCchCopy(newTPAList, tpaSize, m_trustedAssemblies);
                            
                            SafeDelete(m_trustedAssemblies);
                            m_trustedAssemblies = newTPAList;
                        }

                        StringCchCat(m_trustedAssemblies, MAX_CORECLR_PATH, asmPath);
                    }
                }
            } while (0 != FindNextFile(findHandle, &data));

            FindClose(findHandle);
        }
    }
}

bool CoreCLRManager::_ContainsTrustedAssembly(TCHAR * fileName, const TCHAR ** fileExts)
{
    if (_tclen(m_trustedAssemblies) <= 0)
        return false;

    std::wstring asmList(m_trustedAssemblies);

    TCHAR fileNameExt[MAX_PATH];
    int len = sizeof(fileExts) / sizeof(fileExts[0]);

    for (int i = 0; i < len; i++)
    {
        memset(fileNameExt, '\0', MAX_PATH);
        StringCchPrintf(fileNameExt, MAX_PATH, _TEXT("\\%s%s;"), fileName, fileExts[i]);
        if (asmList.find(fileNameExt, 0) != std::wstring::npos) 
            return true;
    }

    return false;
}


void CoreCLRManager::_RemoveExtensionAndNi(TCHAR * fileName)
{
    // Remove extension, if it exists
    TCHAR* extension = wcsrchr(fileName, _T('.'));
    if (extension != NULL)
    {
        extension[0] = _T('\0');

        // Check for .ni
        size_t len = _tclen(fileName);
        if (len > 3 &&
            fileName[len - 1] == _T('i') &&
            fileName[len - 2] == _T('n') &&
            fileName[len - 3] == _T('.'))
        {
            fileName[len - 3] = _T('\0');
        }
    }
}


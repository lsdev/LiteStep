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
#include "CLRModule.h"
#include "../utility/core.hpp"
#include "../utility/safeutility.h"
#include <Shlwapi.h>

using namespace mscorlib;


#define DEFAULT_CLRVERSION  _T("v2.0.50727")
static CLRManager g_clrManager;



CLRModule::CLRModule(const std::wstring& sLocation, DWORD dwFlags)
	: Module(sLocation, dwFlags)
{
    m_pModuleInstance = nullptr;
    m_Instance = nullptr;
}


CLRModule::~CLRModule()
{
	
}


bool CLRModule::Init(HWND hMainWindow, const std::wstring& sAppPath)
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


void CLRModule::Quit()
{
    g_clrManager.UnloadModule(this);
    m_pModuleInstance = nullptr;
    m_Instance = nullptr;
}


HINSTANCE CLRModule::GetInstance() const
{
	return m_Instance;
}


CLRManager::CLRManager()
{
    m_pCorRuntimeHost = nullptr;
    m_pMetaHost = nullptr;
    m_pRuntimeInfo = nullptr;
}


CLRManager::~CLRManager()
{
    _Dispose();
}


bool CLRManager::LoadRuntime()
{
    if (m_pCorRuntimeHost != nullptr)
        return true;

    HRESULT hr;

    do {
        hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_PPV_ARGS(&m_pMetaHost));
        if (FAILED(hr))
        {
            LSLogPrintf(LOG_ERROR, "CLRModule", "CLRCreateInstance failed w/hr 0x%08lx\n", hr);
            break;
        }

        TCHAR clrVersion[MAX_PATH];
        GetRCStringW(L"LSCLRVersion", clrVersion, DEFAULT_CLRVERSION, MAX_PATH);

        hr = m_pMetaHost->GetRuntime(clrVersion, IID_PPV_ARGS(&m_pRuntimeInfo));
        if (FAILED(hr))
        {
            LSLogPrintf(LOG_ERROR, "CLRModule", "ICLRMetaHost::GetRuntime failed w/hr 0x%08lx\n", hr);
            break;
        }

        BOOL fLoadable;
        hr = m_pRuntimeInfo->IsLoadable(&fLoadable);
        if (FAILED(hr))
        {
            LSLogPrintf(LOG_ERROR, "CLRModule", "ICLRRuntimeInfo::IsLoadable failed w/hr 0x%08lx\n", hr);
            break;
        }

        if (!fLoadable)
        {
            LSLogPrintf(LOG_ERROR, "CLRModule", ".NET runtime %s cannot be loaded\n", clrVersion);
            break;
        }

        hr = m_pRuntimeInfo->GetInterface(CLSID_CorRuntimeHost, IID_PPV_ARGS(&m_pCorRuntimeHost));
        if (FAILED(hr))
        {
            LSLogPrintf(LOG_ERROR, "CLRModule", "ICLRRuntimeInfo::GetInterface failed w/hr 0x%08lx\n", hr);
            break;
        }

        hr = m_pCorRuntimeHost->Start();
        if (FAILED(hr))
        {
            LSLogPrintf(LOG_ERROR, "CLRModule", "CLR failed to start w/hr 0x%08lx\n", hr);
            break;
        }
    } while (false);

    //
    // If we fail for any reason then cleanup
    //
    if (FAILED(hr))
    {
        _Dispose();
    }

    return SUCCEEDED(hr);
}

HRESULT CLRManager::LoadModule(CLRModule * module, HWND hWnd, TCHAR * appPath)
{
    HRESULT hr;

    do {
        IUnknownPtr pAppDomainThunk = nullptr;
        hr = m_pCorRuntimeHost->GetDefaultDomain(&pAppDomainThunk);
        if (FAILED(hr))
        {
            LSLogPrintf(LOG_ERROR, "CLRModule", "ICorRuntimeHost::GetDefaultDomain failed w/hr 0x%08lx\n", hr);
            break;
        }

        mscorlib::_AppDomainPtr pDefaultAppDomain = nullptr;
        hr = pAppDomainThunk->QueryInterface(IID_PPV_ARGS(&pDefaultAppDomain));
        if (FAILED(hr))
        {
            LSLogPrintf(LOG_ERROR, "CLRModule", "Failed to get default AppDomain w/hr 0x%08lx\n", hr);
            break;
        }

        TCHAR assemblyName[MAX_PATH];
        StringCchCopy(assemblyName, MAX_PATH, PathFindFileName(module->GetLocation()));
        PathRemoveExtension(assemblyName);

        _AssemblyPtr pAssembly = nullptr;
        bstr_t bstrAssemblyName(assemblyName);

        hr = pDefaultAppDomain->Load_2(bstrAssemblyName, &pAssembly);
        if (FAILED(hr))
        {
            LSLogPrintf(LOG_ERROR, "CLRModule", "Failed to load the assembly w/hr 0x%08lx\n", hr);
            break;
        }

        bstr_t bstrClassName(bstrAssemblyName + _T(".LSModule"));
        hr = pAssembly->GetType_2(bstrClassName, &module->m_pModuleInstance);
        if (FAILED(hr))
        {
            LSLogPrintf(LOG_ERROR, "CLRModule", "Failed to get the Type interface w/hr 0x%08lx\n", hr);
            break;
        }

        bstr_t bstrStaticMethodName(_T("initModule"));
        SAFEARRAY *args = SafeArrayCreateVector(VT_VARIANT, 0, 2);
        LONG index[] = { 0, 1 };

        variant_t vtWndArg;
        V_VT(&vtWndArg) = VT_INT;
        vtWndArg.byref = hWnd;
        SafeArrayPutElement(args, &index[0], &vtWndArg);

        variant_t vtAppPathArg(appPath);
        SafeArrayPutElement(args, &index[1], &vtAppPathArg);

        variant_t vtEmpty, vtRetVal;
        auto bindings = BindingFlags_InvokeMethod | BindingFlags_Static | BindingFlags_Public;

        hr = module->m_pModuleInstance->InvokeMember_3(bstrStaticMethodName, static_cast<BindingFlags>(bindings), NULL, vtEmpty, args, &vtRetVal);
        if (FAILED(hr))
        {
            LSLogPrintf(LOG_ERROR, "CLRModule", "Failed to invoke initModule w/hr 0x%08lx\n", hr);
        }

        SafeArrayDestroy(args);
    } while (false);

    return hr;
}

HRESULT CLRManager::UnloadModule(CLRModule * module)
{
    HRESULT hr = S_OK;

    if (module->m_pModuleInstance != nullptr) {
        bstr_t bstrStaticMethodName(_T("quitModule"));
        SAFEARRAY * args = SafeArrayCreateVector(VT_VARIANT, 0, 0);

        variant_t vtEmpty, vtRetVal;
        auto bindings = BindingFlags_InvokeMethod | BindingFlags_Static | BindingFlags_Public;

        hr = module->m_pModuleInstance->InvokeMember_3(bstrStaticMethodName, static_cast<BindingFlags>(bindings), NULL, vtEmpty, args, &vtRetVal);
        if (FAILED(hr))
        {
            LSLogPrintf(LOG_ERROR, "CLRModule", "Failed to invoke quitModule w/hr 0x%08lx\n", hr);
        }

        SafeArrayDestroy(args);
    }

    return hr;
}

void CLRManager::_Dispose()
{
    if (m_pCorRuntimeHost)
    {
        m_pCorRuntimeHost->Stop();
        SafeRelease(m_pCorRuntimeHost);
    }

    SafeRelease(m_pMetaHost);
    SafeRelease(m_pRuntimeInfo);
}

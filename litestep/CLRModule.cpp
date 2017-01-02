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
#include <Shlwapi.h>
#include "../utility/core.hpp"

#import "mscorlib.tlb" raw_interfaces_only				\
    high_property_prefixes("_get","_put","_putref")		\
    rename("ReportEvent", "InteropServices_ReportEvent")
using namespace mscorlib;


CLRModule::CLRModule(const std::wstring& sLocation, DWORD dwFlags)
	: Module(sLocation, dwFlags)
{
	m_pCorRuntimeHost = nullptr;
	m_pMetaHost = nullptr;
	m_pRuntimeInfo = nullptr;
}


CLRModule::~CLRModule()
{
	_Dispose();
}


bool CLRModule::Init(HWND hMainWindow, const std::wstring& sAppPath)
{
	HRESULT hr = E_FAIL;

	if (_Initialize())
	{
		SAFEARRAY *args = SafeArrayCreateVector(VT_VARIANT, 0, 2);
		variant_t vtWndArg, vtAppPathArg(sAppPath.c_str());
		
		do {
			LONG index = 0;
			
			V_VT(&vtWndArg) = VT_INT;
			vtWndArg.byref = hMainWindow;
			hr = SafeArrayPutElement(args, &index, &vtWndArg);
			if (FAILED(hr))
			{
				break;
			}
			
			index = 1;
			hr = SafeArrayPutElement(args, &index, &vtAppPathArg);
			if (FAILED(hr))
			{
				break;
			}
			
			hr = _InvokeMethod(L"initModule", args);
		} while (false);
		
		SafeArrayDestroy(args);
		args = nullptr;
	}

	return SUCCEEDED(hr);
}


void CLRModule::Quit()
{
	SAFEARRAY * args = SafeArrayCreateVector(VT_VARIANT, 0, 0);

	_InvokeMethod(L"quitModule", args);
	_Dispose();

	SafeArrayDestroy(args);
	args = nullptr;
}


HINSTANCE CLRModule::GetInstance() const
{
	return HINSTANCE();
}


bool CLRModule::_Initialize()
{
	static const WCHAR DEFAULT_CLRVERSION[] = L"v2.0.50727";

	HRESULT hr;
	
	do {
		hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_PPV_ARGS(&m_pMetaHost));
		if (FAILED(hr))
		{
			LSLogPrintf(LOG_ERROR, "CLRModule", "CLRCreateInstance failed w/hr 0x%08lx\n", hr);
			break;
		}

		wchar_t clrVersion[MAX_PATH];
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

	return m_pCorRuntimeHost != nullptr;
}


void CLRModule::_Dispose()
{
	if (m_pMetaHost)
	{
		m_pMetaHost->Release();
		m_pMetaHost = NULL;
	}

	if (m_pRuntimeInfo)
	{
		m_pRuntimeInfo->Release();
		m_pRuntimeInfo = NULL;
	}

	if (m_pCorRuntimeHost)
	{
		m_pCorRuntimeHost->Release();
		m_pCorRuntimeHost = NULL;
	}
}


HRESULT CLRModule::_InvokeMethod(LPCWSTR szMethodName, SAFEARRAY * pArgs)
{
	HRESULT hr;

	IUnknownPtr  pAppDomainThunk = NULL;
	_AppDomainPtr pDefaultAppDomain = NULL;

	do {
		hr = m_pCorRuntimeHost->GetDefaultDomain(&pAppDomainThunk);
		if (FAILED(hr))
		{
			LSLogPrintf(LOG_ERROR, "CLRModule", "ICorRuntimeHost::GetDefaultDomain failed w/hr 0x%08lx\n", hr);
			break;
		}

		hr = pAppDomainThunk->QueryInterface(IID_PPV_ARGS(&pDefaultAppDomain));
		if (FAILED(hr))
		{
			LSLogPrintf(LOG_ERROR, "CLRModule", "Failed to get default AppDomain w/hr 0x%08lx\n", hr);
			break;
		}
				
		__wchar_t * location = (__wchar_t *) malloc(sizeof(__wchar_t) * MAX_PATH_LENGTH);
		ZeroMemory(location, sizeof(__wchar_t) * MAX_PATH_LENGTH);
		StringCchCopy(location, MAX_PATH, GetLocation());

		__wchar_t * classlibrary = PathFindFileName(location);
		PathRemoveExtension(classlibrary);

		bstr_t bstrAssemblyName(classlibrary);
		_AssemblyPtr pAssembly = NULL;
		free(location);

		hr = pDefaultAppDomain->Load_2(bstrAssemblyName, &pAssembly);
		if (FAILED(hr))
		{
			LSLogPrintf(LOG_ERROR, "CLRModule", "Failed to load the assembly w/hr 0x%08lx\n", hr);
			break;
		}

		// Get the Type of ClassLibrary.LSModule
		bstr_t bstrClassName(bstrAssemblyName + L".LSModule");
		_TypePtr pType = NULL;

		hr = pAssembly->GetType_2(bstrClassName, &pType);
		if (FAILED(hr))
		{
			LSLogPrintf(LOG_ERROR, "CLRModule", "Failed to get the Type interface w/hr 0x%08lx\n", hr);
			break;
		}

		bstr_t bstrStaticMethodName(szMethodName);
		variant_t vtEmpty, vtRetVal;

		try {
			hr = pType->InvokeMember_3(bstrStaticMethodName, static_cast<BindingFlags>(
				BindingFlags_InvokeMethod | BindingFlags_Static | BindingFlags_Public),
				NULL, vtEmpty, pArgs, &vtRetVal);
			if (FAILED(hr))
			{
				LSLogPrintf(LOG_ERROR, "CLRModule", "Failed to invoke %s w/hr 0x%08lx\n", szMethodName, hr);
			}
		}
		catch (...) {
			LSLogPrintf(LOG_ERROR, "CLRModule", "A fatal error occured invoking %s\n", szMethodName);
		}

	} while (false);

	return hr;
}
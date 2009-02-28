/*
This is a part of the LiteStep Shell Source code.

Copyright (C) 1997-2005 The LiteStep Development Team

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
/*
	Provides routines useful for localizing code
*/
#include "localization.h"
#include <stdio.h>
#include "safeptr.h"
#include "safestr.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// GetString
//
// Obtains strings from litestep.exe's resources.
//
static void GetString(UINT uId, TCHAR* ptzBuffer, size_t cchBuffer,
                      const TCHAR* ptzDefault)
{
    ASSERT_ISVALIDBUF(ptzBuffer, cchBuffer);
    ASSERT_ISNOTNULL(ptzDefault);
    
    if (LoadString(GetModuleHandle(NULL), uId, ptzBuffer, cchBuffer) == 0)
    {
        HRESULT hr = StringCchCopy(ptzBuffer, cchBuffer, ptzDefault);
        
        if (FAILED(hr) && hr != STRSAFE_E_INSUFFICIENT_BUFFER)
        {
            *ptzBuffer = '\0';
        }
    }
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// CheckedFormat
//
static void CheckedFormat(TCHAR* ptzBuffer, size_t cchBuffer,
                          const TCHAR* ptzFormat, const va_list& vargs)
{
    ASSERT_ISVALIDBUF(ptzBuffer, cchBuffer);
    ASSERT_ISNOTNULL(ptzFormat);
    
    HRESULT hr = E_FAIL;
    
#if !defined(LS_NO_EXCEPTION)
    try
    {
#endif /* LS_NO_EXCEPTION */
        hr = StringCchVPrintf(ptzBuffer, cchBuffer, ptzFormat, vargs);
#if !defined(LS_NO_EXCEPTION)
    }
    catch (...)
    {
        // Don't want exceptions during error reporting
        ASSERT(false);
    }
#endif /* LS_NO_EXCEPTION */
    
    if (FAILED(hr) && (hr != STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        StringCchCopy(ptzBuffer, cchBuffer, ptzFormat);
    }
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// DoError
//
static int DoError(const TCHAR* ptzText, const TCHAR* ptzCaption = NULL)
{
    ASSERT_ISNOTNULL(ptzText);

    TCHAR tzCaption[MAX_LINE_LENGTH] = { _T("LiteStep :: Error") };
    
    if (ptzCaption != NULL)
    {
        StringCchPrintfEx(tzCaption, MAX_LINE_LENGTH,
            NULL, NULL, STRSAFE_NULL_ON_FAILURE,
            _T("LiteStep :: %s :: Error"), ptzCaption);
    }

    return MessageBox(NULL, ptzText, tzCaption,
        MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// Error
//
void Error(UINT uMessageId, LPCTSTR ptzDefault, ...)
{
    ASSERT_ISNOTNULL(ptzDefault);

    TCHAR tzMessage[MAX_LINE_LENGTH] = { 0 };
    TCHAR tzFormat[MAX_LINE_LENGTH] = { 0 };
    
    GetString(uMessageId, tzFormat, MAX_LINE_LENGTH, ptzDefault);
    
    va_list vargs;
    va_start(vargs, ptzDefault);
    
    CheckedFormat(tzMessage, MAX_LINE_LENGTH, tzFormat, vargs);
    
    va_end(vargs);
    
    DoError(tzMessage);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// ErrorEx
//
void ErrorEx(LPCTSTR ptzCaption, UINT uMessageId, LPCTSTR ptzDefault, ...)
{
    ASSERT_ISNOTNULL(ptzDefault);
    ASSERT_ISNOTNULL(ptzCaption);
    
    TCHAR tzFormat[MAX_LINE_LENGTH] = { 0 };
    TCHAR tzMessage[MAX_LINE_LENGTH] = { 0 };
    
    GetString(uMessageId, tzFormat, MAX_LINE_LENGTH, ptzDefault);
    
    va_list vargs;
    va_start(vargs, ptzDefault);
    
    CheckedFormat(tzMessage, MAX_LINE_LENGTH, tzFormat, vargs);
    
    va_end(vargs);
    
    DoError(tzMessage, ptzCaption);
}

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
#ifndef __INCLUDE_LOCALIZATION_H
#define __INCLUDE_LOCALIZATION_H

#include "common.h"

HFONT LocalCreateFontIndirect(LOGFONT *lplf);
HFONT LocalCreateFont(int nHeight,
                      int nWidth,
                      int nEscapement,
                      int nOrientation,
                      int fnWeight,
                      DWORD fdwItalic,
                      DWORD fdwUnderline,
                      DWORD fdwStrikeOut,
                      DWORD fdwCharSet,
                      DWORD fdwOutputPrecision,
                      DWORD fdwClipPrecision,
                      DWORD fdwQuality,
                      DWORD fdwPitchAndFamily,
                      LPCTSTR lpszFace);


class Localization
{
public: 
	Localization();
	~Localization();
	
	void LoadLanguage(LANGID wLanguageID);
	void LoadString(UINT uID, LPTSTR ptzBuffer, size_t cchMax);
	int MessageBox(HWND hWnd, UINT uText, UINT uCaption, UINT uType);
	
private:
	HINSTANCE m_hRes;
	LANGID m_wLanguageID;
};

void Error(UINT uId, const TCHAR* ptzDefault);
void ErrorEx(UINT uId, const TCHAR* ptzDefault, ...);

#define LOCALIZE_THIS 0

#endif // __INCLUDE_LOCALIZATION_H

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
#include "../utility/common.h"
#include "lsapi.h"
#include "../utility/safestr.h" // Always include last in cpp file

#ifndef _NO_PNG_SUPPORT
#include "png_support.h"
#endif // _NO_PNG_SUPPORT


HRGN BitmapToRegion (HBITMAP hbm, COLORREF clrTransp, COLORREF clrTolerance, int xoffset, int yoffset)
{
	// start with a completely transparent rgn
	// this is more correct as no bmp, should render a transparent background
	HRGN hRgn = CreateRectRgn(0, 0, 0, 0);

	if (hbm)
	{
		// create a dc for the 32 bit dib
		HDC hdcMem = CreateCompatibleDC(NULL);
		if (hdcMem)
		{
			VOID * pbits32;
			HBITMAP hbm32;
			BITMAP bm;
			// get the size
			GetObject(hbm, sizeof(BITMAP), &bm);

			BITMAPINFOHEADER bmpInfo32;
			bmpInfo32.biSize	= sizeof(BITMAPINFOHEADER);
			bmpInfo32.biWidth	= bm.bmWidth;
			bmpInfo32.biHeight	= bm.bmHeight;
			bmpInfo32.biPlanes	= 1;
			bmpInfo32.biBitCount	= 32;
			bmpInfo32.biCompression	= BI_RGB;
			bmpInfo32.biSizeImage	= 0;
			bmpInfo32.biXPelsPerMeter	= 0;
			bmpInfo32.biYPelsPerMeter	= 0;
			bmpInfo32.biClrUsed	= 0;
			bmpInfo32.biClrImportant	= 0;

			hbm32 = CreateDIBSection(hdcMem, (BITMAPINFO *) & bmpInfo32, DIB_RGB_COLORS, &pbits32, NULL, 0);
			if (hbm32)
			{
				HBITMAP hbmOld32 = (HBITMAP)SelectObject(hdcMem, hbm32);

				// Create a DC just to copy the bitmap into the memory D
				HDC hdcTmp = CreateCompatibleDC(hdcMem);
				if (hdcTmp)
				{
					// Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits
					int y = 0;
					BITMAP bm32;
					GetObject(hbm32, sizeof(bm32), &bm32);
					while (bm32.bmWidthBytes % 4)
						bm32.bmWidthBytes++;

					// get the limits for the colors
					BYTE clrHiR = (0xff - GetRValue(clrTolerance) > GetRValue(clrTransp)) ? GetRValue(clrTransp) + GetRValue(clrTolerance) : 0xff;
					BYTE clrHiG = (0xff - GetGValue(clrTolerance) > GetGValue(clrTransp)) ? GetGValue(clrTransp) + GetGValue(clrTolerance) : 0xff;
					BYTE clrHiB = (0xff - GetBValue(clrTolerance) > GetBValue(clrTransp)) ? GetBValue(clrTransp) + GetBValue(clrTolerance) : 0xff;
					BYTE clrLoR = (GetRValue(clrTolerance) < GetRValue(clrTransp)) ? GetRValue(clrTransp) - GetRValue(clrTolerance) : 0x00;
					BYTE clrLoG = (GetGValue(clrTolerance) < GetGValue(clrTransp)) ? GetGValue(clrTransp) - GetGValue(clrTolerance) : 0x00;
					BYTE clrLoB = (GetBValue(clrTolerance) < GetBValue(clrTransp)) ? GetBValue(clrTransp) - GetBValue(clrTolerance) : 0x00;

					// Copy the bitmap into the memory D
					HBITMAP hbmOld = (HBITMAP)SelectObject(hdcTmp, hbm);

					BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcTmp, 0, 0, SRCCOPY);

					// Scan each bitmap row from bottom to top (the bitmap is inverted vertically
					BYTE *p;
					BYTE *p32 = (BYTE *)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
					while (y < bm.bmHeight)
					{
						int x = 0;
						while (x < bm.bmWidth)
						{
							int x0 = 0;
							// loop through all transparent pixels...
							while (x < bm.bmWidth)
							{
								p = p32 + 4 * x;

								// if the pixel is non-transparent
								{
									bool isOpaque = *p < clrLoB || *p > clrHiB;
									p++;
									isOpaque |= *p < clrLoG || *p > clrHiG;
									p++;
									isOpaque |= *p < clrLoR || *p > clrHiR;
									if (isOpaque)
										break;
								}

								x++;
							}
							// set first non transparent pixel
							x0 = x;
							// loop through all non transparent pixels
							while (x < bm.bmWidth)
							{
								p = p32 + 4 * x;
								// if the pixel is transparent, then break
								if (*p >= clrLoB && *p <= clrHiB)
								{
									p++;
									if (*p >= clrLoG && *p <= clrHiG)
									{
										p++;
										if (*p >= clrLoR && *p <= clrHiR)
											break;
									}
								}
								x++;
							}
							// if found one or more non-transparent pixels in a row, add them to the rgn...
							if ((x - x0) > 0)
							{
								HRGN hTempRgn = CreateRectRgn(x0 + xoffset, y + yoffset, x + xoffset, y + 1 + yoffset);
								CombineRgn(hRgn, hRgn, hTempRgn, RGN_OR);
								DeleteObject(hTempRgn);
							}
							x++;
						}
						y++;
						p32 -= bm32.bmWidthBytes;
					}
					// Clean up
					SelectObject(hdcTmp, hbmOld);
					DeleteDC(hdcTmp);
				}
				SelectObject(hdcMem, hbmOld32);
				DeleteObject(hbm32);
			}
			DeleteDC(hdcMem);
		}
	}
	return hRgn;
}

#ifndef _NO_PNG_SUPPORT
HANDLE LoadImagePassthru(
    HINSTANCE hinst,      // handle to instance
    LPCSTR lpszName,     // image to load
    UINT uType,           // image type
    int cxDesired,        // desired width
    int cyDesired,        // desired height
    UINT fuLoad        // load options
)
{
	//if ((strstr(lpszName, ".PNG\0") != NULL) || (strstr(lpszName, ".png\0") != NULL))
	if (PathMatchSpec(lpszName, "*.png"))
	{
		return LoadFromPNG(lpszName);
	}
	else
	{
		return LoadImage(hinst, lpszName, uType, cxDesired, cyDesired, fuLoad);
	}
}
#endif // _NO_PNG_SUPPORT


//
// HBITMAP LoadLSImage(LPCSTR pszImage, LPCSTR pszFile)
//
// Takes strings of the form:
//   File.bmp
//   .extract
//   .extract=file.exe[,3]
HBITMAP LoadLSImage(LPCSTR pszImage, LPCSTR pszFile)
{
	char szImageBuf[MAX_PATH];
	char szImageFinal[MAX_PATH];
	HINSTANCE hInstance;
	HBITMAP hbmReturn = NULL;

	if (IsValidStringPtr(pszImage))
	{
		if (strcmpi(pszImage, ".none") != 0)
		{
			hInstance = (HINSTANCE)GetWindowLong(GetLitestepWnd(), GWL_HINSTANCE);
			if (hInstance != NULL)
			{

				StringCchCopy(szImageBuf, MAX_PATH, pszImage);

				// Bitmap merging by Thedd
				//  Thedd - pic1.bmp|pic2.bmp merges the images. Works recursively,
				//  so pic1.bmp|.extract=whatever.dll,3|pic2.bmp also works etc...
				// bitmap merging by grd
				if (strchr(szImageBuf, '|'))
				{

					HDC hdcFirst, hdcSecond, hdcResult;
					HBITMAP hbmFirst, hbmFirstOld;
					HBITMAP hbmSecond, hbmSecondOld;
					HBITMAP	hbmResult, hbmResultOld;
					HBRUSH hbrTransparent;
					RECT rc;
					int wdtFirst, hgtFirst;
					int wdtSecond, hgtSecond;
					int wdtResult, hgtResult;
					char szFirstImage[MAX_PATH];
					char szRestImageBuf[MAX_LINE_LENGTH];
					LPSTR pipepos;

					// get the position after the [|] character
					pipepos = strchr(szImageBuf, '|') + 1;

					// copy the text after to szBuffer2
					StringCchCopy(szFirstImage, MAX_PATH, pipepos);

					// copy the text before to szBuffer1
					StringCchCopyN(szRestImageBuf, MAX_LINE_LENGTH, szImageBuf, (pipepos - szImageBuf) - 1); //StrLen(szImageBuf)-StrLen(szBuffer2)-1);

					// load the two bitmaps
					hbmFirst = LoadLSImage(szFirstImage, pszFile);
					hbmSecond = LoadLSImage(szRestImageBuf, pszFile);

					// if the second one is NULL, then there's no merging to do and
					if (hbmSecond != NULL)
					{
						// create mem dcs for the bitmaps
						hdcFirst = CreateCompatibleDC(NULL);
						hdcSecond = CreateCompatibleDC(NULL);

						// select the bitmaps
						hbmFirstOld = (HBITMAP)SelectObject(hdcFirst, hbmFirst);
						hbmSecondOld = (HBITMAP)SelectObject(hdcSecond, hbmSecond);

						// get the bitmap sizes..
						GetLSBitmapSize(hbmFirst, &wdtFirst, &hgtFirst);
						GetLSBitmapSize(hbmSecond, &wdtSecond, &hgtSecond);

						// in earlier version of bitmap merge, those were painted on to each other
						// now let's paint both images to a new one

						// and we support different sized images!! therefore:
						wdtResult = max(wdtFirst, wdtSecond);
						hgtResult = max(hgtFirst, hgtSecond);

						// create another dc, compatible with second dc
						hdcResult = CreateCompatibleDC(hdcSecond);

						// create a new bitmap for the new dc and select it
						hbmResult = CreateCompatibleBitmap(hdcSecond, wdtResult, hgtResult);
						hbmResultOld = (HBITMAP)SelectObject(hdcResult, hbmResult);

						// paint the background in transparent color...
						hbrTransparent = CreateSolidBrush(RGB(255, 0, 255));

						rc.top = 0;
						rc.left = 0;
						rc.right = wdtResult;
						rc.bottom = hgtResult;

						FillRect(hdcResult, &rc, hbrTransparent);

						DeleteObject(hbrTransparent);

						// first "standard blit" the second image into the new one:
						BitBlt(hdcResult, (wdtResult - wdtSecond) / 2, (hgtResult - hgtSecond) / 2, wdtSecond, hgtSecond, hdcSecond, 0, 0, SRCCOPY);

						// secondly "tranparent blit" the first image over the second one
						TransparentBltLS(hdcResult, (wdtResult - wdtFirst) / 2, (hgtResult - hgtFirst) / 2, wdtFirst, hgtFirst, hdcFirst, 0, 0, RGB(255, 0, 255));

						// deselect the bitmap from the dc and delete the dc to get the image
						SelectObject(hdcResult, hbmResultOld);
						DeleteDC(hdcResult);

						// delete all used objects
						SelectObject(hdcFirst, hbmFirstOld);
						DeleteObject(hbmFirst);
						DeleteDC(hdcFirst);

						SelectObject(hdcSecond, hbmSecondOld);
						DeleteObject(hbmSecond);
						DeleteDC(hdcSecond);

						hbmReturn = hbmResult;
					}
					else
					{
						hbmReturn = hbmFirst;
					}
				}
				// end bitmap merging by grd

				if (!strnicmp(szImageBuf, ".extract", strlen(".extract")))
				{
					HICON hIcon = NULL;

					hIcon = LoadLSIcon(szImageBuf, pszFile);

					if (hIcon)
					{
						hbmReturn = BitmapFromIcon(hIcon);
						DestroyIcon(hIcon);
					}
				}
				else  // For now, we only support .BMP files
				{
					char szFullPath[MAX_PATH];

					LSGetImagePath(szFullPath, MAX_PATH);

					VarExpansionEx(szImageFinal, szImageBuf, MAX_PATH);

					StringCchCat(szFullPath, MAX_PATH, szImageFinal);

					hbmReturn = (HBITMAP)LoadImagePassthru
					            (
					                hInstance,
					                szFullPath,
					                IMAGE_BITMAP,
					                0,
					                0,
					                LR_DEFAULTCOLOR | LR_LOADFROMFILE
					            );

					if (!hbmReturn)
					{
						hbmReturn = (HBITMAP)LoadImagePassthru
						            (
						                hInstance,
						                szImageFinal,
						                IMAGE_BITMAP,
						                0,
						                0,
						                LR_DEFAULTCOLOR | LR_LOADFROMFILE
						            );
					}
				}
			}
		}
	}

	return hbmReturn;
}


//
// BitmapFromIcon(HICON hIcon)
//
// Creates a 64x64 bitmap of an icon, with pink in the transparent regions.
HBITMAP BitmapFromIcon(HICON hIcon)
{
	ICONINFO infoIcon;
	HBITMAP hBitmap = NULL;

	if (GetIconInfo(hIcon, &infoIcon))
	{
		HDC hDC;
		HBITMAP hOldBMP;
		HBRUSH hBrush;
		BITMAP bmBitmap;

		hDC = CreateCompatibleDC(NULL);

		GetObject(infoIcon.hbmColor, sizeof(BITMAP), &bmBitmap);
		hBitmap = CreateBitmapIndirect(&bmBitmap);
		hOldBMP = (HBITMAP)SelectObject(hDC, hBitmap);
		hBrush = CreateSolidBrush(RGB (255, 0, 255));
		DrawIconEx(hDC, 0, 0, hIcon, 0, 0, 0, hBrush, DI_NORMAL);

		DeleteObject(hBrush);
		DeleteObject(infoIcon.hbmMask);
		DeleteObject(infoIcon.hbmColor);
		SelectObject(hDC, hOldBMP);
		DeleteDC(hDC);
	}

	return hBitmap;
}

//
// LoadLSIcon(LPCSTR pszIconPath, LPCSTR pszFile)
//
// Takes strings of the form:
//   File.ico
//   libary.icl,3 <- libary extraction in imagesfolder
//   c:\path\     <- icon extraction for path out of desktop.ini
//   .extract
//   .extract=file.exe[,3]  ... and returns an icon

HICON LoadLSIcon(LPCSTR pszIconPath, LPCSTR pszFile)
{
	char szIconPathBuf[MAX_PATH];
	char szIconPathFinal[MAX_PATH];
	int nIcon = 0;
	HINSTANCE hInstance = NULL;
	HICON hIcon = NULL;

	if (IsValidStringPtr(pszIconPath))
	{

		if (strcmpi(pszIconPath, ".none") != 0)
		{
			hInstance = (HINSTANCE)GetWindowLong(GetLitestepWnd(), GWL_HINSTANCE);
			if (hInstance)
			{
				// here comes a large block which does nothing but turning it into the form
				// <absolute path>[,<iconIndex>]

				// if .extract but nothing else is there...
				// then take the file specified as an icon (could probably be done earlier, but anyhow)
				if (strcmpi(pszIconPath, ".extract") == 0)
				{
					StringCchCopy(szIconPathFinal, MAX_PATH, pszFile);
				}
				else
				{
					// remove ".extract=" (as we won't use it anyway)
					StringCchCopy(szIconPathFinal, MAX_PATH, pszIconPath + (strnicmp(pszIconPath, ".extract=", strlen(".extract=")) ? 0 : strlen(".extract=")));
				}

				VarExpansionEx(szIconPathBuf, szIconPathFinal, MAX_PATH);

				if (strlen(szIconPathBuf) >= 3) // could probably extend it some more
				{
					if (szIconPathBuf[1] != ':' || szIconPathBuf[2] != '\\')
					{
						LSGetImagePath(szIconPathFinal, MAX_PATH);
					}
					else
					{
						szIconPathFinal[0] = '\0';
					}

					StringCchCat(szIconPathFinal, MAX_PATH, szIconPathBuf);

					// large block ends here, now time to separate path and index (if we have an index)
					LPSTR szTemp = strrchr(szIconPathFinal, ',');
					if (szTemp) // iconIndex found
					{
						*szTemp++ = '\0';
						nIcon = atoi(szTemp);
					}

					// now we have the two vars we would like, and the loading can begin

					// well not really, if it's a path, where we're going to get the icon form desktop.ini
					// there is just a little bit more we have to do before we can start loading

					szTemp = strrchr(szIconPathFinal, '\\');
					if (szTemp != NULL)
					{
						if (strlen(szTemp) == 1) // c:\path\ -> desktop.ini
						{
							char szSetting[MAX_PATH];
							StringCchCat(szIconPathFinal, MAX_PATH, "desktop.ini");
							GetPrivateProfileString(".ShellClassInfo", "IconIndex", "0", szSetting, MAX_PATH, szIconPathFinal);
							nIcon = atoi(szSetting);
							GetPrivateProfileString(".ShellClassInfo", "IconFile", "", szSetting, MAX_PATH, szIconPathFinal);
							StringCchCopy(szIconPathFinal, MAX_PATH, szSetting);
						}
					}

					// okay, now it's really time to load the icon...
					// if it's an .ico file we want to do an LoadImage() thing, otherwise it's extracticon
					// so lets find out the extension
					szTemp = strrchr(szIconPathFinal, '.');
					if (!szTemp || strcmpi(szTemp, ".ico")) // it's not an .ico file
					{
						hIcon = ExtractIcon(hInstance, szIconPathFinal, nIcon);
					}
					else
					{
						hIcon = (HICON)LoadImage(hInstance, szIconPathFinal, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
					}
				}
			}
		}
	}

	return hIcon;
}


//
// GetLSBitmapSize(HBITMAP hBitmap, LPINT nWidth, LPINT nHeight)
//
void GetLSBitmapSize(HBITMAP hBitmap, LPINT nWidth, LPINT nHeight)
{
	BITMAP hbmBitmap;

	if (!GetObject(hBitmap, sizeof(hbmBitmap), (LPSTR) & hbmBitmap))
	{
		*nWidth = 0;
		*nHeight = 0;
	}
	else
	{
		*nWidth = hbmBitmap.bmWidth;
		*nHeight = hbmBitmap.bmHeight;
	}
}


void TransparentBltLS(HDC hdcDst, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, COLORREF colorTransparent)
{
	HDC hdcMem, hdcMask, hdcDstCpy;
	HBITMAP hbmMask, hbmMem, hbmDstCpy;
	HBITMAP hbmOldMem, hbmOldMask, hbmOldDstCpy;

	// create a destination compatble dc containing
	// a copy of the destination dc
	hdcDstCpy	= CreateCompatibleDC(hdcDst);
	hbmDstCpy	= CreateCompatibleBitmap(hdcDst, nWidth, nHeight);
	hbmOldDstCpy = (HBITMAP)SelectObject(hdcDstCpy, hbmDstCpy);

	BitBlt(hdcDstCpy, 0, 0, nWidth, nHeight, hdcDst, nXDest, nYDest, SRCCOPY);

	// create a destination compatble dc containing
	// a copy of the source dc
	hdcMem	= CreateCompatibleDC(hdcDst);
	hbmMem	= CreateCompatibleBitmap(hdcDst, nWidth, nHeight);
	hbmOldMem = (HBITMAP)SelectObject(hdcMem, hbmMem);

	BitBlt(hdcMem, 0, 0, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, SRCCOPY);

	// the transparent color should be selected as
	// bkcolor into the memory dc
	SetBkColor(hdcMem, colorTransparent);

	// Create monochrome bitmap for the mask
	hdcMask	= CreateCompatibleDC(hdcDst);
	hbmMask = CreateBitmap(nWidth, nHeight, 1, 1, NULL);
	hbmOldMask = (HBITMAP)SelectObject(hdcMask, hbmMask);

	// Create the mask from the memory dc
	BitBlt(hdcMask, 0, 0, nWidth, nHeight, hdcMem, 0, 0, SRCCOPY);

	// Set the background in hdcMem to black. Using SRCPAINT with black
	// and any other color results in the other color, thus making
	// black the transparent color
	SetBkColor(hdcMem, RGB(0, 0, 0));
	SetTextColor(hdcMem, RGB(255, 255, 255));

	BitBlt(hdcMem, 0, 0, nWidth, nHeight, hdcMask, 0, 0, SRCAND);

	// Set the foreground to black. See comment above.
	SetBkColor(hdcDst, RGB(255, 255, 255));
	SetTextColor(hdcDst, RGB(0, 0, 0));

	BitBlt(hdcDstCpy, 0, 0, nWidth, nHeight, hdcMask, 0, 0, SRCAND);

	// Combine the foreground with the background
	BitBlt(hdcDstCpy, 0, 0, nWidth, nHeight, hdcMem, 0, 0, SRCPAINT);

	// now we have created the image we want to blt
	// in the destination copy dc
	BitBlt(hdcDst, nXDest, nYDest, nWidth, nHeight, hdcDstCpy, 0, 0, SRCCOPY);

	SelectObject(hdcMask, hbmOldMask);
	DeleteObject(hbmMask);
	DeleteDC(hdcMask);

	SelectObject(hdcMem, hbmOldMem);
	DeleteObject(hbmMem);
	DeleteDC(hdcMem);

	SelectObject(hdcDstCpy, hbmOldDstCpy);
	DeleteObject(hbmDstCpy);
	DeleteDC(hdcDstCpy);
}


void Frame3D(HDC dc, RECT rect, COLORREF TopColor, COLORREF BottomColor, int Width)
{
	HPEN hPen1 = CreatePen(PS_SOLID, 1, TopColor);
	HPEN hPen2 = CreatePen(PS_SOLID, 1, BottomColor);
	HPEN OldPen;
	POINT points[3];

	rect.bottom--;
	rect.right--;

	OldPen = (HPEN)SelectObject(dc, hPen1);

	while (Width > 0)
	{
		POINT p;
		Width--;

		points[0].x = rect.left;
		points[0].y = rect.bottom;
		points[1].x = rect.left;
		points[1].y = rect.top;
		points[2].x = rect.right;
		points[2].y = rect.top;

		Polyline(dc, points, 3);

		SelectObject(dc, hPen2);

		p = points[0];
		points[0] = points[2];
		points[1].x = rect.right;
		points[1].y = rect.bottom;
		points[2] = p;
		points[2].x--;

		Polyline(dc, points, 3);

		SelectObject(dc, hPen1);

		InflateRect(&rect, -1, -1);
	}
	SelectObject(dc, OldPen);
	DeleteObject(hPen1);
	DeleteObject(hPen2);
}

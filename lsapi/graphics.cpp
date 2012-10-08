//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2012  LiteStep Development Team
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
#include "png_support.h"
#include "../utility/core.hpp"
#include <algorithm>


static void TransparentBltLSWorker(
    HDC hdcDst, int nWidth, int nHeight,
    HDC hdcSrc, int nXSrc, int nYSrc,
    COLORREF colorTransparent);


//
// Since the transparent color for all LiteStep modules should be 0xFF00FF and
// we are going to assume a tolerance of 0x000000, we can ignore the clrTransp
// and clrTolerence parameter and hard code the values for the High and Low RGB
// bytes The orginial code is commented out, if a module needs the removed
// functionality, it should implement this function with the commented out code.
HRGN BitmapToRegion(
    HBITMAP hbm,
    COLORREF clrTransp, COLORREF clrTolerance,
    int xoffset, int yoffset)
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
            VOID *pbits32;
            HBITMAP hbm32;
            BITMAP bm;
            
            // get the size
            GetObject(hbm, sizeof(BITMAP), &bm);
            
            BITMAPINFOHEADER bmpInfo32;
            bmpInfo32.biSize          = sizeof(BITMAPINFOHEADER);
            bmpInfo32.biWidth         = bm.bmWidth;
            bmpInfo32.biHeight        = bm.bmHeight;
            bmpInfo32.biPlanes        = 1;
            bmpInfo32.biBitCount      = 32;
            bmpInfo32.biCompression   = BI_RGB;
            bmpInfo32.biSizeImage     = 0;
            bmpInfo32.biXPelsPerMeter = 0;
            bmpInfo32.biYPelsPerMeter = 0;
            bmpInfo32.biClrUsed       = 0;
            bmpInfo32.biClrImportant  = 0;
            
            hbm32 = CreateDIBSection(hdcMem, (BITMAPINFO*)&bmpInfo32,
                DIB_RGB_COLORS, &pbits32, NULL, 0);
            
            if (hbm32)
            {
                HBITMAP hbmOld32 = (HBITMAP)SelectObject(hdcMem, hbm32);
                
                // Create a DC just to copy the bitmap into the memory D
                HDC hdcTmp = CreateCompatibleDC(hdcMem);
                if (hdcTmp)
                {
                    // Get how many bytes per row we have for the bitmap bits
                    // (rounded up to 32 bits)
                    BITMAP bm32;
                    GetObject(hbm32, sizeof(bm32), &bm32);
                    while (bm32.bmWidthBytes % 4)
                    {
                        ++bm32.bmWidthBytes;
                    }
                    
#if defined(LS_COMPAT_TRANSPTOL)
                    // get the limits for the colors
                    BYTE clrHiR = (0xff - GetRValue(clrTolerance)) > GetRValue(clrTransp) ?
                        GetRValue(clrTransp) + GetRValue(clrTolerance) : 0xff;
                    BYTE clrHiG = (0xff - GetGValue(clrTolerance)) > GetGValue(clrTransp) ?
                        GetGValue(clrTransp) + GetGValue(clrTolerance) : 0xff;
                    BYTE clrHiB = (0xff - GetBValue(clrTolerance)) > GetBValue(clrTransp) ?
                        GetBValue(clrTransp) + GetBValue(clrTolerance) : 0xff;
                    
                    BYTE clrLoR = GetRValue(clrTolerance) < GetRValue(clrTransp) ?
                        GetRValue(clrTransp) - GetRValue(clrTolerance) : 0x00;
                    BYTE clrLoG = GetGValue(clrTolerance) < GetGValue(clrTransp) ?
                        GetGValue(clrTransp) - GetGValue(clrTolerance) : 0x00;
                    BYTE clrLoB = GetBValue(clrTolerance) < GetBValue(clrTransp) ?
                        GetBValue(clrTransp) - GetBValue(clrTolerance) : 0x00;
#endif // LS_COMPAT_TRANSPTOL
                    
                    // Copy the bitmap into the memory D
                    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcTmp, hbm);
                    
                    BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight,
                        hdcTmp, 0, 0, SRCCOPY);
                    
                    // Scan each bitmap row from bottom to top
                    // (the bitmap is inverted vertically)
#if defined(LS_COMPAT_TRANSPTOL)
                    BYTE *p;
#else
                    DWORD *p;
#endif // LS_COMPAT_TRANSPTOL
                    BYTE *p32 = (BYTE *)bm32.bmBits + \
                        (bm32.bmHeight - 1) * bm32.bmWidthBytes;
                    
                    int y = 0;
                    
                    while (y < bm.bmHeight)
                    {
                        int x = 0, x0;
                        
                        while (x < bm.bmWidth)
                        {
                            // loop through all transparent pixels...
                            while (x < bm.bmWidth)
                            {
#if defined(LS_COMPAT_TRANSPTOL)
                                p = p32 + 4 * x;
                                
                                // if the pixel is non-transparent, then break
                                if (*p < clrLoB || *p > clrHiB)
                                {
                                    break;
                                }
                                ++p;
                                if (*p < clrLoG || *p > clrHiG)
                                {
                                    break;
                                }
                                ++p;
                                if (*p < clrLoR || *p > clrHiR)
                                {
                                    break;
                                }
#else
                                p = (DWORD*)(p32 + 4 * x);
                                if (*p != 0xFF00FF)
                                {
                                    break;
                                }
#endif // LS_COMPAT_TRANSPTOL
                                
                                ++x;
                            }
                            
                            // save first non transparent pixel offset
                            x0 = x;
                            
                            // loop through all non transparent pixels
                            while (x < bm.bmWidth)
                            {
#if defined(LS_COMPAT_TRANSPTOL)
                                p = p32 + 4 * x;
                                
                                // if the pixel is transparent, then break
                                if (*p >= clrLoB && *p <= clrHiB)
                                {
                                    ++p;
                                    if (*p >= clrLoG && *p <= clrHiG)
                                    {
                                        ++p;
                                        if (*p >= clrLoR && *p <= clrHiR)
                                        {
                                            break;
                                        }
                                    }
                                }
#else
                                p = (DWORD*)(p32 + 4 * x);
                                if (*p == 0xFF00FF)
                                {
                                    break;
                                }
#endif // LS_COMPAT_TRANSPTOL
                                
                                ++x;
                            }
                            
                            // if found one or more non-transparent pixels in a
                            // row, add them to the rgn...
                            if ((x - x0) > 0)
                            {
                                HRGN hTempRgn = CreateRectRgn(
                                    x0 + xoffset, y + yoffset,
                                    x + xoffset, y + 1 + yoffset);
                                CombineRgn(hRgn, hRgn, hTempRgn, RGN_OR);
                                DeleteObject(hTempRgn);
                            }
                            
                            ++x;
                        }
                        
                        ++y;
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


//
// HBITMAP LoadLSImage(LPCSTR pszImage, LPCSTR pszFile)
//
// Takes strings of the form:
//   File.bmp
//   .extract
//   .extract=file.exe[,3]
HBITMAP LoadLSImage(LPCSTR pszImage, LPCSTR pszFile)
{
    HBITMAP hbmReturn = NULL;
    
    if (pszImage != NULL)
    {
        if (_stricmp(pszImage, ".none") != 0)
        {
            char szImage[MAX_PATH];
            StringCchCopy(szImage, MAX_PATH, pszImage);
            
            // Bitmap merging by Thedd
            //  Thedd - pic1.bmp|pic2.bmp merges the images. Works recursively,
            //  so pic1.bmp|.extract=whatever.dll,3|pic2.bmp also works etc...
            // bitmap merging by grd
            LPSTR pszSecondImage = strchr(szImage, '|');
            if (pszSecondImage)
            {
                HDC hdcFirst, hdcSecond, hdcResult;
                HBITMAP hbmFirst, hbmFirstOld;
                HBITMAP hbmSecond, hbmSecondOld;
                HBITMAP    hbmResult, hbmResultOld;
                HBRUSH hbrTransparent;
                RECT rc;
                int wdtFirst, hgtFirst;
                int wdtSecond, hgtSecond;
                int wdtResult, hgtResult;
                
                // get the position after the [|] character
                *pszSecondImage = '\0';
                ++pszSecondImage;
                
                // load the two bitmaps
                hbmFirst = LoadLSImage(szImage, pszFile);
                hbmSecond = LoadLSImage(pszSecondImage, pszFile);
                
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
                    
                    // in earlier version of bitmap merge, those were painted on
                    // to each other now let's paint both images to a new one
                    
                    // and we support different sized images!! therefore:
                    wdtResult = std::max(wdtFirst, wdtSecond);
                    hgtResult = std::max(hgtFirst, hgtSecond);
                    
                    // create another dc, compatible with second dc
                    hdcResult = CreateCompatibleDC(hdcSecond);
                    
                    // create a new bitmap for the new dc and select it
                    hbmResult = CreateCompatibleBitmap(hdcSecond,
                        wdtResult, hgtResult);
                    hbmResultOld = (HBITMAP)SelectObject(hdcResult, hbmResult);
                    
                    rc.top = 0;
                    rc.left = 0;
                    rc.right = wdtResult;
                    rc.bottom = hgtResult;
                    
                    // paint the background in transparent color...
                    hbrTransparent = CreateSolidBrush(RGB(255, 0, 255));
                    FillRect(hdcResult, &rc, hbrTransparent);
                    DeleteObject(hbrTransparent);
                    
                    // first "standard blit" the second image into the new one:
                    BitBlt(hdcResult, (wdtResult - wdtSecond) / 2,
                        (hgtResult - hgtSecond) / 2, wdtSecond, hgtSecond,
                        hdcSecond, 0, 0, SRCCOPY);
                    
                    // Secondly "tranparent blit" the first image over the
                    // second one Since TransparentBltLS double buffers the
                    // painting to reduce flicker and we are using only memory
                    // DC's in this function, we will just call
                    // TransparentBltLSWorker and shave off a few BitBlt calls
                    TransparentBltLSWorker(hdcResult, wdtFirst, hgtFirst,
                        hdcFirst, 0, 0, RGB(255, 0, 255));
                    
                    // deselect the bitmap from the dc and delete the dc to get
                    // the image
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
            else
            {
                if (!_strnicmp(szImage, ".extract", 8 /*strlen(".extract")*/))
                {
                    HICON hIcon = NULL;
                    
                    hIcon = LoadLSIcon(szImage, pszFile);
                    
                    if (hIcon)
                    {
                        hbmReturn = BitmapFromIcon(hIcon);
                        DestroyIcon(hIcon);
                    }
                }
                else
                {
                    // Append the image name to the LiteStep image path and
                    // attempt to load the image.
                    char szExpandedImage[MAX_PATH];
                    
                    VarExpansionEx(szExpandedImage, szImage, MAX_PATH);
                    LSGetImagePath(szImage, MAX_PATH);
                    PathAppend(szImage, szExpandedImage);
                    
                    if (PathMatchSpec(szImage, "*.png"))
                    {
                        hbmReturn = LoadFromPNG(szImage);
                    }
                    else
                    {
                        hbmReturn = (HBITMAP)LoadImage(
                            NULL, szImage, IMAGE_BITMAP, 0, 0,
                            LR_DEFAULTCOLOR | LR_LOADFROMFILE);
                    }
                    
                    // If that fails, treat the image as a fully qualified path
                    // and try loading it
                    if (hbmReturn == NULL)
                    {
                        if (PathMatchSpec(szExpandedImage, "*.png"))
                        {
                            hbmReturn = LoadFromPNG(szExpandedImage);
                        }
                        else
                        {
                            hbmReturn = (HBITMAP)LoadImage(
                                NULL, szExpandedImage, IMAGE_BITMAP, 0, 0,
                                LR_DEFAULTCOLOR | LR_LOADFROMFILE);
                        }
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
    
    if (GetIconInfo(hIcon, &infoIcon))
    {
        HDC hDC;
        HBITMAP hOldBMP;
        HBRUSH hBrush;
        
        hDC = CreateCompatibleDC(NULL);
        hOldBMP = (HBITMAP)SelectObject(hDC, infoIcon.hbmColor);
        hBrush = CreateSolidBrush(RGB (255, 0, 255));
        DrawIconEx(hDC, 0, 0, hIcon, 0, 0, 0, hBrush, DI_NORMAL);
        DeleteObject(hBrush);
        DeleteObject(infoIcon.hbmMask);
        SelectObject(hDC, hOldBMP);
        DeleteDC(hDC);
        
        return infoIcon.hbmColor;
    }
    
    return NULL;
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
    HICON hIcon = NULL;
    
    if (pszIconPath != NULL)
    {
        if (_stricmp(pszIconPath, ".none") != 0)
        {
            char szIconPath[MAX_PATH];
            char szIconLSImagePath[MAX_PATH];
            LPSTR pszIconFile = (LPSTR)pszIconPath;
            int nIcon = 0;
            
            // here comes a large block which does nothing but turning it into
            // the form <absolute path>[,<iconIndex>]
            
            // if .extract but nothing else is there...
            // then take the file specified as an icon (could probably be done
            // earlier, but anyhow)
            if (_stricmp(pszIconPath, ".extract") == 0)
            {
                pszIconFile = (LPSTR)pszFile;
            }
            else if (_strnicmp(pszIconPath, ".extract=", 9) == 0)
            {
                // remove ".extract=" (as we won't use it anyway)
                pszIconFile = (LPSTR)pszIconPath + 9;
            }
            
            VarExpansionEx(szIconPath, pszIconFile, MAX_PATH);
            
            if (PathIsRelative(szIconPath))
            {
                LSGetImagePath(szIconLSImagePath, MAX_PATH);
                PathAppend(szIconLSImagePath, szIconPath);
                pszIconFile = szIconLSImagePath;
            }
            else
            {
                pszIconFile = szIconPath;
            }
            
            // large block ends here, now time to separate path and index (if we
            // have an index)
            nIcon = PathParseIconLocation(pszIconFile);
            
            // now we have the two vars we would like, and the loading can begin
            // well not really, if it's a path, where we're going to get the
            // icon form desktop.ini there is just a little bit more we have to
            // do before we can start loading
            if (PathIsDirectory(pszIconFile))
            {
                char szTemp[MAX_PATH];
                
                PathAppend(pszIconFile, "desktop.ini");
                nIcon = GetPrivateProfileInt(".ShellClassInfo", "IconIndex",
                    0, pszIconFile);
                
                GetPrivateProfileString(".ShellClassInfo", "IconFile",
                    "", szTemp, MAX_PATH, pszIconFile);
                StringCchCopy(pszIconFile, MAX_PATH, szTemp);
            }
            
            // okay, now it's really time to load the icon... if it's an .ico
            // file we want to do an LoadImage() thing, otherwise it's
            // extracticon so lets find out the extension
            if (PathMatchSpec(pszIconFile, "*.ico"))
            {
                hIcon = (HICON)LoadImage(
                    NULL, pszIconFile, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
            }
            else
            {
                hIcon = ExtractIcon(GetModuleHandle(NULL), pszIconFile, nIcon);
                
                if (hIcon == (HICON)1)
                {
                    hIcon = NULL;
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


void TransparentBltLS(
    HDC hdcDst, int nXDest, int nYDest, int nWidth, int nHeight,
    HDC hdcSrc, int nXSrc, int nYSrc,
    COLORREF colorTransparent)
{
    HDC hdcDstCpy;
    HBITMAP hbmDstCpy;
    HBITMAP hbmOldDstCpy;
    
    // create a destination compatble dc containing
    // a copy of the destination dc
    hdcDstCpy = CreateCompatibleDC(hdcDst);
    hbmDstCpy = CreateCompatibleBitmap(hdcDst, nWidth, nHeight);
    hbmOldDstCpy = (HBITMAP)SelectObject(hdcDstCpy, hbmDstCpy);
    
    BitBlt(hdcDstCpy, 0, 0, nWidth, nHeight, hdcDst, nXDest, nYDest, SRCCOPY);
    
    TransparentBltLSWorker(hdcDstCpy, nWidth, nHeight,
        hdcSrc, nXSrc, nYSrc, colorTransparent);
    
    // now we have created the image we want to blt
    // in the destination copy dc
    BitBlt(hdcDst, nXDest, nYDest, nWidth, nHeight, hdcDstCpy, 0, 0, SRCCOPY);
    
    SelectObject(hdcDstCpy, hbmOldDstCpy);
    DeleteObject(hbmDstCpy);
    DeleteDC(hdcDstCpy);
}


static void TransparentBltLSWorker(
    HDC hdcDst, int nWidth, int nHeight,
    HDC hdcSrc, int nXSrc, int nYSrc,
    COLORREF colorTransparent)
{
    HDC hdcMem, hdcMask;
    HBITMAP hbmMask, hbmMem;
    HBITMAP hbmOldMem, hbmOldMask;
    
    // create a destination compatble dc containing a copy of the source dc
    hdcMem = CreateCompatibleDC(hdcDst);
    hbmMem = CreateCompatibleBitmap(hdcDst, nWidth, nHeight);
    hbmOldMem = (HBITMAP)SelectObject(hdcMem, hbmMem);
    
    BitBlt(hdcMem, 0, 0, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, SRCCOPY);
    
    // Create monochrome bitmap for the mask
    hdcMask = CreateCompatibleDC(hdcDst);
    hbmMask = CreateBitmap(nWidth, nHeight, 1, 1, NULL);
    hbmOldMask = (HBITMAP)SelectObject(hdcMask, hbmMask);
    
    // pixels matching the hdcMem bkcolor are white in the mask,
    // while all others are black.
    SetBkColor(hdcMem, colorTransparent);
    // Create the mask from the memory dc holding the source image
    BitBlt(hdcMask, 0, 0, nWidth, nHeight, hdcMem, 0, 0, SRCCOPY);
    
    // Mask off source (XOR - hdcMem bkcolor is still the transparent color)
    BitBlt(hdcMem, 0, 0, nWidth, nHeight, hdcMask, 0, 0, SRCINVERT);
    // Mask off dest (assumption is made that hdcDest bkcolor is white)
    BitBlt(hdcDst, 0, 0, nWidth, nHeight, hdcMask, 0, 0, SRCAND);
    
    // Combine the foreground with the background
    BitBlt(hdcDst, 0, 0, nWidth, nHeight, hdcMem, 0, 0, SRCPAINT);
    
    SelectObject(hdcMask, hbmOldMask);
    DeleteObject(hbmMask);
    DeleteDC(hdcMask);
    
    SelectObject(hdcMem, hbmOldMem);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
}


void Frame3D(
    HDC hdc, RECT rect, COLORREF TopColor, COLORREF BottomColor, int Width)
{
    HPEN hPen1, hPen2, hOldPen;
    
    --rect.bottom;
    --rect.right;
    
    hPen1 = CreatePen(PS_SOLID, 1, TopColor);
    hPen2 = CreatePen(PS_SOLID, 1, BottomColor);
    hOldPen = (HPEN)SelectObject(hdc, hPen1);
    
    while (Width-- > 0)
    {
        POINT points[3];
        
        // left, top borders (do not draw corners)
        points[0].x = rect.left;
        points[0].y = rect.bottom-1;
        points[1].x = rect.left;
        points[1].y = rect.top;
        points[2].x = rect.right; // end point is not filled
        points[2].y = rect.top;
        
        SelectObject(hdc, hPen1);
        Polyline(hdc, points, 3);
        
        // bottom, right borders (draw corners)
        points[0].y = rect.bottom;
        points[1].x = rect.right;
        points[1].y = rect.bottom;
        points[2].y -= 1; // end point is not filled
        
        SelectObject(hdc, hPen2);
        Polyline(hdc, points, 3);
        
        InflateRect(&rect, -1, -1);
    }
    
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen2);
    DeleteObject(hPen1);
}

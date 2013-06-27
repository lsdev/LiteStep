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
#include "png_support.h"
#include "../utility/debug.hpp"

#if defined(LS_USE_PICOPNG)

#include "picopng.h"
#include <vector>
#include <fstream>

//designed for loading files from hard disk in an std::vector
void loadFile(std::vector<unsigned char>& buffer, const std::string& filename)
{
    std::ifstream file(filename.c_str(),
        std::ios::in|std::ios::binary|std::ios::ate);
    
    if (!file)
    {
        return;
    }
    
    //get file size
    std::streamsize size = 0;
    
    if (file.seekg(0, std::ios::end).good())
    {
        size = file.tellg();
    }
    
    if (file.seekg(0, std::ios::beg).good())
    {
        size -= file.tellg();
    }
    
    //read contents of the file into the vector
    buffer.resize(static_cast<size_t>(size));
    
    if (size > 0)
    {
        file.read(reinterpret_cast<char*>(&buffer[0]), size);
    }
}

HBITMAP LoadFromPNG(LPCSTR pszFilename)
{
    if (pszFilename == NULL)
    {
        return NULL;
    }
    
    //load and decode
    std::vector<unsigned char> buffer, image, palette;
    
    loadFile(buffer, pszFilename);
    
    if (buffer.empty())
    {
        return NULL;
    }
    
    unsigned long w, h;
    int error = decodePNG(image, w, h, &buffer[0],
        static_cast<unsigned long>(buffer.size()));
    
    if (error != 0)
    {
        return NULL;
    }
    
    if (image.size() <= 4)
    {
        return NULL;
    }
    
    BITMAPINFO bmi = { {0} };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = h;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biCompression = BI_RGB;
    
    unsigned char* bits;
    std::vector<unsigned char> bgra;
    
    HBITMAP hDibSection = CreateDIBSection(
        NULL, &bmi, 0, reinterpret_cast<LPVOID*>(&bits), NULL, 0);
    
    for (int i = static_cast<int>(image.size()) - w*4; i >= 0; i -= w*4)
    {
        for (unsigned long j = 0; j < w*4; j += 4)
        {
            bgra.push_back(image[i + j + 2]); // blue
            bgra.push_back(image[i + j + 1]); // green
            bgra.push_back(image[i + j]);     // red
            bgra.push_back(image[i + j + 3]); // alpha
        }
    }
    
    memcpy(bits, &(bgra[0]), bgra.size());
    
    return hDibSection;
}

#else // LS_USE_PICOPNG

#include <png.h>
#include <stdlib.h>

typedef struct _PNGERROR
{
    HWND Wnd;
    jmp_buf ErrorJump;
    //
} PNGERROR, *PPNGERROR;

void PNGErrorHandler(png_structp PngStruct, png_const_charp Message)
{
    PPNGERROR PngError = \
        reinterpret_cast<PPNGERROR>(png_get_error_ptr(PngStruct));
    
    if (!PngError)
    {
        throw Message;
    }
    else
    {
        MessageBox(PngError->Wnd, Message, "LiteStep::LoadFromPNG:PngError",
            MB_OK | MB_TOPMOST | MB_ICONERROR);
        
        longjmp(PngError->ErrorJump, 1);
    }
}

png_voidp ls_png_malloc(png_structp /* png_ptr */, png_size_t size)
{
    return malloc(size);
}

void ls_png_free(png_structp /* png_ptr */, png_voidp ptr)
{
    free(ptr);
}

void ls_png_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
    FILE* file = (FILE*)png_get_io_ptr(png_ptr);
    ASSERT(file); // if libpng calls this function, this mustn't be empty
    
    fread(data, 1, length, file);
}

//
// In order to avoid CRT mismatches, we override all CRT functions libpng uses
// with our own. The alternative, using fopen and fread in our code and then
// passing a FILE* into libpng, isn't reliable because it crashes if libpng was
// compiled with a different CRT than lsapi.dll
//
HBITMAP LoadFromPNG(LPCSTR pszFilename)
{
    HBITMAP hDibSection = NULL;
    
    if (pszFilename != NULL)
    {
        FILE * hFile = fopen(pszFilename, "rb");
        
        if (hFile)
        {
            const size_t num_sig_bytes = 8;
            unsigned char sig[num_sig_bytes];
            
            fread(sig, 1, num_sig_bytes, hFile);
            
            if (png_check_sig(sig, num_sig_bytes))
            {
                PNGERROR PngError = { 0 };
                
                PngError.Wnd = GetForegroundWindow();
                
                png_structp Read = png_create_read_struct_2(
                    PNG_LIBPNG_VER_STRING, &PngError, PNGErrorHandler, NULL,
                    NULL, ls_png_malloc, ls_png_free);
                
                if (Read)
                {
                    png_infop Info = png_create_info_struct(Read);
                    
                    if (Info)
                    {
                        if (!setjmp(PngError.ErrorJump))
                        {
                            png_set_read_fn(Read, hFile, ls_png_read_data);
                            png_set_sig_bytes(Read, num_sig_bytes);
                            png_read_info(Read, Info);
                            
                            const unsigned char bit_depth = png_get_bit_depth(Read, Info);
                            const unsigned char color_type = png_get_color_type(Read, Info);
                            
                            if (color_type == PNG_COLOR_TYPE_PALETTE)
                            {
                                png_set_palette_to_rgb(Read);
                            }
                            
                            if ((color_type == PNG_COLOR_TYPE_GRAY) ||
                                (color_type == PNG_COLOR_TYPE_GRAY_ALPHA))
                            {
                                if (bit_depth < 8)
                                {
                                    png_set_gray_1_2_4_to_8(Read);
                                }
                                png_set_gray_to_rgb(Read);
                            }
                            
                            if (png_get_valid(Read, Info, PNG_INFO_tRNS))
                            {
                                png_set_tRNS_to_alpha(Read);
                            }
                            
                            if (color_type & PNG_COLOR_MASK_COLOR)
                            {
                                png_set_bgr(Read);
                            }
                            
                            if (bit_depth == 16)
                            {
                                png_set_strip_16(Read);
                            }
                            
                            double image_gamma = 1 / 2.2;
                            png_get_gAMA(Read, Info, &image_gamma);
                            png_set_gamma(Read, 2.2, image_gamma);
                            
                            const int num_passes = png_set_interlace_handling(Read);
                            
                            png_read_update_info(Read, Info);
                            
                            BITMAPINFO bmi = { {0} };
                            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                            bmi.bmiHeader.biWidth = (LONG)png_get_image_width(Read, Info);
                            bmi.bmiHeader.biHeight = -(LONG)png_get_image_height(Read, Info);
                            bmi.bmiHeader.biBitCount = (WORD)(png_get_channels(Read, Info) * png_get_bit_depth(Read, Info));
                            bmi.bmiHeader.biPlanes = 1;
                            bmi.bmiHeader.biCompression = BI_RGB;
                            
                            unsigned char* bits;
                            hDibSection = CreateDIBSection(NULL, &bmi, 0,
                                reinterpret_cast<LPVOID*>(&bits), NULL, 0);
                            
                            if (!bits)
                            {
                                longjmp(PngError.ErrorJump, 1);
                            }
                            
                            unsigned int dib_bytes_per_scanline = \
                                (((bmi.bmiHeader.biWidth * bmi.bmiHeader.biBitCount) + 31) & ~31) >> 3;
                            
                            for (int Pass = 0; Pass < num_passes; ++Pass)
                            {
                                for (int y = 0; y < -bmi.bmiHeader.biHeight; ++y)
                                {
                                    unsigned char* Scanline = \
                                        reinterpret_cast<unsigned char*>(bits + (y * dib_bytes_per_scanline));
                                    
                                    png_read_row(Read, Scanline, NULL);
                                }
                            }
                            
                            png_read_end(Read, Info);
                            png_destroy_read_struct(&Read, &Info, NULL);
                        }
                        else
                        {
                            png_destroy_read_struct(&Read, &Info, NULL);
                        }
                    }
                    else
                    {
                        png_destroy_read_struct(&Read, NULL, NULL);
                    }
                }
            }
            
            fclose(hFile);
        }
    }
    
    return hDibSection;
}

#endif // LS_USE_PICOPNG

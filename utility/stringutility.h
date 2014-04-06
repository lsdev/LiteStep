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
#if !defined(STRINGUTILITY_H)
#define STRINGUTILITY_H

#include "common.h"

#include <map>
#include <memory>
#include <string>
#include <string.h> // for _wcsicmp/wcscmp
#include <unordered_map>


//
// stringicmp
//
// provides case insensitive string comparison for std::map
//
struct stringicmp
{
    bool operator()(const std::wstring &s1, const std::wstring &s2) const
    {
        return (_wcsicmp(s1.c_str(), s2.c_str()) < 0);
    }
};


//
// stringcmp
//
// provides case string comparison for std::map
//
struct stringcmp
{
    bool operator()(const std::wstring &s1, const std::wstring &s2) const
    {
        return (wcscmp(s1.c_str(), s2.c_str()) < 0);
    }
};


//
// CStrings
//
// Standard containers using C-style strings as keys.
//
namespace CStrings
{
    struct CaseSensitive
    {
        struct Hash
        {
            size_t operator()(LPCWSTR str) const
            {
#if defined(_WIN64)
                static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
                const size_t _FNV_offset_basis = 14695981039346656037ULL;
                const size_t _FNV_prime = 1099511628211ULL;
#else
                static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
                const size_t _FNV_offset_basis = 2166136261U;
                const size_t _FNV_prime = 16777619U;
#endif
                size_t value = _FNV_offset_basis;
                for (LPCWSTR chr = str; *chr != 0; ++chr)
                {
                    value ^= (size_t)*chr;
                    value *= _FNV_prime;
                }

#if defined(_WIN64)
                static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
                value ^= value >> 32;
#else
                static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
#endif
                return value;
            }
        };

        struct Compare
        {
            bool operator()(LPCWSTR a, LPCWSTR b) const
            {
                return wcscmp(a, b) < 0;
            }
        };

        struct Equal
        {
            bool operator()(LPCWSTR a, LPCWSTR b) const
            {
                return wcscmp(a, b) == 0;
            }
        };

        template <
            typename Type,
            typename Allocator = std::allocator<pair<const LPCWSTR, Type>>
        >
        using Map = std::map<LPCWSTR, Type, Compare, Allocator>;

        template <
            typename Type,
            typename Allocator = std::allocator<pair<const LPCWSTR, Type>>
        >
        using ConstMap = const Map<Type, Allocator>;

        template <
            typename Type,
            typename Allocator = std::allocator<pair<const LPCWSTR, Type>>
        >
        using UnorderedMap = std::unordered_map<LPCWSTR, Type, Hash, Equal, Allocator>;

        template <
            typename Type,
            typename Allocator = std::allocator<pair<const LPCWSTR, Type>>
        >
        using ConstUnorderedMap = const UnorderedMap<Type, Allocator>;
    };

    struct CaseInsensitive
    {
        struct Hash
        {
            size_t operator()(LPCWSTR str) const
            {
#if defined(_WIN64)
                static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
                const size_t _FNV_offset_basis = 14695981039346656037ULL;
                const size_t _FNV_prime = 1099511628211ULL;
#else
                static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
                const size_t _FNV_offset_basis = 2166136261U;
                const size_t _FNV_prime = 16777619U;
#endif
                size_t value = _FNV_offset_basis;
                for (LPCWSTR chr = str; *chr != 0; ++chr)
                {
                    value ^= (size_t)towlower(*chr);
                    value *= _FNV_prime;
                }

#if defined(_WIN64)
                static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
                value ^= value >> 32;
#else
                static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
#endif
                return value;
            }
        };

        struct Compare
        {
            bool operator()(LPCWSTR a, LPCWSTR b) const
            {
                return _wcsicmp(a, b) < 0;
            }
        };

        struct Equal
        {
            bool operator()(LPCWSTR a, LPCWSTR b) const
            {
                return _wcsicmp(a, b) == 0;
            }
        };

        template <
            typename Type,
            typename Allocator = std::allocator<pair<const LPCWSTR, Type>>
        >
        using Map = std::map<LPCWSTR, Type, Compare, Allocator>;

        template <
            typename Type,
            typename Allocator = std::allocator<pair<const LPCWSTR, Type>>
        >
        using ConstMap = const Map<Type, Allocator>;

        template <
            typename Type,
            typename Allocator = std::allocator<pair<const LPCWSTR, Type>>
        >
        using UnorderedMap = std::unordered_map<LPCWSTR, Type, Hash, Equal, Allocator>;

        template <
            typename Type,
            typename Allocator = std::allocator<pair<const LPCWSTR, Type>>
        >
        using ConstUnorderedMap = const UnorderedMap<Type, Allocator>;
    };
}


//
// Quick conversion
//
wchar_t *WCSFromMBS(const char *pszMBS);
char *MBSFromWCS(const wchar_t *pwzWCS);

#define WCSTOMBS(str) std::unique_ptr<char>(MBSFromWCS(str)).get()
#define MBSTOWCS(str) std::unique_ptr<wchar_t>(WCSFromMBS(str)).get()

#endif // STRINGUTILITY_H

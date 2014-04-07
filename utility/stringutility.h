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
#include <set>
#include <string>
#include <string.h> // for _wcsicmp/wcscmp
#include <unordered_map>
#include <unordered_set>


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
// CaseSensitive
//
// Case sensitive functions for StringKeyedContainers
//
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

        size_t operator()(const std::wstring & str) const
        {
            return operator()(str.c_str());
        }
    };

    struct Compare
    {
        bool operator()(LPCWSTR a, LPCWSTR b) const
        {
            return wcscmp(a, b) < 0;
        }

        bool operator()(const std::wstring & a, const std::wstring & b) const
        {
            return wcscmp(a.c_str(), b.c_str()) < 0;
        }
    };

    struct Equal
    {
        bool operator()(LPCWSTR a, LPCWSTR b) const
        {
            return wcscmp(a, b) == 0;
        }

        bool operator()(const std::wstring & a, const std::wstring & b) const
        {
            return wcscmp(a.c_str(), b.c_str()) == 0;
        }
    };
};


//
// CaseInsensitive
//
// Case insensitive functions for StringKeyedContainers
//
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

        size_t operator()(const std::wstring & str) const
        {
            return operator()(str.c_str());
        }
    };

    struct Compare
    {
        bool operator()(LPCSTR a, LPCSTR b) const
        {
            return _stricmp(a, b) < 0;
        }

        bool operator()(LPCWSTR a, LPCWSTR b) const
        {
            return _wcsicmp(a, b) < 0;
        }

        bool operator()(const std::string & a, const std::string & b) const
        {
            return _stricmp(a.c_str(), b.c_str()) < 0;
        }

        bool operator()(const std::wstring & a, const std::wstring & b) const
        {
            return _wcsicmp(a.c_str(), b.c_str()) < 0;
        }
    };

    struct Equal
    {
        bool operator()(LPCSTR a, LPCSTR b) const
        {
            return _stricmp(a, b) == 0;
        }

        bool operator()(LPCWSTR a, LPCWSTR b) const
        {
            return _wcsicmp(a, b) == 0;
        }

        bool operator()(const std::string & a, const std::string & b) const
        {
            return _stricmp(a.c_str(), b.c_str()) == 0;
        }

        bool operator()(const std::wstring & a, const std::wstring & b) const
        {
            return _wcsicmp(a.c_str(), b.c_str()) == 0;
        }
    };
};


//
// StringKeyedMaps
//
// Standard maps using strings as keys.
//
template <
    typename KeyType,
    typename Type,
    typename KeyOperators = CaseInsensitive,
    typename Allocator = std::allocator<std::pair<std::add_const<KeyType>, Type>>
>
struct StringKeyedMaps
{
    using Map = std::map<KeyType, Type, typename KeyOperators::Compare, Allocator>;
    using ConstMap = const Map;
    using MultiMap = std::multimap<KeyType, Type, typename KeyOperators::Compare, Allocator>;
    using ConstMultiMap = const MultiMap;
    using UnorderedMap = std::unordered_map<KeyType, Type, typename KeyOperators::Hash, typename KeyOperators::Equal, Allocator>;
    using ConstUnorderedMap = const UnorderedMap;
    using UnorderedMultiMap = std::unordered_multimap<KeyType, Type, typename KeyOperators::Hash, typename KeyOperators::Equal, Allocator>;
    using ConstUnorderedMultiMap = const UnorderedMultiMap;
};


//
// StringKeyedSets
//
// Standard sets using strings as keys.
//
template <
    typename Type,
    typename Operators = CaseInsensitive,
    typename Allocator = std::allocator<std::pair<std::add_const<Type>, Type>>
>
struct StringKeyedSets
{
    using Set = std::set<Type, typename Operators::Compare, Allocator>;
    using ConstSet = const Set;
    using MultiSet = std::multiset<Type, typename Operators::Compare, Allocator>;
    using ConstMultiSet = const MultiSet;
    using UnorderedSet = std::unordered_set<Type, typename Operators::Hash, typename Operators::Equal, Allocator>;
    using ConstUnorderedSet = const UnorderedSet;
    using UnorderedMultiSet = std::unordered_multiset<Type, typename Operators::Hash, typename Operators::Equal, Allocator>;
    using ConstUnorderedMultiSet = const UnorderedMultiSet;
};


//
// Quick conversion
//
wchar_t *WCSFromMBS(const char *pszMBS);
char *MBSFromWCS(const wchar_t *pwzWCS);

#define WCSTOMBS(str) std::unique_ptr<char>(MBSFromWCS(str)).get()
#define MBSTOWCS(str) std::unique_ptr<wchar_t>(WCSFromMBS(str)).get()

#endif // STRINGUTILITY_H

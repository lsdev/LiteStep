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
#if !defined(SETTINGSITERATOR_H)
#define SETTINGSITERATOR_H

#include "settingsdefines.h"
#include "../utility/common.h"


/**
 * Iterator for settings with more than one value.
 */
class SettingsIterator
{
public:
    /**
     * Constructs a SettingsIterator for the given SettingsMap and associated
     * with the given file.
     *
     * @param  pSettingsMap  SettingsMap to iterate over
     * @param  sPath        Path to configuration file
     */
    SettingsIterator(SettingsMap* pSettingsMap, const std::wstring& sPath);
    
    /**
     * Retrieve the next value.
     *
     * @param   pwzValue  buffer to receive value
     * @param   cchValue  size of buffer
     * @return  <code>TRUE</code> if operation succeeded or <code>FALSE</code>
     *          if there are no more values to retrieve
     */
    BOOL ReadNextLine(LPWSTR pwzValue, size_t cchValue);
    
    /**
     * Retrieve the next value that does not start with an '*'.
     *
     * @param   pwzValue  buffer to receive value
     * @param   cchValue  size of buffer
     * @return  <code>TRUE</code> if operation succeeded or <code>FALSE</code>
     *          if there are no more values to retrieve
     */
    BOOL ReadNextCommand(LPWSTR pwzValue, size_t cchValue);
    
    /**
     * Retrieve the next value with the specified setting name that starts
     * with an '*'.
     *
     * @param   pwzConfig  setting name
     * @param   pwzValue   buffer to receive value
     * @param   cchValue   size of buffer
     * @return  <code>TRUE</code> if operation succeeded or <code>FALSE</code>
     *          if there are no more values to retrieve
     */
    BOOL ReadNextConfig(LPCWSTR pwzConfig, LPWSTR pszValue, size_t cchValue);
    
    /**
     * Returns path to configuration file.
     */
    std::wstring getPath()
    {
        return m_sPath;
    }
    
private:
    /** Settings map to iterate */
    SettingsMap* m_pSettingsMap;
    
    /** Iterator for LCReadNextLine */
    SettingsMap::iterator m_pFileIterator;
    
    /** Iterators for each setting name used with LCReadNextConfig. */
    IteratorMap m_Iterators;
    
    /** Path to configuration file */
    std::wstring m_sPath;
};


#endif // SETTINGSITERATOR_H

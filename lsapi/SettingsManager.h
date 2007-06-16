/*
This is a part of the LiteStep Shell Source code.

Copyright (C) 1997-2006 The LiteStep Development Team

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

/**
 * @file
 */

#ifndef __SETTINGSMANAGER_H
#define __SETTINGSMANAGER_H

#include "../utility/common.h"
#include "../utility/criticalsection.h"
#include <string>
#include <map>
#include <set>
#include "settingsdefines.h"
#include "settingsiterator.h"


/**
 * Information about an open configuration file.
 */
struct FileInfo
{
    /** Local settings map for this file */
    SettingsMap* m_pSettingsMap;
    
    /** Reference count */
    UINT m_Count;
};


/** Set of SettingsIterators. */
typedef std::set<SettingsIterator*> IteratorSet;

/** Maps file names to FileInfo structures. */
typedef std::map<std::string, FileInfo*, stringicmp> FileMap;


/**
 * Manages the global settings list and module-specific settings. The global settings list
 * includes all the settings specified in <code>step.rc</code> and included files.
 * Modules can also read their own private settings using {@link #LCOpen} and related
 * functions.
 */
class SettingsManager
{
    /** Iterators for doing LCReadNextConfig/Line */
    IteratorSet m_Iterators;
    
    /** Global settings map */
    SettingsMap m_SettingsMap;
    
    /** Files opened through LCOpen */
    FileMap m_FileMap;
    
    /** Critical section for serializing access to members */
    CriticalSection m_CritSection;

protected:

    /**
     * Searches for a global setting by name.
     *
     * @param   pszName  setting name
     * @param   it       iterator to be set to point to setting
     * @return  <code>TRUE</code> if the setting exists or <code>FALSE</code> otherwise
     */
    BOOL _FindLine(LPCSTR pszName, SettingsMap::iterator &it);
    
    /**
     * Assigns values to predefined settings.
     *
     * @param  szLSPath  path to Litestep directory
     */
    void _SetupVars(LPCSTR szLSPath);
    
    /**
     * Sets the value of a setting to the path of a special shell folder.
     *
     * @param   pszVariable  setting name
     * @param   nFolder      one of the <code>CSIDL_*</code> constants
     * @return  <code>true</code> if the operation succeeded or <code>false</code> otherwise
     */
    bool _SetShellFolderVariable(LPCSTR pszVariable, int nFolder);

public:

    /**
     * Constructor.
     *
     * @param  pszLiteStepPath  path to Litestep directory
     */
    SettingsManager(LPCSTR pszLiteStepPath);
    
    /**
     * Destructor.
     */
    ~SettingsManager();
    
    /**
     * Parses a configuration file and adds its contents to the global settings.
     *
     * @param  pszFileName  path to configuration file
     */
    void ParseFile(LPCSTR pszFileName);
    
    /**
     * Retrieves a Boolean value from the global settings. Returns <code>fIfFound</code>
     * if the setting exists and <code>!fIfFound</code> if it does not.
     *
     * @param   pszKeyName  setting name
     * @param   fIfFound    value to return if setting is found
     * @return  setting value
     */
    BOOL GetRCBool(LPCSTR pszKeyName, BOOL fIfFound);
    
    /**
     * Retrieves a Boolean value from the global settings. Returns <code>fDefault</code>
     * if the setting does not exist.
     *
     * @param   pszKeyName setting name
     * @param   fDefault   default value
     * @return  setting value
     */
    BOOL GetRCBoolDef(LPCSTR pszKeyName, BOOL fDefault);
    
    /**
     * Retrieves a color value from the global settings. Returns <code>crDefault</code>
     * if the setting does not exist.
     *
     * @param   pszKeyName  setting name
     * @param   crDefault   default value
     * @return  setting value
     */
    COLORREF GetRCColor(LPCSTR pszKeyName, COLORREF crDefault);
    
    /**
     * Retrieves an integer value from the global settings. Returns <code>nDefault</code>
     * if the setting does not exist.
     *
     * @param   pszKeyName  setting name
     * @param   nDefault    default value
     * @return  setting value
     */
    int GetRCInt(LPCSTR pszKeyName, int nDefault);
    
    /**
     * Retrieves a raw string value from the global settings. If setting does not exist,
     * copies <code>pszDefault</code> into the buffer and returns <code>FALSE</code>.
     * <code>pszDefault</code> may be <code>NULL</code> in which case nothing is copied
     * into the buffer.
     *
     * <code>GetRCLine</code> does not parse the string value like <code>GetRCString</code>
     * does. Its purpose is for retreiving command lines for applications and bang commands.
     *
     * @param   pszKeyName    setting name
     * @param   pszBuffer     buffer to receive value
     * @param   cchBufferLen  size of buffer
     * @param   pszDefault    default value
     * @return  <code>TRUE</code> if setting exists or <code>FALSE</code> otherwise
     */
    BOOL GetRCLine(LPCSTR pszKeyName, LPSTR pszBuffer, int cchBufferLen, LPCSTR pszDefault);
    
    /**
     * Retreives a string value from the global settings. If the setting does not exist,
     * copies <code>pszDefault</code> into the buffer and returns <code>FALSE</code>.
     * <code>pszDefault</code> may be <code>NULL</code> in which case nothing is copied
     * into the buffer.
     *
     * The raw setting value is parsed and the first space-delimited token or quoted string
     * is the value returned.
     *
     * @param   pszKeyName    setting name
     * @param   pszBuffer     buffer to receive value
     * @param   pszDefault    default value
     * @param   cchBufferLen  size of buffer
     * @return  <code>TRUE</code> if setting exists or <code>FALSE</code> otherwise
     */
    BOOL GetRCString(LPCSTR pszKeyName, LPSTR pszBuffer, LPCSTR pszDefault, int cchBufferLen);
    
    /**
     * Retrieves a string value from the global settings. Returns <code>FALSE</code> if the
     * setting does not exist. Performs the same operation as {@link #GetRCString}.
     *
     * @param   pszKeyName    setting name
     * @param   pszBuffer     buffer to receive value
     * @param   cchBufferLen  size of buffer
     * @return  <code>TRUE</code> if setting exists or <code>FALSE</code> otherwise
     */
    BOOL GetVariable(LPCSTR pszKeyName, LPSTR pszBuffer, DWORD cchBufferLen);
    
    /**
     * Assigns a new value to a global setting. If the setting already exists its previous
     * value is overwritten, otherwise a new setting is created.
     *
     * @param  pszKeyName  setting name
     * @param  pszValue    new setting value
     */
    void SetVariable(LPCSTR pszKeyName, LPCSTR pszValue);
    
    /**
     * Opens a configuration file for sequential access to its contents. The file contents
     * can be read with {@link #LCReadNextConfig} or {@link #LCReadNextLineOrCommand}. The
     * file should be closed with a call to {@link #LCClose} when it is no longer needed.
     *
     * If the pszPath argument is <code>NULL</code>, opens the global settings for sequential
     * access.
     *
     * Returns an opaque file handle. For historic reasons, the type of the return value is
     * a <code>FILE *</code>, but the value itself is <em>not</em> a <code>FILE *</code> and
     * must not be used as such.
     *
     * @param   pszPath  path to configuration file or <code>NULL</code> to access global settings
     * @return  file handle or <code>NULL</code> if the file could not be opened
     */
    FILE* LCOpen (LPCSTR pszPath);
    
    /**
     * Closes a configuration file opened with {@link #LCOpen}.
     *
     * @param   pFile  file handle returned by LCOpen
     * @return  <code>TRUE</code> if the operation succeeded or <code>FALSE</code> otherwise
     */
    BOOL LCClose (FILE *pFile);
    
    /**
     * Retrieves the next line that begins with the specified setting name from a configuration
     * file. The entire line (including the setting name) is placed in the buffer.
     *
     * Call this function repeatedly until it returns <code>FALSE</code> to retrieve all
     * the lines that begin with the specified setting name.
     *
     * @param   pFile         file handle returned by LCOpen
     * @param   pszConfig     setting name
     * @param   pszBuffer     buffer to receive line
     * @param   cchBufferLen  size of buffer
     * @return  <code>TRUE</code> if the line was retrieved or <code>FALSE</code> if there are
     *          no more lines or an error occurred
     */
    BOOL LCReadNextConfig(FILE *pFile, LPCSTR pszConfig, LPSTR pszBuffer, size_t cchBufferLen);
    
    /**
     * Retrieves the next line from a configuration file. The entire line (including the
     * setting name) is placed in the buffer.
     *
     * Call this function repeatedly until it returns <code>FALSE</code> to retrieve all
     * the lines in the file.
     *
     * @param   pFile         file handle returned by LCOpen
     * @param   pszBuffer     buffer to receive line
     * @param   cchBufferLen  size of buffer
     * @return  <code>TRUE</code> if the line was retrieved or <code>FALSE</code> if there are
     *          no more lines or an error occurred
     */
    BOOL LCReadNextLineOrCommand(FILE *pFile, LPSTR pszBuffer, size_t cchBufferLen);
    
    /**
     * Expands variable references. The template string is copied into the buffer with all
     * variable references (<code>$var$</code>) replaced by the value of the variable.
     *
     * @param  pszBuffer     buffer to received the expanded string
     * @param  pszTemplate   string to be expanded
     * @param  cchBufferLen  size of the buffer
     */
    void VarExpansionEx(LPSTR pszBuffer, LPCSTR pszTemplate, size_t cchBufferLen);
};

#endif // __SETTINGSMANAGER_H

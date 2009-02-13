//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2009  LiteStep Development Team
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
#if !defined(SETTINGSFILEPARSER_H)
#define SETTINGSFILEPARSER_H

#include "settingsdefines.h"
#include "lsapidefines.h"


/**
 * Parses configuration files.
 */
class FileParser //: boost::noncopyable
{
public:
    /**
     * Constructor.
     *
     * @param  pSettingsMap  SettingsMap to receive settings from files
     */
    FileParser(SettingsMap* pSettingsMap);
    
    /**
     * Destructor.
     */
    ~FileParser();
    
    /**
     * Parses a configuration file. Settings read from the file are added to
     * the SettingsMap object passed in to the FileParser constructor.
     *
     * @param  ptzFileName  path to file
     */
    void ParseFile(LPCTSTR ptzFileName);
    
private:
    /** Settings map to receive settings read from file */
    SettingsMap* m_pSettingsMap;
    
    /** Handle to current file */
    FILE* m_phFile;
    
    /** Current Line Number */
    unsigned int m_uLineNumber;
    
    /** Full path to configuration file */
    TCHAR m_tzFullPath[MAX_PATH_LENGTH];
    
    /**
     * Reads the next line from current file. The line is split into a setting
     * name and a setting value and the value is stripped of extraneous space
     * and comments.
     *
     * @param  ptzName   buffer to receive setting name
     * @param  ptzValue  buffer to receive setting value
     * @return <code>true</code> if operation succeeded or <code>false</code>
     *         if end of file was reached or an I/O error occurred.
     */
    bool _ReadLineFromFile(LPTSTR ptzName, LPTSTR ptzValue);
    
    /**
     * Strips leading and trailing whitespace and comments from a string. The
     * string is modified in place.
     */
    void _StripString(LPTSTR ptzString);
    
    /**
     * Processes a line read from a file. If the line is a preprocessor
     * directive then it is handled appropriately, otherwise it is added to the
     * SettingsMap object.
     *
     * @param  ptzName   setting name
     * @param  ptzValue  setting value
     */
    void _ProcessLine(LPCTSTR ptzName, LPCTSTR ptzValue);
    
    /**
     * Processes an 'If' preprocessor directive.
     *
     * @param  ptzExpression  conditional expression
     */
    void _ProcessIf(LPCTSTR ptzExpression);
    
    /**
     * Recursively skips over an 'If' preprocessor directive.
     */
    void _SkipIf();
};


#endif // SETTINGSFILEPARSER_H

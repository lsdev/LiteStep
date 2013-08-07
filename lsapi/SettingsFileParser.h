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
#if !defined(SETTINGSFILEPARSER_H)
#define SETTINGSFILEPARSER_H

#include "settingsdefines.h"
#include "lsapidefines.h"
#include <deque>
#include <list>
#include <strsafe.h>


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

private:
    /** Item used to check for recursive includes. */
    struct TrailItem {
        TrailItem(UINT uLine, LPCTSTR ptzPath) {
            this->uLine = uLine;
            this->ptzPath = ptzPath;
        }
        bool operator==(const TrailItem & item) {
            return _tcsicmp(item.ptzPath, this->ptzPath) == 0;
        }
        UINT uLine;
        LPCTSTR ptzPath;
    };

private:
    /**
     * Constructor.
     *
     * @param  pSettingsMap  SettingsMap to receive settings from files
     */
    FileParser(SettingsMap* pSettingsMap, std::list<TrailItem> &trail);

private:
    /**
     * Not implemented.
     */
    FileParser(const FileParser &);
    FileParser& operator=(const FileParser&);

public:
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

    /** Reference to the current trail of included files */
    std::list<TrailItem> &m_trail;

    /** Where the trail is actually stored, in the top-level parser */
    std::list<TrailItem> m_baseTrail;
    
    /** Handle to current file */
    FILE* m_phFile;
    
    /** Current Line Number */
    unsigned int m_uLineNumber;
    
    /** Full path to configuration file */
    TCHAR m_tzFullPath[MAX_PATH_LENGTH];
    
    /** Contains an RC key */
    struct TCStack {
        TCStack(LPCTSTR ptzString) {
            StringCchCopy(this->tzString, _countof(this->tzString), ptzString);
        }
        TCHAR tzString[MAX_RCCOMMAND];
    };

    /** Stack of prefixes. */
    std::deque<TCStack> m_stPrefixes;

    /** The next line to be parsed by _ReadLineFromFile */
    TCHAR m_tzReadAhead[MAX_LINE_LENGTH];

    /**
     * Reads the next line from the current file.
     */
    bool _ReadNextLine(LPTSTR ptzBuffer);
    
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

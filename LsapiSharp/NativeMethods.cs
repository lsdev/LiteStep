//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2015  LiteStep Development Team
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

using System;
using System.Runtime.InteropServices;
using System.Text;

[assembly: CLSCompliant(true)]

namespace LsapiSharp
{
    public static class NativeMethods
    {
        private const string LSAPI = "lsapi.dll";

        /// <summary>
        /// Logging Level Error
        /// </summary>
        public const int LS_LOG_ERROR = 1;

        /// <summary>
        /// Logging Level Warning
        /// </summary>
        public const int LS_LOG_WARNING = 2;

        /// <summary>
        /// Logging Level Notice (Info)
        /// </summary>
        public const int LS_LOG_NOTICE = 3;

        /// <summary>
        /// Logging Level Debug
        /// </summary>
        public const int LS_LOG_DEBUG = 4;

        /// <summary>
        /// Valid match
        /// </summary>
        public const int MATCH_VALID = 1;

        /// <summary>
        /// Premature end of pattern string
        /// </summary>
        public const int MATCH_END = 2;

        /// <summary>
        /// Premature end of text string
        /// </summary>
        public const int MATCH_ABORT = 3;

        /// <summary>
        /// Match failure on [..] construct
        /// </summary>
        public const int MATCH_RANGE = 4;

        /// <summary>
        /// Match failure on literal match
        /// </summary>
        public const int MATCH_LITERAL = 5;

        /// <summary>
        /// Bad pattern
        /// </summary>
        public const int MATCH_PATTERN = 6;

        /// <summary>
        /// Valid pattern
        /// </summary>
        public const int PATTERN_VALID = 0;

        /// <summary>
        /// Literal escape at end of pattern
        /// </summary>
        public const int PATTERN_ESC = -1;

        /// <summary>
        /// Malformed range in [..] construct
        /// </summary>
        public const int PATTERN_RANGE = -2;

        /// <summary>
        /// No end bracket in [..] construct
        /// </summary>
        public const int PATTERN_CLOSE = -3;

        /// <summary>
        /// [..] contstruct is empty
        /// </summary>
        public const int PATTERN_EMPTY = -4;

        /// <summary>
        /// 
        /// </summary>
        public const int ELD_BANGS = 1;

        /// <summary>
        /// 
        /// </summary>
        public const int ELD_MODULES = 2;

        /// <summary>
        /// 
        /// </summary>
        public const int ELD_REVIDS = 3;

        /// <summary>
        /// 
        /// </summary>
        public const int ELD_BANGS_V2 = 4;

        /// <summary>
        /// 
        /// </summary>
        public const int ELD_PERFORMANCE = 5;

        /// <summary>
        /// Win32 representation of DWORD value
        /// </summary>
        [StructLayout(LayoutKind.Explicit, Size = 4)]
        public struct LSColorRef
        {
            [FieldOffset(0)]
            public byte R;

            [FieldOffset(1)]
            public byte G;

            [FieldOffset(2)]
            public byte B;

            [FieldOffset(0)]
            public int Value;

            public LSColorRef(byte r, byte g, byte b)
            {
                this.Value = 0;
                this.R = r;
                this.G = g;
                this.B = b;
            }

            public LSColorRef(int value)
            {
                this.R = 0;
                this.G = 0;
                this.B = 0;
                this.Value = value & 0x00FFFFFF;
            }
        }

        /// <summary>
        /// Win32 representation of the HRESULT value
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Size = 4)]   
        public struct HResult
        {
            ///<summary>
            ///Success code
            ///</summary>
            public const int S_OK = unchecked((int)0x00000000);
           
            ///<summary>
            ///Success code false
            ///</summary>
            public const int S_FALSE = unchecked((int)0x00000001);

            ///<summary>
            ///Catastrophic failure
            ///</summary>
            public const int E_UNEXPECTED = unchecked((int)0x8000FFFF);

            ///<summary>
            ///One or more arguments are invalid
            ///</summary>
            public const int E_INVALIDARG = unchecked((int)0x80070057);

            ///<summary>
            ///Invalid pointer
            ///</summary>
            public const int E_POINTER = unchecked((int)0x80004003);

            ///<summary>
            ///Unspecified error
            ///</summary>
            public const int E_FAIL = unchecked((int)0x80004005);

            public int Value;

            public HResult(int value)
            {
                this.Value = value;
            }

            public static implicit operator int(HResult hr)
            {
                return hr.Value;
            }

            public static implicit operator HResult(int hr)
            {
                return new HResult(hr);
            }
        }

        /// <summary>
        /// Initializes the Litestep API which is the main entry point into library 
        /// </summary>
        /// <param name="litestepPath">Litestep's working directory</param>
        /// <param name="steprcPath">Path to Litestep's configuration file</param>
        /// <returns></returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool LSAPIInitialize(string litestepPath, string steprcPath);

        /// <summary>
        /// Opens a configuration file for sequential access to its contents. 
        /// </summary>
        /// <remarks>
        /// The file contents can be read with {@link #LCReadNextConfig} or {@link #LCReadNextLineOrCommand}. 
        /// The file should be closed with a call to {@link #LCClose} when it is no longer needed.
        /// 
        /// If the <paramref name="pszPath"/> argument is <code>NULL</code>, opens the global settings
        /// for sequential access.
        /// </remarks>
        /// <param name="filePath">Full path to the file to open</param>
        /// <returns>File handle on success or <code>NULL</code> if the file could not be opened</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern IntPtr LCOpen(string filePath);

        /// <summary>
        /// Closes a configuration file opened with {@link #LCOpen}.
        /// </summary>
        /// <param name="hFile">A valid file handle reference</param>
        /// <returns>Returns true on success otherwise false</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool LCClose(IntPtr hFile);

        /// <summary>
        /// Retrieves the next none config line (one that does not start with a '*' from a 
        /// configuration file. The entire line (including the setting name) is placed in the buffer.
        /// </summary>
        /// <remarks>
        /// Call this function repeatedly until it returns <code>FALSE</code> to retrieve all non config lines in the file.
        /// </remarks>
        /// <param name="hFile">File handle returned by LCOpen</param>
        /// <param name="buffer">Buffer to receive line</param>
        /// <param name="size">Size of buffer</param>
        /// <returns>Returns true on success otherwise false</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool LCReadNextCommand(IntPtr hFile, StringBuilder buffer, int size);

        /// <summary>
        /// Retrieves the next config line (one that starts with a '*') that begins with the 
        /// specified setting name from a configuration file. The entire line (including the setting name) 
        /// is placed in the buffer.
        /// </summary>
        /// <remarks>
        /// Call this function repeatedly until it returns <code>FALSE</code> to retrieve all the 
        /// lines that begin with the specified setting name.
        /// </remarks>
        /// <param name="hFile">File handle returned by LCOpen</param>
        /// <param name="config">Setting name</param>
        /// <param name="buffer">Buffer to receive line</param>
        /// <param name="size">Size of buffer</param>
        /// <returns>Returns true on success otherwise false</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool LCReadNextConfig(IntPtr hFile, string config, StringBuilder buffer, int size);

        /// <summary>
        /// Retrieves the next line from a configuration file. The entire line (including the setting name) is placed in the buffer.
        /// </summary>
        /// <remarks>
        /// Call this function repeatedly until it returns <code>FALSE</code> to retrieve all non config lines in the file.
        /// </remarks>
        /// <param name="hFile">File handle returned by LCOpen</param>
        /// <param name="buffer">Buffer to receive line</param>
        /// <param name="size">Size of buffer</param>
        /// <returns>Returns true on success otherwise false</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool LCReadNextLine(IntPtr hFile, StringBuilder buffer, int size);

        /// <summary>
        /// Parses and retrieves the values in a config line (one that starts with a '*') that begins with the 
        /// specified setting name from a configuration file.
        /// </summary>
        /// <remarks>
        /// Call this function repeatedly until it returns <code>0</code> to retrieve all config lines for the specified config name.
        /// </remarks>
        /// <param name="config">Setting name</param>
        /// <param name="tokens">An array of buffers to receive the values</param>
        /// <param name="tokenLen">The number of tokens expected to retrieve</param>
        /// <param name="extraParams">The remaining part of the config line that was not collected in the <paramref name="tokens"/> buffer</param>
        /// <returns>Returns the number of tokens found in the configuration line</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern int LCTokenize(string config, IntPtr tokens, int tokenLen, StringBuilder extraParams);

        /// <summary>
        /// Parses and retrieves the values in a config line (one that starts with a '*') that begins with the 
        /// specified setting name from a configuration file.
        /// </summary>
        /// <remarks>
        /// Parses lines that contain commands within brackes i.e [!command]
        /// </remarks>
        /// <param name="config">Command name</param>
        /// <param name="tokens">An array of buffers to receive the values</param>
        /// <param name="tokenLen">The number of tokens expected to retrieve</param>
        /// <param name="extraParams">The remaining part of the config line that was not collected in the <paramref name="tokens"/> buffer</param>
        /// <returns>Returns the number of tokens found in the configuration line</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern int CommandTokenize(string config, IntPtr tokens, int tokenLen, StringBuilder extraParams);

        /// <summary>
        /// Parses and retrieves the commands in a config line 
        /// </summary>
        /// <param name="line">A line in a configuration file</param>
        /// <param name="command">The buffer to receive the parsed command</param>
        /// <param name="args">The buffer to receive the arguments</param>
        /// <param name="commandLen">The size of the <paramref name="command"/> buffer</param>
        /// <param name="argsLen">The size of the <paramref name="args"/> buffer </param>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void CommandParse(string line, StringBuilder command, StringBuilder args, int commandLen, int argsLen);
        
        /// <summary>
        /// Cycles through a list of tokens parsing and retreiving values in a config line (one that starts with a '*')
        /// that begins with the specified setting name from a configuration file.
        /// </summary>
        /// <param name="line">A line in a configuration file</param>
        /// <param name="token">The buffer to receive the next token</param>
        /// <param name="nextToken">The next token in the line</param>
        /// <param name="useBrackets">parse tokens between brackes '[]'</param>
        /// <returns>Returns true on success otherwise false</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetToken(string line, StringBuilder token, IntPtr nextToken, bool useBrackets);

        /// <summary>
        /// Retrieves an integer value from the global settings. Returns <paramref name="defaultVal"/> 
        /// if the setting does not exist.
        /// </summary>
        /// <param name="settingName">Setting name</param>
        /// <param name="defaultVal">Default value</param>
        /// <returns>The setting value or default value</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern int GetRCInt(string settingName, int defaultVal);

        /// <summary>
        /// Retrieves an integer value from the global settings. Returns <paramref name="defaultVal"/> 
        /// if the setting does not exist.
        /// </summary>
        /// <param name="settingName">Setting name</param>
        /// <param name="defaultVal">Default value</param>
        /// <returns>The setting value or default value</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern long GetRCInt64(string settingName, long defaultVal);

        /// <summary>
        /// Retrieves an integer value from the global settings. Returns <paramref name="defaultVal"/> 
        /// if the setting does not exist.
        /// </summary>
        /// <param name="settingName">Setting name</param>
        /// <param name="defaultVal">Default value</param>
        /// <returns>The setting value or default value</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern float GetRCFloat(string settingName, float defaultVal);

        /// <summary>
        /// Retrieves an integer value from the global settings. Returns <paramref name="defaultVal"/> 
        /// if the setting does not exist.
        /// </summary>
        /// <param name="settingName">Setting name</param>
        /// <param name="defaultVal">Default value</param>
        /// <returns>The setting value or default value</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern double GetRCDouble(string settingName, double defaultVal);

        /// <summary>
        /// Retrieves an boolean value from the global settings. Returns <paramref name="defaultVal"/> 
        /// if the setting does not exist.
        /// </summary>
        /// <param name="settingName">Setting name</param>
        /// <param name="defaultVal">Default value</param>
        /// <returns>The setting value or default value</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetRCBoolDef(string settingName, bool defaultVal);

        /// <summary>
        /// Retrieves a Boolean value from the global settings. Returns <paramref name="ifFound"/> 
        /// if the setting exists and !<paramref name="ifFound"/> if it does not.
        /// </summary>
        /// <param name="settingName">Setting name</param>
        /// <param name="ifFound">Value to return if setting is found</param>
        /// <returns>The setting value</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetRCBool(string settingName, bool ifFound);

        /// <summary>
        /// Retrieves a string value from the global settings. Returns <code>FALSE</code> if 
        /// the setting does not exist copies <paramref="defaultVal" /> into the buffer and returns
        /// <code>FALSE</code>. <paramref="defaultVal" /> may be <code>null</code> in which case 
        /// nothing is copied into the buffer.
        /// </summary>
        /// <remarks>
        /// The raw setting value is parsed and the first space-delimited token or quoted string is the value returned.
        /// </remarks>
        /// <param name="settingName">Setting name</param>
        /// <param name="buffer">Buffer to receive value</param>
        /// <param name="defaultVal">Default value</param>
        /// <param name="size">Size of buffer</param>
        /// <returns>Returns true if it exists otherwise false</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetRCString(string settingName, StringBuilder buffer, string defaultVal, int size);

        /// <summary>
        /// Retrieves a raw string value from the global settings. If setting does not exist, copies <paramref="defaultVal" /> 
        /// into the buffer and returns <code>FALSE</code>. <paramref="defaultVal" /> may be <code>null</code> in which case 
        /// nothing is copied into the buffer.
        /// </summary>
        /// <remarks>
        /// <code>GetRCLine</code> does not parse the string value like <code>GetRCString</code> does. Its purpose is for retreiving command
        /// lines for applications and bang commands.
        /// </remarks>
        /// <param name="settingName">Setting name</param>
        /// <param name="buffer">Buffer to receive value</param>
        /// <param name="size">Size of buffer</param>
        /// <param name="defaultVal">Default value</param>
        /// <returns><code>TRUE</code> if setting exists or <code>FALSE</code> otherwise</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetRCLine(string settingName, StringBuilder buffer, int size, string defaultVal);

        /// <summary>
        /// Retrieves a color value from the global settings. Returns <paramref name="defaultVal"/> if the setting does not exist.
        /// </summary>
        /// <param name="settingsName">Setting name</param>
        /// <param name="defaultVal">Default value</param>
        /// <returns>The setting value or default value</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern LSColorRef GetRCColor(string settingsName, LSColorRef defaultVal);

        /// <summary>
        /// FILL ME IN
        /// </summary>
        /// <param name="settingName">Setting name</param>
        /// <param name="defaultVal">Default value</param>
        /// <param name="maxVal">Max value</param>
        /// <returns></returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern int GetRCCoordinate(string settingName, int defaultVal, int maxVal);

        /// <summary>
        /// Parses the coordinate from the line
        /// </summary>
        /// <param name="line"></param>
        /// <param name="defaultVal">Default value</param>
        /// <param name="maxVal">Max value</param>
        /// <returns></returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern int ParseCoordinate(string line, int defaultVal, int maxVal);

        /// <summary>
        /// Retrieves a string value from the global settings. Returns <code>FALSE</code> if the setting 
        /// does not exist. Performs the same operation as {@link #GetRCString}.
        /// </summary>
        /// <param name="settingName">The setting name</param>
        /// <param name="settingVal">Buffer to receive value</param>
        /// <returns><code>TRUE</code> if setting exists or <code>FALSE</code> otherwise</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool LSGetVariable(string settingName, StringBuilder settingVal);

        /// <summary>
        /// Retrieves a string value from the global settings. Returns <code>FALSE</code> if the setting 
        /// does not exist. Performs the same operation as {@link #GetRCString}.
        /// </summary>
        /// <param name="settingName">The setting name</param>
        /// <param name="settingVal">Buffer to receive value</param>
        /// <param name="size">The size of buffer</param>
        /// <returns><code>TRUE</code> if setting exists or <code>FALSE</code> otherwise</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode, EntryPoint = "LSGetVariableExW")]
        public static extern bool LSGetVariable(string settingName, StringBuilder settingVal, long size);

        /// <summary>
        /// Assigns a new value to a global setting. If the setting already exists its previous value 
        /// is overwritten, otherwise a new setting is created.
        /// </summary>
        /// <param name="settingName">The setting name</param>
        /// <param name="settingVal">New setting value</param>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void LSSetVariable(string settingName, string settingVal);

        /// <summary>
        /// Expands variable references. The template string is copied into the buffer with 
        /// all variable references (<code>$var$</code>) replaced by the value of the variable.
        /// </summary>
        /// <param name="buffer">buffer to received the expanded string</param>
        /// <param name="template">string to be expanded</param>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void VarExpansion(StringBuilder buffer, string template);

        /// <summary>
        /// Expands variable references. The template string is copied into the buffer with 
        /// all variable references (<code>$var$</code>) replaced by the value of the variable.
        /// </summary>
        /// <param name="buffer">buffer to received the expanded string</param>
        /// <param name="template">string to be expanded</param>
        /// <param name="size">The size of buffer</param>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode, EntryPoint = "VarExpansionExW")]
        public static extern void VarExpansion(StringBuilder buffer, string template, int size);

        /// <summary>
        /// Match the pattern against a text value
        /// </summary>
        /// <remarks>
        /// A match means the entire string <paramref name="text"/> is used up in matching.
        /// </remarks>
        /// <param name="pattern">A pattern expression</param>
        /// <param name="text">The string value to evaluate</param>
        /// <returns>Returns true on success otherwise false</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode, EntryPoint = "matchW")]
        public static extern bool Match(string pattern, string text);

        /// <summary>
        /// Match the pattern against a text value
        /// </summary>
        /// <param name="pattern">A pattern expression</param>
        /// <param name="text">The string value to evaluate</param>
        /// <returns>
        /// returns MATCH_VALID if pattern matches, or an errorcode as follows
        /// otherwise:
        ///
        ///     MATCH_PATTERN  - bad pattern
        ///     MATCH_LITERAL  - match failure on literal mismatch
        ///     MATCH_RANGE    - match failure on[..] construct
        ///     MATCH_ABORT    - premature end of text string
        ///     MATCH_END      - premature end of pattern string
        ///     MATCH_VALID    - valid match
        /// </returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode, EntryPoint = "matcheW")]
        public static extern int MatchE(string pattern, string text);

        /// <summary>
        /// Determines if a pattern is a well formed regular expression according to the folowing syntax:
        /// 
        /// In the pattern string:
        ///       `*' matches any sequence of characters (zero or more)
        ///       `?' matches any character
        ///       [SET] matches any character in the specified set,
        ///       [!SET] or[^ SET] matches any character not in the specified set.
        ///
        /// A set is composed of characters or ranges; a range looks like
        /// character hyphen character(as in 0-9 or A-Z).  [0-9a-zA-Z_] is the
        /// minimal set of characters allowed in the[..] pattern construct.
        /// Other characters are allowed(ie. 8 bit characters) if your _tsystem
        /// will support them.
        ///
        /// To suppress the special syntactic significance of any of `[]*?!^-\',
        /// and match the character exactly, precede it with a `\'.
        /// </summary>
        /// <remarks>
        /// Zero is returned in error_type if the pattern is a valid one.
        /// error_type return values are as follows:
        ///
        ///   PATTERN_VALID - pattern is well formed
        ///   PATTERN_ESC   - pattern has invalid escape('\' at end of pattern)
        ///   PATTERN_RANGE - [..] construct has a no end range in a '-' pair (ie[a -])
        ///   PATTERN_CLOSE - [..] construct has no end bracket(ie[abc - g )
        ///   PATTERN_EMPTY - [..] construct is empty(ie[])
        /// </remarks>
        /// <param name="pattern">A regular expression</param>
        /// <param name="errorCode">Is a return code based on the type of pattern error</param>
        /// <returns>Returns true on success otherwise false</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode, EntryPoint = "is_valid_patternW")]
        public static extern bool IsPatternValid(string pattern, ref int errorCode);
        
        /// <summary>
        /// Bang command callback
        /// </summary>
        /// <param name="hWndSender">The window handle sent the command</param>
        /// <param name="args">The arguments associated with the command</param>
        /// <code>
        /// typedef void (__cdecl *BangCommandW)(HWND hSender, LPCWSTR pszArgs);
        /// </code>
        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public delegate void BangCommandDelegate(int hWndSender, string args);

        /// <summary>
        /// Registers bang commands
        /// </summary>
        /// <param name="command">the command identifier that starts with an exclamation point (!)</param>
        /// <param name="commandDelegate">the callback method associated with the command</param>
        /// <returns>Returns true on success otherwise false</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool AddBangCommand(string command, [MarshalAs(UnmanagedType.FunctionPtr)] BangCommandDelegate commandDelegate);

        /// <summary>
        /// Removes a bang command from the list.
        /// </summary>
        /// <param name="command">bang command name</param>
        /// <returns>Returns true on success otherwise false</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool RemoveBangCommand(string command);

        /// <summary>
        /// Executes a bang command <code>!command</code>
        /// </summary>
        /// <param name="hwndCaller">The window handle sent the command</param>
        /// <param name="command">The identifier representing the command</param>
        /// <param name="args">The arguments associated with the command</param>
        /// <returns>Returns true on success otherwise false</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode, EntryPoint = "ParseBangCommandW")]
        public static extern bool ExecuteBangCommand(int hwndCaller, string command, string args);

        /// <summary>
        /// Self explanatory :-)
        /// </summary>
        /// <param name="level">The level to log to</param>
        /// <param name="module">The name of the module logging from</param>
        /// <param name="message">The content to write to file</param>
        /// <returns>Returns true on success otherwise false</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Winapi)]
        public static extern bool LSLog(int level, string module, string message);

        /// <summary>
        /// Self explanatory :-)
        /// </summary>
        /// <param name="level">The level to log to</param>
        /// <param name="module">The name of the module logging from</param>
        /// <param name="format">The formatting instructions</param>
        /// <param name="args">The values toreplace the tokens in the formatting instruction</param>
        /// <returns>Returns true on success otherwise false</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, EntryPoint = "LSLogPrintf")]
        public static extern bool LSLog(int level, string module, string format, params object[] args);

        /// <summary>
        /// Loads a string resource from the executable file associated with a specified module, 
        /// copies the string into a buffer, and appends a terminating null character.
        /// </summary>
        /// <param name="hInstance">A handle to an instance of the module whose executable file contains the string resource.</param>
        /// <param name="resId">The identifier of the string to be loaded.</param>
        /// <param name="resText">The buffer is to receive the string. Must be of sufficient length to hold a pointer (8 bytes).</param>
        /// <param name="size">
        /// The size of the buffer, in characters. The string is truncated and null-terminated if it is longer 
        /// than the number of characters specified. If this parameter is 0, then lpBuffer receives a read-only pointer to the resource itself.
        /// </param>
        /// <param name="defaultText">The default string to return if the resource was not located</param>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode, EntryPoint = "GetResStrW")]
        public static extern void GetResString(IntPtr hInstance, int resId, StringBuilder resText, int size, string defaultText);

        /// <summary>
        /// Loads a string resource from the executable file associated with a specified module, 
        /// copies the string into a buffer, and appends a terminating null character.
        /// </summary>
        /// <param name="hInstance">A handle to an instance of the module whose executable file contains the string resource.</param>
        /// <param name="resId">The identifier of the string to be loaded.</param>
        /// <param name="resText">The buffer is to receive the string. Must be of sufficient length to hold a pointer (8 bytes).</param>
        /// <param name="size">
        /// The size of the buffer, in characters. The string is truncated and null-terminated if it is longer 
        /// than the number of characters specified. If this parameter is 0, then lpBuffer receives a read-only pointer to the resource itself.
        /// </param>
        /// <param name="defaultText">The default string to return if the resource was not located</param>
        /// <param name="args">Formatting arguements</param>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode, EntryPoint = "GetResStrExW")]
        public static extern void GetResString(IntPtr hInstance, int resId, StringBuilder resText, int size, string defaultText, params object[] args);

        /// <summary>
        /// Bang enumeration callback
        /// </summary>
        /// <remarks>
        /// typedef BOOL (CALLBACK* LSENUMBANGSPROCW)(LPCWSTR, LPARAM);
        /// </remarks>
        /// <param name="command"></param>
        /// <param name="lparam"></param>
        /// <returns>Returns true on success otherwise false</returns>
        [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Unicode)]
        public delegate bool EnumBangDelegate(string command, IntPtr lparam);

        /// <summary>
        /// Bang enumeration callback
        /// </summary>
        /// <remarks>typedef BOOL (CALLBACK* LSENUMBANGSV2PROCW)(HINSTANCE, LPCWSTR, LPARAM);</remarks>
        /// <param name="hInstance"></param>
        /// <param name="command"></param>
        /// <param name="lparam"></param>
        /// <returns>Returns true on success otherwise false</returns>
        [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Unicode)]
        public delegate bool EnumBangV2Delegate(IntPtr hInstance, string command, IntPtr lparam);

        /// <summary>
        /// Rev ID enumeration callback
        /// </summary>
        /// <remarks>typedef BOOL (CALLBACK* LSENUMREVIDSPROCW)(LPCWSTR, LPARAM);</remarks>
        /// <param name="revId"></param>
        /// <param name="lparam"></param>
        /// <returns>Returns true on success otherwise false</returns>
        [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Unicode)]
        public delegate bool EnumRevIdsDelegate(string revId, IntPtr lparam);

        /// <summary>
        /// Module enumeration callback
        /// </summary>
        /// <remarks>typedef BOOL (CALLBACK* LSENUMMODULESPROCW)(LPCWSTR, DWORD, LPARAM)</remarks>
        /// <param name="module"></param>
        /// <param name="flags"></param>
        /// <param name="lparam"></param>
        /// <returns>Returns true on success otherwise false</returns>
        [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Unicode)]
        public delegate bool EnumModuleDelegate(string module, long flags, IntPtr lparam);

        /// <summary>
        /// Module performance enumeration callback
        /// </summary>
        /// <remarks>typedef BOOL (CALLBACK* LSENUMPERFORMANCEPROCW)(LPCWSTR, DWORD, LPARAM)</remarks>
        /// <param name="module"></param>
        /// <param name="flags"></param>
        /// <param name="lparam"></param>
        /// <returns>Returns true on success otherwise false</returns>
        [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Unicode)]
        public delegate bool EnumPerformanceDelegate(string module, long flags, IntPtr lparam);

        /// <summary>
        /// FILL ME IN
        /// </summary>
        /// <remarks>
        /// Enumerates over the following information:
        ///     - Bang commands
        ///     - Revision IDs 
        ///     - Loaded Modules
        ///     - Performance information about the module
        /// </remarks>
        /// <param name="eldInfo">The type of information to enumerate</param>
        /// <param name="callback">The delegate to call during enumeration</param>
        /// <param name="lparam">Extra information</param>
        /// <returns>
        /// Return values:
        ///   E_INVALIDARG - Invalid value for uInfo
        ///   E_POINTER    - Invalid callback
        ///   E_FAIL       - Unspecified error
        ///   E_UNEXPECTED - Callback crashed or other catastrophic failure
        ///   S_OK         - Enumeration successful, callback always returned TRUE
        ///   S_FALSE      - Enumeration successful, but cancelled by callback
        /// </returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern HResult EnumLSData(int eldInfo, [MarshalAs(UnmanagedType.FunctionPtr)] Delegate callback, IntPtr lparam);
    }
}

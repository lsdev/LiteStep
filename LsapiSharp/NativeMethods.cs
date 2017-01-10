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
        [StructLayout(LayoutKind.Sequential)]   
        public struct LSHResult
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

            /// <summary>
            /// No such interface supported
            /// </summary>
            public const int E_NOINTERFACE = unchecked((int)0x80004002);

            /// <summary>
            /// Class not registered
            /// </summary>
            public const int REGDB_E_CLASSNOTREG = unchecked((int)0x80040154);

            /// <summary>
            ///  Class does not support aggregation (or class object is remote)
            /// </summary>
            public const int CLASS_E_NOAGGREGATION = unchecked((int)0x80040110);

            public int Value;

            public LSHResult(int value)
            {
                this.Value = value;
            }

            public static implicit operator int(LSHResult hr)
            {
                return hr.Value;
            }

            public static implicit operator LSHResult(int hr)
            {
                return new LSHResult(hr);
            }
        }

        /// <summary>
        /// Win32 representation of the RECT structure
        /// </summary>
        [StructLayout(LayoutKind.Sequential)]
        public struct LSRect
        {
            int Left, Top, Right, Bottom;

            public LSRect(int left, int top, int right, int bottom)
            {
                Left = left;
                Top = top;
                Right = right;
                Bottom = bottom;
            }

            public int X
            {
                get { return Left; }
                set { Right -= (Left - value); Left = value; }
            }

            public int Y
            {
                get { return Top; }
                set { Bottom -= (Top - value); Top = value; }
            }

            public int Height
            {
                get { return Bottom - Top; }
                set { Bottom = value + Top; }
            }

            public int Width
            {
                get { return Right - Left; }
                set { Right = value + Left; }
            }
        }

        /// <summary>
        /// Win32 representation of the POINT structure
        /// </summary>
        [StructLayout(LayoutKind.Sequential)]
        public struct LSPoint
        {
            int X;
            int Y;

            public LSPoint(int x, int y)
            {
                this.X = x;
                this.Y = y;
            }
        }

        /// <summary>
        /// Win32 representation of the MONITORINFO structure
        /// </summary>
        [StructLayout(LayoutKind.Sequential)]
        public struct LSMonitorInfo
        {
            int Size;
            LSRect Monitor;
            LSRect Work;
            int flags;

            public static LSMonitorInfo Create()
            {
                return new LSMonitorInfo { Size = Marshal.SizeOf(typeof(LSMonitorInfo)) };
            }
        }

        /// <summary>
        /// Win32 representation of the DISPLAY_DEVICEW structure
        /// </summary>
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        public struct LSDisplayDevice
        {
            int cb;
            string DeviceName;
            string DeviceString;
            int StateFlags;
            string DeviceID;
            string DeviceKey;

            public static LSDisplayDevice Create()
            {
                return new LSDisplayDevice { cb = Marshal.SizeOf(typeof(LSDisplayDevice)) };
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
        /// Refreshes the list of bang commands
        /// </summary>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl)]
        public static extern void LSAPIReloadBangs();

        /// <summary>
        /// Refreshes the settings
        /// </summary>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl)]
        public static extern void LSAPIReloadSettings();

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
        /// <remarks>
        /// typedef void (__cdecl *BangCommandW)(HWND hSender, LPCWSTR pszArgs);
        /// </remarks>
        /// <param name="hWndSender">The window handle sent the command</param>
        /// <param name="args">The arguments associated with the command</param>
        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public delegate void BangCommandDelegate(IntPtr hWndSender, string args);

        /// <summary>
        /// Bang command callback
        /// </summary>
        /// <remarks>
        /// typedef void (__cdecl *BangCommandExW)(HWND hSender, LPCWSTR pszCommand, LPCWSTR pszArgs);
        /// </remarks>
        /// <param name="hWndSender">The window handle sent the command</param>
        /// <param name="command"></param>
        /// <param name="args">The arguments associated with the command</param>
        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public delegate void BangCommandDelegate2(IntPtr hWndSender, string command, string args);

        /// <summary>
        /// Registers bang commands
        /// </summary>
        /// <param name="command">the command identifier that starts with an exclamation point (!)</param>
        /// <param name="commandDelegate">the callback method associated with the command</param>
        /// <returns>Returns true on success otherwise false</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool AddBangCommand(string command, [MarshalAs(UnmanagedType.FunctionPtr)] Delegate commandDelegate);

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
        public static extern bool ExecuteBangCommand(IntPtr hwndCaller, string command, string args);

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
        public static extern LSHResult EnumLSData(int eldInfo, [MarshalAs(UnmanagedType.FunctionPtr)] Delegate callback, IntPtr lparam);

        /// <summary>
        /// Performs an operation on a specified file.
        /// </summary>
        /// <param name="hOwnerWnd">A handle to the parent window used for displaying a UI or error messages.</param>
        /// <param name="command">A pointer to a null-terminated string that specifies the !bang command on which to execute the </param>
        /// <param name="showCommand">
        /// The flags that specify how an application is to be displayed when it is opened. If lpFile specifies a 
        /// document file, the flag is simply passed to the associated application. It is up to the application to 
        /// decide how to handle it. 
        /// </param>
        /// <returns>
        /// If the function succeeds, it returns a value greater than 32. If the function fails, 
        /// it returns an error value that indicates the cause of the failure.
        /// </returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern IntPtr LSExecute(IntPtr hOwnerWnd, string command, int showCommand);

        /// <summary>
        /// Performs an operation on a specified file.
        /// </summary>
        /// <param name="hOwnerWnd">A handle to the parent window used for displaying a UI or error messages.</param>
        /// <param name="operation">
        /// A pointer to a null-terminated string, referred to in this case as a verb, that specifies the action to 
        /// be performed. The set of available verbs depends on the particular file or folder. Generally, the actions 
        /// available from an object's shortcut menu are available verbs.
        /// </param>
        /// <param name="command">
        /// A pointer to a null-terminated string that specifies the file or !bang command on which to execute the 
        /// specified verb. 
        /// </param>
        /// <param name="args">
        /// If lpFile specifies an executable file, this parameter is a pointer to a null-terminated string that specifies the parameters to be passed to 
        /// the application. The format of this string is determined by the verb that is to be invoked. 
        /// </param>
        /// <param name="directory">A pointer to a null-terminated string that specifies the default (working) directory for the action. </param>
        /// <param name="nShowCmd">
        /// The flags that specify how an application is to be displayed when it is opened. If lpFile specifies a 
        /// document file, the flag is simply passed to the associated application. It is up to the application to 
        /// decide how to handle it. 
        /// </param>
        /// <returns>
        /// If the function succeeds, it returns a value greater than 32. If the function fails, 
        /// it returns an error value that indicates the cause of the failure.
        /// </returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode, EntryPoint = "LSExecuteExW")]
        public static extern IntPtr LSExecute(IntPtr hOwnerWnd, string operation, string command, string args, string directory, int nShowCmd);

        /// <summary>
        /// Sets the main litestep window
        /// </summary>
        /// <param name="hWnd">The handle to the window</param>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl)]
        public static extern void LSAPISetLitestepWindow(IntPtr hWnd);

        /// <summary>
        /// Gets the litestep main window
        /// </summary>
        /// <returns>Returns a handle to the litestep main window on success</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode, EntryPoint = "GetLitestepWnd")]
        public static extern IntPtr GetLitestepWindow();

        /// <summary>
        /// Gets the path to the litestep directory
        /// </summary>
        /// <param name="path"></param>
        /// <param name="size"></param>
        /// <returns>Returns to true on success otherwise false</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Winapi, CharSet = CharSet.Unicode, EntryPoint = "LSGetLitestepPathW")]
        public static extern bool LSGetLitestepPath(StringBuilder path, int size);

        /// <summary>
        /// Gets the path to the image directory
        /// </summary>
        /// <param name="path"></param>
        /// <param name="size"></param>
        /// <returns>Returns to true on success otherwise false</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Winapi, CharSet = CharSet.Unicode, EntryPoint = "LSGetImagePathW")]
        public static extern bool LSGetImagePath(StringBuilder path, int size);

        /// <summary>
        /// FILL ME IN
        /// </summary>
        /// <param name="hDC"></param>
        /// <param name="rect"></param>
        /// <param name="topColor"></param>
        /// <param name="bottomColor"></param>
        /// <param name="width"></param>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Frame3D(IntPtr hDC, LSRect rect, LSColorRef topColor, LSColorRef bottomColor, int width);

        /// <summary>
        /// FILL ME IN
        /// </summary>
        /// <param name="hBmp"></param>
        /// <param name="transparent"></param>
        /// <param name="tolerance"></param>
        /// <param name="xoffset"></param>
        /// <param name="yoffset"></param>
        /// <returns></returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr BitmapToRegion(IntPtr hBmp, LSColorRef transparent, LSColorRef tolerance, int xoffset, int yoffset);

        /// <summary>
        /// FILL ME IN
        /// </summary>
        /// <param name="hIcon"></param>
        /// <returns></returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr BitmapFromIcon(IntPtr hIcon);

        /// <summary>
        /// FILL ME IN
        /// </summary>
        /// <param name="file"></param>
        /// <param name="image"></param>
        /// <returns></returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern IntPtr LoadLSImage(string file, string image);

        /// <summary>
        /// FILL ME IN
        /// </summary>
        /// <param name="image"></param>
        /// <param name="file"></param>
        /// <returns></returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern IntPtr LoadLSIcon(string image, string file);

        /// <summary>
        /// FILL ME IN
        /// </summary>
        /// <param name="hBitmap"></param>
        /// <param name="nWidth"></param>
        /// <param name="nHeight"></param>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl)]
        public static extern void GetLSBitmapSize(
            IntPtr hBitmap, 
            [MarshalAs(UnmanagedType.LPStruct)] out int width, 
            [MarshalAs(UnmanagedType.LPStruct)] out int height);

        /// <summary>
        /// FILL ME IN
        /// </summary>
        /// <param name="hDC"></param>
        /// <param name="xDest"></param>
        /// <param name="yDest"></param>
        /// <param name="width"></param>
        /// <param name="height"></param>
        /// <param name="tempDC"></param>
        /// <param name="xSrc"></param>
        /// <param name="ySrc"></param>
        /// <param name="transparent"></param>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl)]
        public static extern void TransparentBltLS(IntPtr hDC, int xDest, int yDest, int width, int height, IntPtr tempDC, int xSrc, int ySrc, LSColorRef transparent);

        /// <summary>
        /// Sets the size of the work area.
        /// </summary>
        /// <param name="left"></param>
        /// <param name="top"></param>
        /// <param name="right"></param>
        /// <param name="bottom"></param>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetDesktopArea(int left, int top, int right, int bottom);

        /// <summary>
        /// Retrieves the specified system metric or system configuration setting.
        /// </summary>
        /// <remarks>See <see cref="https://msdn.microsoft.com/en-us/library/windows/desktop/ms724385(v=vs.85).aspx"/> 
        /// for details on <paramref name="index"/> values</remarks>
        /// <param name="index">The system metric or configuration setting to be retrieved</param>
        /// <returns>
        /// If the function succeeds, the return value is the requested system metric or configuration setting.
        /// If the function fails, the return value is 0. GetLastError does not provide extended error information. 
        ///</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl)]
        public static extern int LSGetSystemMetrics(int index);

        /// <summary>
        /// The MonitorFromWindow function retrieves a handle to the display monitor that has 
        /// the largest area of intersection with the bounding rectangle of a specified window.
        /// </summary>
        /// <param name="hWnd">A handle to the window of interest.</param>
        /// <param name="flags">Determines the function's return value if the window does not intersect any display monitor.</param>
        /// <returns>
        /// If the window intersects one or more display monitor rectangles, the return value is an HMONITOR handle to the display monitor
        /// that has the largest area of intersection with the window.
        /// </returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr LSMonitorFromWindow(IntPtr hWnd, int flags);

        /// <summary>
        /// The MonitorFromRect function retrieves a handle to the display monitor that has the 
        /// largest area of intersection with a specified rectangle.
        /// </summary>
        /// <param name="coordinates">A pointer to a RECT structure that specifies the rectangle of interest in virtual-screen coordinates.</param>
        /// <param name="flags">Determines the function's return value if the rectangle does not intersect any display monitor.</param>
        /// <returns></returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr LSMonitorFromRect([In, MarshalAs(UnmanagedType.LPStruct)] LSRect coordinates, int flags);

        /// <summary>
        /// The MonitorFromPoint function retrieves a handle to the display monitor that contains a specified point.
        /// </summary>
        /// <param name="coordinates">A POINT structure that specifies the point of interest in virtual-screen coordinates.</param>
        /// <param name="flags">Determines the function's return value if the point is not contained within any display monitor.</param>
        /// <returns>
        /// If the point is contained by a display monitor, the return value is an HMONITOR handle to that display monitor.
        /// If the point is not contained by a display monitor, the return value depends on the value of <paramref name="flags"/>.
        /// </returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr LSMonitorFromPoint(LSPoint coordinates, int flags);

        /// <summary>
        /// Retrieves information about a display monitor.
        /// </summary>
        /// <param name="hMonitor">A handle to the display monitor of interest.</param>
        /// <param name="info">A pointer to a MONITORINFO or MONITORINFOEX structure that receives information 
        /// about the specified display monitor.</param>
        /// <returns>
        /// If the function succeeds, the return value is nonzero.
        /// If the function fails, the return value is zero.
        /// </returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool LSGetMonitorInfo(IntPtr hMonitor, [In, Out, MarshalAs(UnmanagedType.LPStruct)] LSMonitorInfo info);

        /// <summary>
        /// A MonitorEnumProc function is an application-defined callback function that 
        /// is called by the EnumDisplayMonitors function.
        /// </summary>
        /// <remarks>typedef BOOL (CALLBACK* MONITORENUMPROC)(HMONITOR, HDC, LPRECT, LPARAM)</remarks>
        /// <param name="hMonitor">A handle to the display monitor.</param>
        /// <param name="hDC">A handle to a device context.</param>
        /// <param name="area">A pointer to a RECT structure.</param>
        /// <param name="lparam">Application-defined data that EnumDisplayMonitors passes directly to the enumeration function.</param>
        /// <returns>To continue the enumeration, return TRUE otherise FALSE.</returns>
        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        public delegate bool EnumMonitorDelegate(IntPtr hMonitor, IntPtr hDC, LSRect area, IntPtr lparam);

        /// <summary>
        /// Enumerates display monitors (including invisible pseudo-monitors associated with the mirroring drivers) that 
        /// intersect a region formed by the intersection of a specified clipping rectangle and the visible region of a device context.
        /// </summary>
        /// <param name="hDC">A handle to a display device context that defines the visible region of interest.</param>
        /// <param name="clip">A pointer to a RECT structure that specifies a clipping rectangle. The region of interest is the intersection 
        /// of the clipping rectangle with the visible region specified by hdc.</param>
        /// <param name="callback">A pointer to a MonitorEnumProc application-defined callback function.</param>
        /// <param name="lparam">Application-defined data that EnumDisplayMonitors passes directly to the MonitorEnumProc function.</param>
        /// <returns>If the function succeeds, the return value is nonzero otherwise zero</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool LSEnumDisplayMonitors(
            IntPtr hDC, 
            [In, MarshalAs(UnmanagedType.LPStruct)] LSRect clip, 
            [MarshalAs(UnmanagedType.FunctionPtr)] EnumMonitorDelegate callback, 
            IntPtr lparam);

        /// <summary>
        /// Obtains information about the display devices in the current session.
        /// </summary>
        /// <param name="device">A pointer to the device name. </param>
        /// <param name="index">An index value that specifies the display device of interest.</param>
        /// <param name="displayInfo">A pointer to a <seealso cref="LSDisplayDevice"/> structure that 
        /// receives information about the display device specified by <paramref name="index"/>. </param>
        /// <param name="flags">Set this flag to EDD_GET_DEVICE_INTERFACE_NAME (0x00000001) to retrieve 
        /// the device interface name for GUID_DEVINTERFACE_MONITOR, which is registered by the operating 
        /// system on a per monitor basis. </param>
        /// <returns>
        /// If the function succeeds, the return value is nonzero. If the function fails, the return value is zero. 
        /// The function fails if <paramref name="index"/> is greater than the largest device index.
        /// </returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool LSEnumDisplayDevices(
            string device, 
            int index, 
            [In, MarshalAs(UnmanagedType.LPStruct)] LSDisplayDevice displayInfo, 
            int flags);

        /// <summary>
        /// Creates a single uninitialized object of the class associated with a specified CLSID.
        /// </summary>
        /// <param name="rclsid">The CLSID associated with the data and code that will be used to create the object.</param>
        /// <param name="pUnkOuter">
        /// If NULL, indicates that the object is not being created as part of an aggregate. If non-NULL, 
        /// pointer to the aggregate object's IUnknown interface (the controlling IUnknown).
        /// </param>
        /// <param name="dwClsContext">Context in which the code that manages the newly created object will run.</param>
        /// <param name="riid">A reference to the identifier of the interface to be used to communicate with the object.</param>
        /// <param name="ppv">
        /// Address of pointer variable that receives the interface pointer requested in riid. 
        /// Upon successful return, *ppv contains the requested interface pointer. 
        /// </param>
        /// <returns>
        /// This function can return the following values
        ///   E_NOINTERFACE          - The specified class does not implement the requested interface, or the controlling IUnknown does not expose the requested interface
        ///   E_POINTER              - The ppv parameter is NULL.
        ///   CLASS_E_NOAGGREGATION  - This class cannot be created as part of an aggregate.
        ///   REGDB_E_CLASSNOTREG    - A specified class is not registered in the registration database. 
        ///   S_OK                   - An instance of the specified object class was successfully created.
        /// </returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl)]
        public static extern LSHResult LSCoCreateInstance(
            [In, MarshalAs(UnmanagedType.LPStruct)] Guid rclsid,
            IntPtr pUnkOuter, 
            int dwClsContext,
            [In, MarshalAs(UnmanagedType.LPStruct)] Guid riid,
            [MarshalAs(UnmanagedType.IUnknown)] out object ppv);


        /// <summary>
        /// Sets the COM interface factory for this API
        /// </summary>
        /// <param name="factory">Factory object</param>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl)]
        public static extern void LSAPISetCOMFactory([In, MarshalAs(UnmanagedType.IUnknown)] object factory);
    }
}

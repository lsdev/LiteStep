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
        /// Win32 representation of DWORD value
        /// </summary>
        [StructLayout(LayoutKind.Explicit, Size = 4)]
        public struct COLORREF
        {
            public COLORREF(byte r, byte g, byte b)
            {
                this.Value = 0;
                this.R = r;
                this.G = g;
                this.B = b;
            }

            public COLORREF(int value)
            {
                this.R = 0;
                this.G = 0;
                this.B = 0;
                this.Value = value & 0x00FFFFFF;
            }

            [FieldOffset(0)]
            public byte R;

            [FieldOffset(1)]
            public byte G;

            [FieldOffset(2)]
            public byte B;

            [FieldOffset(0)]
            public int Value;
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
        /// Retrieves an integer value from the global settings. Returns <paramref name="defaultVal"/> 
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
        public static extern COLORREF GetRCColor(string settingsName, COLORREF defaultVal);

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
        /// Self explanatory :-)
        /// </summary>
        /// <param name="level">The level to log to</param>
        /// <param name="module">The name of the module logging from</param>
        /// <param name="message">The content to write to file</param>
        /// <returns>Returns true on success otherwise false</returns>
        [DllImport(LSAPI, CallingConvention = CallingConvention.Cdecl)]
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
        public static extern bool LSLog(int level, string module, string format, params object [] args);
    }
}

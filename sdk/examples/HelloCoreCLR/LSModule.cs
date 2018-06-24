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
using static LsapiSharp.NativeMethods;

namespace HelloCoreCLR
{
    public class LSModule
    {
        const string BANG_COMMAND = "!HelloCoreClr";

        // Implements the !Hello bang command
        static BangCommandDelegate BangHello = (sender, cmd) =>
        {
            LSLog(LS_LOG_DEBUG, typeof(LSModule).FullName, "BangHello called from .NET Core");
            MessageBox(IntPtr.Zero, "Hello, Litestep from .NET Core!", typeof(LSModule).FullName, 0);
        };

        /// <summary>
        /// Litestep calls this function after it loads the module. This is where the
        /// module should register bang commands and create windows. If initialization
        /// succeeds, return zero. If an error occurs, return a nonzero value.
        /// </summary>
        /// <param name="hWndMain"></param>
        /// <param name="appPath"></param>
        public static void initModule(Int32 hWndMain, String appPath)
        {
            LSLog(LS_LOG_DEBUG, typeof(LSModule).FullName, "initModule called");
            AddBangCommand(BANG_COMMAND, BangHello);
        }

        /// <summary>
        /// Litestep calls this function before it unloads the module. This function
        /// should unregister bang commands and destroy windows. 
        /// </summary>
        public static void quitModule()
        {
            LSLog(LS_LOG_DEBUG, typeof(LSModule).FullName, "quitModule called");
            RemoveBangCommand(BANG_COMMAND);
        }

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        static extern int MessageBox(IntPtr hWnd, String text, String caption, int options);
    }

}

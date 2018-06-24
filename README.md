# LiteStep
LiteStep Alternative Shell is a replacement shell for the standard Windows® Explorer shell originally created by Francis Gastellu. 

This fork focuses on the interoperability of Litestep and the [.NET Framework](https://www.microsoft.com/net/framework).

## Installation

Please refer to the official [installation guide](http://litestep.info/overview/litestep-manual.html) for information on how to setup and configure Litestep

## Documentation

#### Step.rc 

As mentioned above this version of Litestep is focused on the .NET Framework and the following are descriptions of the configurations that should be present in the `step.rc` in order to use .NET.

    ; Set the version of the .NET Runtime that Litestep will use to load modules. This version represents
    ; the actual folder name on the file system located in %WINDIR%\Microsoft.NET\Framework64
    ; If the setting is omitted the default runtime is v2.0.50727
    LSCLRVersion v4.0.30319 
    
    ; Loading modules are the same as described in the Litestep manual
    ; To load a module built using the .NET Framework append the "clr" token after the module path 
    LoadModule $LiteStepDir$HelloCLR.dll clr

    ; OR to set the version of the .NET Core Runtime. This version represents
    ; the actual folder name on the file system located in %PROGRAMFILES%\dotnet\shared\Microsoft.NETCore.App
    LSCLRVersion 2.0.0

    ; Loading modules are the same as described in the Litestep manual
    ; To load a module built using the .NET Framework append the "coreclr" token after the module path 
    LoadModule $LiteStepDir$HelloCoreCLR.dll coreclr
    
#### .NET Module

Now that you know how to configure Litestep to load the .NET Runtime the following demonstrates how to create a module with the .NET Framework. The Litestep SDK comes with a library called Lsapi# _(pronounced LSAPI-sharp)_ which is a C# binding for Litestep's API that will enable modules to interact with the shell. 

Following is a list of requirements in order for your module to be loaded by the shell

1. Add the LsapiSharp assembly as a reference to your project
2. Name the assembly and the entry point namespace the same (ie. MyModule.dll and namespace MyModule {})
3. The entry point class name must be called **LSModule** with two methods called **initModule** and **quitModule** respectively

#### Example HelloCLR.dll

```c#
using System;
using System.Runtime.InteropServices;
using static LsapiSharp.NativeMethods;

namespace HelloCLR
{
    public static class LSModule
    {
        const string BANG_COMMAND = "!HelloClr";

        // Implements the !Hello bang command
        static BangCommandDelegate BangHello = (sender, cmd) =>
        {
            LSLog(LS_LOG_DEBUG, typeof(LSModule).FullName, "BangHello called");
            MessageBox(IntPtr.Zero, "Hello, Litestep!", typeof(LSModule).FullName, 0);
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
```

## LICENSE

LiteStep is released under the GNU General Public License Version 2. [link](docs/license.txt)

## Questions?

If you have any questions or comments please feel free to drop me a line :-).

Email: <donellesanders@gmail.com>
Follow Me: [@DonelleJr](https://twitter.com/DonelleJr)

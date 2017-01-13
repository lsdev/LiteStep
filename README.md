# LiteStep
LiteStep Alternative Shell is a replacement shell for the standard Windows® Explorer shell originally created by Francis Gastellu. 

This fork focuses on the interoperability of Litestep and the [.NET Framework](https://www.microsoft.com/net/framework).

## Installation

Please refer to the official [installation guide](http://litestep.info/overview/litestep-manual.html) for information on how to setup and configure Litestep

## Documentation

As mentioned above this version of Litestep is focused on the .NET Framework and the following are descriptions of the configurations that should be present in the **step.rc** in order to use .NET.

    ; Set the version of the .NET Runtime that Litestep will use to load modules
    ; If the setting is omitted the default runtime is v2.0.50727
    LSCLRVersion v4.0.30319 
    
    ; Loading modules are the same as described in the Litestep manual
    ; To load a module built using the .NET Framework append the "clr" token after the module path 
    LoadModule $LiteStepDir$HelloCLR.dll clr
    

## LICENSE

LiteStep is released under the GNU General Public License Version 2. [link](docs/license.txt)

## Questions?

If you have any questions or comments please feel free to drop me a line :-).

Email: <donellesanders@gmail.com>
Follow Me: [@DonelleJr](https://twitter.com/DonelleJr)

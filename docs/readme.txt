=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    LiteStep 0.24.7 Release Candidate 1
    Readme/Release Notes
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


-- PREFACE ---------------------------------------------------------------------

 The next update from your friendly devteam has arrived, announcing the first
 Release Candidate of LiteStep 0.24.7. As usual the changes since the last
 release (Beta 4) are listed below; if you want a list of changes since 0.24.6
 please check changes.txt in addition to this document.
 Since the number of bugreports has significantly decreased in the last couple
 of weeks it looks like we're finally past the "beta" stage. If you can still
 find any bugs and/or annoyances please let us know.
 Note that installation instructions are not included. Use at your own risk.


-- CHANGES SINCE BETA 4 --------------------------------------------------------

 - Fixed startup items without full path not being run. This should for example
   fix the problems with Point32.exe (the intellimouse driver/utility app)

 - Startup apps are run after modules are loaded. Should fix some problems with
   missing systray icons on startup

 - Temporary aboutbox replaced with 0.24.6-style aboutbox

 - New lsapi export: EnumLSData (for modules and apps to gather core info, e.g.
   for aboutbox, lscp, rabidmodulebangs)

 - Fixed various tray issues, especially with the volume and DUN icons. The
   second "ghost" DUN icon on XP is gone. Thanks jugg!

 - Fixed empty MessageBox appearing if module was loaded twice

 - Fixed protection against modules being loaded multiple times

 - macros.h now includes RabidCow's fixed multimon macros

 - Fixed trailing whitespace not being removed from step.rc lines with an
   end-line comment (should help with some mzscript problems)

 - Fixed conditional statements not working with negative numbers

 - Changed accelerator keys in the (english) recovery menu: Recycle is &c, Run
   is &r, Shutdown is &u

 - LM_UN/RELOADMODULE no longer automatically translate string to HINSTANCE.
   Automatic translation from SendMessage to PostMessage removed. This fixes
   most !un/reloadmodule problems. Note that making a module reload itself may
   not work reliably (e.g. reloading mzscript in a *Script or unloading
   hotkey.dll in a *Hotkey line)

 - TrayService won't start if another tray is found

 - Fixed !unloadmodule not taking multiple [bracket][tokens] as arguments (used
   to unload multiple modules on a single !unloadmodule line)

 - GetRC* APIs are a bit more robust (finally added checks for NULL pointers)


-- WHAT NEEDS TO BE TESTED -----------------------------------------------------

 - !reloadmodule, !unloadmodule, especially in (mz)scripts

 - Module threading

 - Windows 9x compatibility, especially Windows 95


-- KNOWN PROBLEMS --------------------------------------------------------------

 - LCReadNextLine returns items in alphabetical order. No recent modules are
   affected by this


-- FINAL WORDS -----------------------------------------------------------------

 Many thanks to Phil Stopford for hosting the binaries. A huge "thank you" to
 everyone who supported us in the beta phase. Without your feedback and
 encouragement this release would not have been possible.
 Please send any suggestions/bug reports to ilmcuts_AT_gmx_DOT_net and/or post
 on the LS Mailing List (http://wuzzle.org/list/litestep.php or
 news://news.gmane.org/gmane.comp.windows.shells.litestep).
 Happy litestepping,

 The LiteStep Development Team.
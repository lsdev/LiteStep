=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    LiteStep 0.24.7 Beta 4
    Readme/Release Notes
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


-- PREFACE ---------------------------------------------------------------------

 Here's yet another update from your friendly devteam, announcing 0.24.7 beta 4.
 Changes since beta 3 are listed below; as always check changes.txt for a list
 of changes since 0.24.6.
 In this build a number of issues have been addressed. More code changes went
 into it than into the last two betas combined. However it is still labelled
 "beta" since there may still be some bugs left. If you can find any bugs and/or
 annoyances please let us know.
 As usual, here's the disclaimer: This build is for experienced users only;
 installation instructions are not included. Use at your own risk.


-- CHANGES SINCE BETA 3 --------------------------------------------------------

 - No more problems with spaces in $LitestepDir$.

 - AppBar hack added. A few ApBars may work with this.

 - Added language indicator icon support (internat.exe), except for XP. Systray
   modules need to be updated to display the context menu correctly (appears in
   0,0 otherwise).

 - Miscellaneous other tray patches.

 - Fixed ShellHook issues on 9x (should fix taskbar3 problems).

 - Major thread safety fixes in BangManager, ModuleManager, MessageManager.

 - Fixed potential access violations after !recycle, !quit, and !logoff.
   Should fix crashes with !recycle in scripts.

 - Fixed access violations in conditional statements parser.

 - Fixed StartupRunner not running regkeys correctly (RunOnce was among them).

 - Added RunOnceEx support (thanks to the cooperation with Geoshell).

 - Added !ToggleModules.

 - Various potential memory leaks fixed (e.g. if a module didn't call LCClose
   after LCOpen the core would leak memory on !recycle).

 - If an undefined evar is used an error message is displayed even if math
   support is enabled.

 - Added -startup to force running of startup apps.

 - LM_UN/RELOADMODULE can now un/reload modules by passing the HINSTANCE of the
   module as the wParam. Set the lParam to LMM_HINSTANCE in that case.
   !Reloadmodule and !unloadmodule take advantage of this and should thus no
   longer crash even if a module tries to un-/reload itself.

 - Fixed problems with modules using LM_RELOADMODULE during the startup process
   (e.g. mzscript, netloadmodule).

 - Removed -install and -uninstall.

 - Removed !ToggleWharf and !Popup from the core.


-- WHAT NEEDS TO BE TESTED -----------------------------------------------------

 If you can provide details about any of the issues below please don't hesitate
 to send an email. :)

 - Module threading.

 - Windows 9x compatibility, especially Windows 95.

 - System DDE (add LSUseSystemDDE to your step.rc and restart LS. !recycle is
   not enough).

 - Locking the workstation while Litestep is running, both with and without
   LSUseSystemDDE. There were reports of Litestep locking up after the
   workstation is unlocked. We haven't been able to reproduce this behavior.

 - jDesk in conjunction with IconDesk. For some strange reason jDesk sometimes
   reports "window class already exists" even if it's loaded first.


-- KNOWN PROBLEMS --------------------------------------------------------------

 - No fully functional aboutbox. The current MessageBox is a temporary solution.

 - Fullscreen app detection suffers from false positives.

 - LS locks up without an error message if a PNG is used and libpng13.dll and/or
   zlib.dll aren't present.


-- FINAL WORDS -----------------------------------------------------------------

 Once again thanks to Phil Stopford for hosting the binaries. And of course
 thanks to everyone who used the betas so far. Your feedback has been very
 encouraging and is greatly appreciated.
 Please direct any suggestions/bug reports to ilmcuts_AT_gmx_DOT_net and/or post
 on the LS Mailing List (http://wuzzle.org/list/litestep.php).
 Thanks for your support,

 The LiteStep Development Team.
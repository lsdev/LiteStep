=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    LiteStep 0.24.7 Beta 3
    Readme/Release Notes
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


-- PREFACE ---------------------------------------------------------------------

 Once more here's an update from your friendly devteam, this time announcing
 0.24.7 beta 3. Below is a list of changes since beta 2, check changes.txt for a
 list of changes since 0.24.6.
 This build hopefully fixes all bugs reported for beta 1 and beta 2. All
 sections below were updated to reflect these changes, so read them carefully.
 If there are still bugs and/or annoyances in this beta please let us know.
 As usual, here's the disclaimer: This build is for experienced users only;
 installation instructions are not included. Use at your own risk.


-- CHANGES SINCE BETA 2 --------------------------------------------------------

 - DDEService now runs in its own thread, this should fix a few deadlocks.
   Do NOT load liteman.dll with this build.

 - Added "LSUseSystemDDE". This loads Windows' DDE server instead of the
   DDEService. Results in larger memory usage but "proper" DDE support.
   LSUseSystemDDE takes effect on startup only, not on !recycle.
   Requires shdocvw.dll (bundled with Internet Explorer 4).

 - Autmatic module hiding is disabled by default; replaced LSNoAutoHideModules
   with LSAutoHideModules to enable it. It is recommended to bind !HideModules
   and !ShowModules to hotkeys instead.

 - LCOpen uses a separate SettingsMap for each file opened with LCOpen. This is
   the thread-safe approach but makes it impossible for modules to use the
   GetRC* APIs to retrieve settings from those files. For example this affects
   Rabidvwm which can no longer read its settings directly from .box files. Put
   the settings into step.rc instead or "include" the .box file(s) as a
   workaround.

 - Improved handling of exceptions in threaded modules.

 - Improved handling of exceptions during the execution of !bangs.

 - Added inform-sega's ShellExecute fix for .lnk files. This should enable you
   to use .lnk files as LS commands; e.g.
     LabelOnLeftClick c:\path\browser.lnk

 - TrayService rewritten. Should fix most (hopefully all) tray problems.

 - Fixed $username$ not accepting names with spaces.
 
 - Added a SHGetSpecialFolderPath workaround to improve Win95 compatibility.

 - Fixed minor resource leaks in StartupRunner.

 - Fixed bugs in the DDEService implementation (paths with spaces were quoted
   incorrectly).

 - Added $quicklaunch$ from Indie LS.

 - litestep.exe build option changed to "Multithreaded DLL" - reduces filesize.

 - Removed some code duplication.

 - Reduced header dependency.


-- WHAT NEEDS TO BE TESTED -----------------------------------------------------

 If you can provide details about any of the issues below please don't hesitate
 to send an email. :)

 - Ability to be used as a drop-in replacement for 0.24.6. You should not have
   to make any changes in your step.rc when upgrading from 0.24.6 or Indie LS,
   except to work around the missing features listed in changes.txt. If you
   experience any problems during this process please report them.

 - Module threading.

 - Windows 9x compatibility, especially Windows 95.

 - Systray. The reported issues should be fixed but you never know.

 - Loading ICQ Lite and/or Crazy Browser while LS is running.

 - System DDE.

 - Locking the workstation while Litestep is running, both with and without
   LSUseSystemDDE. There were reports of Litestep locking up after the
   workstation is unlocked.

 - DUN icon on Windows XP.

 - McAfee Antivirus. Apparently there should be two icons but LS only shows one.


-- KNOWN PROBLEMS --------------------------------------------------------------

 - No fully functional aboutbox. The current MessageBox is a temporary solution.

 - -install/-uninstall command line switches not finished (and thus
   disfunctional). The question has arisen whether this is even the job of
   litestep.exe or if it should be moved to a separate app. Please comment if
   you have any thoughts on this.

 - Deadlocks in (hopefully) rare circumstances.

 - Fullscreen app detection suffers from false positives.

 - #include dependencies in lsapi.h make it very difficult to distribute lsapi.h
   with a module's sourcecode.

 - LS locks up if a PNG is used and libpng13.dll or zlib.dll is not present.


-- FINAL WORDS -----------------------------------------------------------------

 Thanks to Phil Stopford for hosting the binaries, and to everyone for the
 feedback. You are a great help in tracking down bugs and other little glitches.
 Please direct any suggestions/bug reports to ilmcuts_AT_gmx_DOT_net and/or post
 on the LS Mailing List (http://wuzzle.org/list/litestep.php).
 Thanks for your support,

 The LiteStep Development Team.
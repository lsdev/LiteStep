=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    LiteStep 0.24.7 Beta 2
    Readme/Release Notes
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


-- PREFACE ---------------------------------------------------------------------

 Here's the next update from your friendly devteam. Once more it is assumed that
 you are familiar with the last 0.24.7 build (Beta 1). If you are not familiar
 with the 0.24.7 prerelease builds please check changes.txt before reading this
 document.
 Compared to Beta 1 this build fixes some of the most reported bugs. Both the
 list of known bugs and the "what needs to be tested" section were updated, so
 please read those again. It would be a great help if you could notify us if a
 problem you reported is neither fixed nor included in the list of known bugs.
 For experienced users only; installation instructions are not included. As
 always, use at your own risk.
 This document only covers the most important issues, for details check
 changes.txt (still slightly incomplete).


-- CHANGES SINCE BETA 1 --------------------------------------------------------

 - Fixed StartupRunner's handling of unquoted paths and of programs with command
   line arguments.

 - Fixed math evaluation not supporting zeroes or spaces.

 - Fixed LCReadNext* APIs not using case insensitive string comparison.

 - Fixed exceptions during module initialization not being propagated to
   the ModuleManager.

 - Added a temporary aboutbox (after frequent requests from various parties via
   Phil Stopford).


-- WHAT NEEDS TO BE TESTED -----------------------------------------------------

 If you can provide details about any of the issues below please don't hesitate
 to send an email. :)

 - Ability to be used as a drop-in replacement for 0.24.6. You should not have
   to make any changes in your step.rc when upgrading from 0.24.6 or Indie LS,
   except to work around the missing features listed in changes.txt. If you
   experience any problems during this process please report them.

 - Loading Liteman.dll with 0.24.7. This should NOT have any sideeffects.

 - Threading.

 - jKey (especially if used with vk104.txt).

 - LSMail - it can cause other modules to act very strangely. If you are loading
   LSMail and there is a module showing unusual behavior try disabling LSMail.
   In case that fixes the other module's behavior please include the name and
   version of both modules in your bugreport.

 - LSWChanger.

 - Windows 9x compatibility. LS apparently does not load under Win95.

 - Systray. Some icons appear to be missing.

 - Loading ICQ Lite and/or Crazy Browser while LS is running.


-- KNOWN PROBLEMS --------------------------------------------------------------

 - No fully functional aboutbox. The current MessageBox is a temporary solution.

 - -install/-uninstall command line switches not finished (and thus
   dysfunctional). The question has arisen whether this is even the job of
   litestep.exe or if it should be moved to a separate app. Please comment if
   you have any thoughts on this.

 - LSMail can interfere with other modules.

 - Loading liteman may cause LS to freeze after locking the workstation.

 - Threading issues in (hopefully) rare circumstances.

 - Fullscreen app detection suffers from false positives. Because of that it may
   be disabled by default in the final release. Any thoughts?

 - #include dependencies in lsapi.h make it very difficult to distribute lsapi.h
   with a module's sourcecode.

 - TrayService doesn't catch all icons. Blank entries in the systray.

 - LS locks up if a PNG is used and libpng13.dll or zlib.dll is not present.


-- FINAL WORDS -----------------------------------------------------------------

 Thanks to Phil Stopford for hosting the binaries, and to everyone for the
 immensely use- and helpful feedback. I don't think I have ever received such
 detailed bugreports before. You're definitely a huge help for shaping up
 0.24.7.
 Please direct any suggestions/bug reports to ilmcuts@gmx.net and/or post on the
 LS Mailing List.
 Thanks for your support,

 The LiteStep Development Team.
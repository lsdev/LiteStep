=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    LiteStep 0.24.8
    Readme
    
    Copyright (c) 2009 The LiteStep Development Team
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

-- PREFACE ---------------------------------------------------------------------

 Your friendly devteam is proud to announce the release of LiteStep 0.24.8
 Final.  Changes since Release Candidate 1 are listed below as usual.  Refer to
 changes.txt for a list of changes since 0.24.7.  Refer to release_notes.txt for
 complete documentation on this release.

 LiteStep 0.24.8 Final is our last planned 0.24.x series release.  The next
 release series will be 0.25.x and will have full support for Windows Vista, but
 will no longer support Windows 95.  If you are currently running Windows Vista
 you will want to look at our current unstable releases.  Please visit our
 home page (link at bottom) for downloads.

-- CHANGES SINCE 0.24.8 RELEASE CANDIDATE 1 ------------------------------------

 - Fixed Appbar crashes

 - Fixed several Startup Runner issues.

 - Fixed the Step.rc Evaluation Parser's abs() function from converting the
   input to an integer.  Now abs(-1.23) will return 1.23 instead of 1.

 - Fixed LiteStep from shutting down too early if an application blocks the
   shutdown process which would leave the user with an empty desktop.

 - Fixed !about dialog's font display under Windows 95.

 - Fixed a regression from 0.24.7 that was introduced in 0.24.8 RC1 which caused
   systray tooltips and infotips to be broken on certain locales.

 - Fixed several API bugs: NULL paramater crash in RemoveBangCommand() and
   invalid message format for LM_GETMINRECT and LM_APPCOMMAND.

 - Fixed hook.dll implementation.

 - Updated German language strings.

 - Updated !Confirm dialog's default caption to "!Confirm LiteStep".

 - Updated the !about dialog's System Information page to show >2GB of memory
   statistics if needed and to show partial gigabytes.

 - Updated !about dialog's System Information page to support "Windows 2000" OS
   version.

 - Greatly improved the full screen application detection code.  It now detects
   any full screen application on the primary monitor correctly and hides or
   shows all modules accordingly (when LSAutoHideModules is enabled).

 - Appbar implementation now support multiple monitor configurations.

 - Added LiteStep version resource information to hook.dll.

 - Upgraded to libpng version 1.2.34.

 - Brought the VC7.1 and MinGW build files up to date.

-- FINAL WORDS -----------------------------------------------------------------

 Thanks to the LiteStep Community for all the positive feedback and support and
 for the endless patience.  Once again it has taken a long time, but LiteStep
 0.24.8 has finally arrived.

 Please send any suggestions or bug reports to the LSDev team contact address
 at: devteam <at> lsdev.org

 When reporting a bug, please include:
 (use the !About dialog to retrieve version information)

  - Windows OS Version and Service Pack level
  - LiteStep Core Version
  - All currently loaded modules and their versions
  - Theme name and version (if applicable)
  - Your contact information (email address)
  - A complete description of the bug and if possible a step-by-step procedure
    to reproduce the bug.  If we can not reproduce the bug, we will not be able
    to fix it!  So, please do your best to help us help you.  Thanks!

 Happy LiteStepping,

 - The LiteStep Development Team
   http://www.lsdev.org/

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

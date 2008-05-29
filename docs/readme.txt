=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    LiteStep 0.24.8 Release Candidate 1
    Readme
    
    Copyright (c) 2008 - LiteStep Development Team
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


-- PREFACE ---------------------------------------------------------------------

 Your friendly devteam is proud to announce the release of LiteStep 0.24.8
 Release Candidate 1.  Major changes since 0.24.7 are listed below.  Refer to
 release_notes.txt for complete documentation on this release.


-- CHANGES SINCE 0.24.7 --------------------------------------------------------

 - New evaluation parser

   With the evaluation parser, both conditional expressions (If, ElseIf) and
   inline expressions ($a + b$) now use the same syntax.  The new parser also
   adds some new capabilities that were previously unavailable such as strings
   and builtin functions.

   The new syntax should be backwards compatible with existing files, but this
   is new code and there are bound to be bugs.  So, we're needing users to try
   out the new code and report bugs back to us.  Also be sure to try out some of
   the new features and provide feedback on those as well.  The new syntax is
   documented in the "release_notes.txt".

 - Appbar support

   We have added an implementation to support AppBars.  This is a feature found
   in the Explorer shell which some applications use to dock their window to the
   edge of a screen (e.g the Explorer taskbar).  Some typical applications that
   use this feature are Trillian and various media players.

   This also adds support for WorkArea updates when an AppBar is positioned or
   removed.  Please test the interaction between AppBars adjusting the WorkArea
   and the Desktop modules adjusting the WorkArea.  Hopefully they will interact
   nicely.  What you should expect, is if the desktop module has changed the
   WorkArea for a specific screen edge, then any AppBar placed on the same
   screen edge will be positioned so that it does not overlap the old workarea.
   This implementation appears to work in practice in the limited testing we
   were able to do.  Please let us know if you find any odd or undesirable
   behavior in regards to the WorkArea and AppBars.

 - Icon notification tray (systray) service fixes

   The Icon Notification Tray service has been updated to fix several issues.
   The most visible one being where old icons and sometimes balloon (info)
   tips would appear during a recycle.  Please verify that all expected
   functionality still works.

 - Added checks for recursively defined variables to VarExpansionEx.

 - Removed !gather bang command proxy for VWM modules (all current VWM modules
   now implement that function internally).

 - Improvements to LSAutoHideModules.


-- FINAL WORDS -----------------------------------------------------------------

 Thanks to the LiteStep Community for all the positive feedback and support and
 for the endless patience.  Once again it has taken a long time, but LiteStep
 0.24.8 is finally on its way.

 If you upgrade to this version from 0.24.7 Final, and *anything* breaks, please
 let us know.  We intend for 0.24.8 to be drop-in-compatible with all existing
 valid configurations.

 Please send any suggestions or bug reports to the LSDev team contact address
 at: devteam <at> lsdev.org

 When reporting a bug, please include:
 (use the !About dialog to retrieve version information)

  - LiteStep Core Version
  - All currently loaded modules and their versions
  - Theme name and version (if applicable)
  - Your contact information (email address)
  - A complete description of the issueand if possible a step-by-step procedure
    to reproduce the bug.  If we can not reproduce the bug, we will not be able
    to fix it!  So, please do your best to help us help you.  Thanks!

 Happy LiteStepping,

 - The LiteStep Development Team
   http://www.lsdev.org/

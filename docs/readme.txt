=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    LiteStep 0.24.7 Beta 1
    Readme/Release Notes
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


-- PREFACE ---------------------------------------------------------------------

 Here's a little update from your friendly devteam. This build was supposed to
 be released as "RC 1", but a few bugs (including a very nasty threading bug)
 were discovered literally in the last minutes. We're not entirely sure that the
 bug was fixed, so it was decided to play safe and label this as a "Beta" and
 not as a "Release Candidate." It is assumed that you are familiar with the last
 0.24.7 build, as the rest of this document will focus on the differences
 between that build and this beta.
 For experienced users only; installation instructions are not included. As
 always, use at your own risk.
 This document only covers the most important issues, for details check (the
 rewritten yet slightly incomplete) changes.txt.


-- CHANGES SINCE THE LAST BUILD ------------------------------------------------

 Compared to Message's build from September you'll find very little new
 features, but a HUGE amount of compatibility fixes. Those fixes aren't
 mentioned in the changelog despite their importance, as 0.24.7 is supposed to
 be a drop-in replacement for 0.24.6; hence a compatibility fix is no "change"
 to be recorded in changes.txt.
 In addition to these mostly internal compatibility fixes the most important
 changes are:

 - Modules are no longer threaded by default, which leads to a drop in !recycle
   and startup performance but ensures much better compatibility to modules not
   prepared for threading (sadly the number of such modules is bigger than we
   initially assumed). However startup/!recycle performance should still be
   better than under 0.24.6.

 - Added basic math evaluation and even more basic logging support.

 - Fixed !reload, !refresh, and !reloadmodule.

 - More efficient graphics functions in lsapi.

 - There is one new step.rc setting called "LSNoAutoHideModules" which disables
   the automatic module hiding if a fullscreen app is detected.


-- WHAT NEEDS TO BE TESTED -----------------------------------------------------

 - Ability to be used as a drop-in replacement for 0.24.6. You should not have
   to make any changes in your step.rc when upgrading from 0.24.6 or Indie LS,
   except to work around the missing features listed in changes.txt. If you
   experience any problems during this process please report them.

 - The fullscreen app detection. It seems to suffer from false positives, which
   means it sometimes triggers and this hides all modules despite there being no
   (visible) fullscreen app; for example while the Object Dock installer is
   running. If you find other programs reprodcuably (and wrongly) triggering the
   fullscreen app detection please let us know.

 - !Reloadmodule.

 - !Recycle with three or more "threaded" modules.

 - jKey (especially if used with vk104.txt), LSMail, jAmpToo compatibility.


-- KNOWN PROBLEMS --------------------------------------------------------------

 This is NOT a complete list but should cover the most important points:

 - No aboutbox (or rather, no functional aboutbox).

 - -install/-uninstall command line switches not finished (and thus
   disfunctional). The question has arisen whether this is even the job of
   litestep.exe or if it should be moved to a separate app. Please comment if
   you have any thoughts on this.

 - jKey, LSMail, and jAmpToo were a bit unstable at times.

 - Threading issues in (hopefully) rare circumstances.


-- FINAL WORDS -----------------------------------------------------------------

 Massive thanks to Phil Stopford for hosting the binaries, and to the testers,
 without you 0.24.7 would still only run a handful of themes. Please direct any
 suggestions/bug reports to ilmcuts@gmx.net and/or post on the LS Mailing List.
 Have fun.

 The LiteStep Development Team.
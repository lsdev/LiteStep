=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    LiteStep 0.24.7 Release Candidate 2
    Readme/Release Notes
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


-- PREFACE ---------------------------------------------------------------------

 Another update from your friendly devteam has arrived. This is Release
 Candidate 2 of LiteStep 0.24.7. As usual the changes since the last release
 (RC 1) are listed below; if you want a list of changes since 0.24.6 please
 check changes.txt.

 Unless there is a showstopper in this build it will be relabelled to be the
 final build of 0.24.7. If you do find such bugs please let us know by email
 (address is at the end of this file) or by posting on the LS Mailing List.
 Installation instructions are not included. Use at your own risk.


-- CHANGES SINCE RELEASE CANDIDATE 1 -------------------------------------------

 - Fixed bug in the tray code causing 9x/ME to crash (many thanks to Clint
   Foster who tracked this down)

 - No more error message on startup if iernonce.dll (RunOnceEx support) doesn't
   exist, this may happen on Win95 setups

 - Fixed version resource reporting 0.2.4.7 instead of 0.24.7.0

 - Fixed a small memory leak in the DDEService

 - The !Run implementation is less prone to crash

 - Fixed potential buffer overflows in !alert, !confirm, !unloadmodule, and
   !reloadmodule

 - Fixed fileparser stopping parse process if the first token on a non-comment
   line was longer than 63 characters

 - Made a few APIs more robust (less likely to crash if bad parameters are
   passed)

 - Fixed LCOpen not returning NULL if file doesn't exist

 - Added new step.rc setting "LSTrayService". "LSTrayService FALSE" disables
   the core's system tray support. Defaults to TRUE. Most useful if LS is run
   ontop of other shells that implement their own tray.


-- WHAT NEEDS TO BE TESTED -----------------------------------------------------

 - Windows 9x/ME compatibility


-- FINAL WORDS -----------------------------------------------------------------

 Thanks to Phil Stopford for hosting the binaries, and to everyone for the
 great feedback. 

 Please send any suggestions or bug reports to ilmcuts at gmx dot net and/or
 post on the LS Mailing List (http://wuzzle.org/list/litestep.php or
 news://news.gmane.org/gmane.comp.windows.shells.litestep).
 
 Happy litestepping,

 The LiteStep Development Team
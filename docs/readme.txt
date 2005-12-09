=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    LiteStep 0.24.7 Release Candidate 4
    Readme/Release Notes
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


-- PREFACE ------------------------------------------------------------

 Your friendly devteam is proud to announce Release Candidate 4 of
 LiteStep 0.24.7. Changes since Release Candidate 3 are listed below as
 usual. Refer to changes.txt for a a list of changes since 0.24.6.

 This will hopefully be the last Release Candidate before the Final
 0.24.7 release planned late next week. Please let us know right away
 if you find any showstoppers.

 Installation instructions are not included. Use at your own risk.


-- CHANGES SINCE RELEASE CANDIDATE 3 ----------------------------------

 - Fixed libpng/lsapi CRT dependency mismatch

 - Fixed potential log buffer overflow (Thanks Hollow)

 - Fixed missing/ invalid Win32 error messages during module load

 - Fixed several issues with the TrayService

 - Fixed thread race condition in BangManager (Thanks luthe)

 - Fixed various buffer and pointer access issues

 - Fixed ModuleManager reporting a module loaded, that doesn't exist

 - Updated string table translations (many thanks to LSML members)

 - Updated libpng13.dll to version 1.2.8 and zlib.dll to version 1.2.3

 - Added Makefile for MinGW

 - Added VersionInfo resource to lsapi.dll
 
 - Various minor fixes


-- FINAL WORDS --------------------------------------------------------

 Thanks to the LiteStep Community for all the positive feedback and
 support; and for the endless patience. It has taken a long time, but
 we may yet see a Final Release.

 Please send any suggestions or bug reports to the LS Mailing List or
 use the lsdev.org forums. You can also send a private email to ilmcuts
 at gmx dot net and please CC jugg at litestep dot net.

 Happy LiteStepping,

 - The Litestep Development Team  (www.lsdev.org)


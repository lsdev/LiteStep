# Microsoft Developer Studio Project File - Name="litestep" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=litestep - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "litestep.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "litestep.mak" CFG="litestep - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "litestep - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "litestep - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "litestep - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /WX /GX /O2 /I "../lsapi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "NDEBUG" /o /win32 "NUL"
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ..\lsapi\release\lsapi.lib ../hook/release/hook.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib kernel32.lib user32.lib delayimp.lib shlwapi.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /opt:NOWIN98 /delayload:"hook.dll"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy                         /y                         ..\*.txt                         release\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "litestep - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /WX /GX /ZI /Od /I "../lsapi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FAs /FR /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "_DEBUG" /o /win32 "NUL"
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\lsapi\debug\lsapi.lib ../hook/debug/hook.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib kernel32.lib user32.lib delayimp.lib shlwapi.lib /nologo /subsystem:windows /profile /debug /machine:I386 /nodefaultlib:"libcd.lib" /delayload:"hook.dll"

!ENDIF 

# Begin Target

# Name "litestep - Win32 Release"
# Name "litestep - Win32 Debug"
# Begin Group "Source"

# PROP Default_Filter "*.cpp;*.c"
# Begin Source File

SOURCE=..\lsapi\BangCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\DataStore.cpp
# End Source File
# Begin Source File

SOURCE=.\DDEService.cpp
# End Source File
# Begin Source File

SOURCE=.\DDEWorker.cpp
# End Source File
# Begin Source File

SOURCE=.\HookManager.cpp
# End Source File
# Begin Source File

SOURCE=.\litestep.cpp
# End Source File
# Begin Source File

SOURCE=.\MessageManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Module.cpp
# End Source File
# Begin Source File

SOURCE=.\ModuleManager.cpp
# End Source File
# Begin Source File

SOURCE=.\RecoveryMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\StartupRunner.cpp
# End Source File
# Begin Source File

SOURCE=.\TrayService.cpp
# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\buildoptions.h
# End Source File
# Begin Source File

SOURCE=.\DataStore.h
# End Source File
# Begin Source File

SOURCE=.\DDEService.h
# End Source File
# Begin Source File

SOURCE=.\DDEWorker.h
# End Source File
# Begin Source File

SOURCE=.\HookManager.h
# End Source File
# Begin Source File

SOURCE=.\litestep.h
# End Source File
# Begin Source File

SOURCE=.\MessageManager.h
# End Source File
# Begin Source File

SOURCE=.\Module.h
# End Source File
# Begin Source File

SOURCE=.\ModuleManager.h
# End Source File
# Begin Source File

SOURCE=.\RecoveryMenu.h
# End Source File
# Begin Source File

SOURCE=.\StartupRunner.h
# End Source File
# Begin Source File

SOURCE=.\TrayService.h
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter "*.rc;*.bmp;*.ico;*.h"
# Begin Source File

SOURCE=.\litestep.bmp
# End Source File
# Begin Source File

SOURCE=.\litestep.ico
# End Source File
# Begin Source File

SOURCE=.\litestep.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# End Target
# End Project

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

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Release_VC6"
# PROP Intermediate_Dir "Release_VC6"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /WX /GX /Zd /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "LSAPI_PRIVATE" /FD /c
# ADD MTL /nologo /D "NDEBUG" /o "NUL" /win32
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD LINK32 advapi32.lib comdlg32.lib delayimp.lib gdi32.lib kernel32.lib msvcprt.lib msvcrt.lib ole32.lib oleaut32.lib shell32.lib shlwapi.lib user32.lib uuid.lib ..\Release_VC6\hook.lib ..\Release_VC6\lsapi.lib /nologo /subsystem:windows /machine:I386 /pdb:none /map:"../Release_VC6/litestep.map" /nodefaultlib /delayload:"hook.dll" /opt:NOWIN98 /MAPINFO:LINES /MAPINFO:EXPORTS

!ELSEIF  "$(CFG)" == "litestep - Win32 Debug"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Debug_VC6"
# PROP Intermediate_Dir "Debug_VC6"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "LSAPI_PRIVATE" /D "LS_NO_EXCEPTION" /FR /FD /GZ /c
# ADD MTL /nologo /D "_DEBUG" /o "NUL" /win32
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD LINK32 advapi32.lib comdlg32.lib delayimp.lib gdi32.lib kernel32.lib msvcprtd.lib msvcrtd.lib ole32.lib oleaut32.lib shell32.lib shlwapi.lib user32.lib uuid.lib ..\Debug_VC6\hook.lib ..\Debug_VC6\lsapi.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /nodefaultlib /delayload:"hook.dll"

!ENDIF 

# Begin Target

# Name "litestep - Win32 Release"
# Name "litestep - Win32 Debug"
# Begin Group "Source"

# PROP Default_Filter "*.cpp;*.c"
# Begin Source File

SOURCE=.\DataStore.cpp
# End Source File
# Begin Source File

SOURCE=.\DDEService.cpp
# End Source File
# Begin Source File

SOURCE=.\DDEStub.cpp
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

SOURCE=.\TrayNotifyIcon.cpp
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

SOURCE=.\DDEStub.h
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

SOURCE=.\TrayAppBar.h
# End Source File
# Begin Source File

SOURCE=.\TrayNotifyIcon.h
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

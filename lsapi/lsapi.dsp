# Microsoft Developer Studio Project File - Name="lsapi" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=lsapi - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lsapi.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lsapi.mak" CFG="lsapi - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lsapi - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "lsapi - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "lsapi - Win32 Release Without PNG" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "lsapi - Win32 Debug Without PNG " (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath "Desktop"
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "lsapi - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /WX /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "LSAPI_INTERNAL" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /o "NUL" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 delayimp.lib ../../libpng/projects/msvc/win32/libpng/dll/libpng13.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib version.lib kernel32.lib user32.lib shlwapi.lib /nologo /base:"0x13000000" /subsystem:windows /dll /machine:I386 /out:"..\litestep\Release\lsapi.dll" /opt:NOWIN98 /delayload:"libpng13.dll"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "lsapi - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /WX /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "LSAPI_INTERNAL" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /o "NUL" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 delayimp.lib ../../libpng/projects/msvc/win32/libpng/dll/libpng13.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib version.lib kernel32.lib user32.lib shlwapi.lib /nologo /subsystem:windows /dll /profile /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"..\litestep\Debug\lsapi.dll" /delayload:"libpng13.dll"

!ELSEIF  "$(CFG)" == "lsapi - Win32 Release Without PNG"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "lsapi___Win32_Release_Without_PNG"
# PROP BASE Intermediate_Dir "lsapi___Win32_Release_Without_PNG"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "LSAPI_INTERNAL" /FR /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /WX /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "LSAPI_INTERNAL" /D "_NO_PNG_SUPPORT" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /o "NUL" /win32
# SUBTRACT BASE MTL /mktyplib203
# ADD MTL /nologo /D "NDEBUG" /o "NUL" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib version.lib kernel32.lib user32.lib /nologo /base:"0x13000000" /subsystem:windows /dll /machine:I386 /out:"..\litestep\Release\lsapi.dll" /opt:NOWIN98
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib version.lib kernel32.lib user32.lib shlwapi.lib /nologo /base:"0x13000000" /subsystem:windows /dll /machine:I386 /out:"..\litestep\Release\lsapi.dll" /opt:NOWIN98
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "lsapi - Win32 Debug Without PNG "

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "lsapi___Win32_Debug_Without_PNG_"
# PROP BASE Intermediate_Dir "lsapi___Win32_Debug_Without_PNG_"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "LSAPI_INTERNAL" /FR /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "LSAPI_INTERNAL" /D "_NO_PNG_SUPPORT" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /o "NUL" /win32
# SUBTRACT BASE MTL /mktyplib203
# ADD MTL /nologo /D "_DEBUG" /o "NUL" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib version.lib kernel32.lib user32.lib /nologo /subsystem:windows /dll /profile /debug /machine:I386 /out:"..\litestep\Debug\lsapi.dll"
# ADD LINK32 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib version.lib kernel32.lib user32.lib shlwapi.lib /nologo /subsystem:windows /dll /profile /debug /machine:I386 /out:"..\litestep\Debug\lsapi.dll"

!ENDIF 

# Begin Target

# Name "lsapi - Win32 Release"
# Name "lsapi - Win32 Debug"
# Name "lsapi - Win32 Release Without PNG"
# Name "lsapi - Win32 Debug Without PNG "
# Begin Group "Source"

# PROP Default_Filter "*.cpp;*.c"
# Begin Source File

SOURCE=.\BangCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\litestep\BangManager.cpp
# End Source File
# Begin Source File

SOURCE=.\bangs.cpp
# End Source File
# Begin Source File

SOURCE=.\graphics.cpp
# End Source File
# Begin Source File

SOURCE=.\lsapi.cpp
# End Source File
# Begin Source File

SOURCE=.\lsMultiMon.cpp
# End Source File
# Begin Source File

SOURCE=.\match.cpp
# End Source File
# Begin Source File

SOURCE=.\png_support.cpp

!IF  "$(CFG)" == "lsapi - Win32 Release"

!ELSEIF  "$(CFG)" == "lsapi - Win32 Debug"

!ELSEIF  "$(CFG)" == "lsapi - Win32 Release Without PNG"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "lsapi - Win32 Debug Without PNG "

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\settings.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsEvalParser.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsFileParser.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsIterator.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsManager.cpp
# End Source File
# Begin Source File

SOURCE=.\stubs.cpp
# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\BangCommand.h
# End Source File
# Begin Source File

SOURCE=..\litestep\BangManager.h
# End Source File
# Begin Source File

SOURCE=.\bangs.h
# End Source File
# Begin Source File

SOURCE=.\lsapi.h
# End Source File
# Begin Source File

SOURCE=.\lsapidefines.h
# End Source File
# Begin Source File

SOURCE=.\lsMultiMon.h
# End Source File
# Begin Source File

SOURCE=.\match.h
# End Source File
# Begin Source File

SOURCE=.\png_support.h
# End Source File
# Begin Source File

SOURCE=.\SettingsDefines.h
# End Source File
# Begin Source File

SOURCE=.\SettingsEvalParser.h
# End Source File
# Begin Source File

SOURCE=.\SettingsFileParser.h
# End Source File
# Begin Source File

SOURCE=.\SettingsIterator.h
# End Source File
# Begin Source File

SOURCE=.\SettingsManager.h
# End Source File
# Begin Source File

SOURCE=.\ThreadedBangCommand.h
# End Source File
# End Group
# End Target
# End Project

# Microsoft Developer Studio Project File - Name="utility" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=utility - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "utility.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "utility.mak" CFG="utility - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "utility - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "utility - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "utility - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_VC6"
# PROP Intermediate_Dir "Release_VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /WX /GX /Zd /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "utility - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_VC6"
# PROP Intermediate_Dir "Debug_VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_LIB" /D "LS_NO_EXCEPTION" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "utility - Win32 Release"
# Name "utility - Win32 Debug"
# Begin Group "Source"

# PROP Default_Filter "*.cpp;*.c"
# Begin Source File

SOURCE=.\localization.cpp
# End Source File
# Begin Source File

SOURCE=.\lswinbase.cpp
# End Source File
# Begin Source File

SOURCE=.\lswinbasex.cpp
# End Source File
# Begin Source File

SOURCE=.\safeptr.cpp
# End Source File
# Begin Source File

SOURCE=.\shellhlp.cpp
# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\Base.h
# End Source File
# Begin Source File

SOURCE=.\common.h
# End Source File
# Begin Source File

SOURCE=.\core.hpp
# End Source File
# Begin Source File

SOURCE=.\criticalsection.h
# End Source File
# Begin Source File

SOURCE=.\debug.hpp
# End Source File
# Begin Source File

SOURCE=.\ILiteStep.h
# End Source File
# Begin Source File

SOURCE=.\IManager.h
# End Source File
# Begin Source File

SOURCE=.\IService.h
# End Source File
# Begin Source File

SOURCE=.\localization.h
# End Source File
# Begin Source File

SOURCE=.\lswinbase.h
# End Source File
# Begin Source File

SOURCE=.\lswinbasex.h
# End Source File
# Begin Source File

SOURCE=.\macros.h
# End Source File
# Begin Source File

SOURCE=.\safeptr.h
# End Source File
# Begin Source File

SOURCE=.\safestr.h
# End Source File
# Begin Source File

SOURCE=.\shellhlp.h
# End Source File
# Begin Source File

SOURCE=.\stringutility.h
# End Source File
# Begin Source File

SOURCE=.\wharfdata.h
# End Source File
# End Group
# End Target
# End Project

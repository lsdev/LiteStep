#-----------------------------------------------------------------------------
# Makefile for MinGW
#
# To make a release build:     make
# To make a debug build:       make DEBUG=1
# To clean up a release build: make clean
# To clean up a debug build:   make clean DEBUG=1
#
# The output directory (Release or Debug by default) must exist before you
# run make. For 'make clean' to work you need rm.exe.
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Tools and Flags
#-----------------------------------------------------------------------------

# C++ compiler
CXX = g++

# C++ compiler flags
ifdef DEBUG
CXXFLAGS = -Wall -DLSAPI_PRIVATE -D_DEBUG -g
else
CXXFLAGS = -Wall -DLSAPI_PRIVATE -DNDEBUG
endif

# Linker flags
ifdef DEBUG
LDFLAGS = -mwindows
else
LDFLAGS = -mwindows -s
endif

# Resource compiler
RC = windres

# Resource compiler flags
RCFLAGS = -O coff

# dllwrap
DLLWRAP = dllwrap

# rm
RM = rm -f

ifeq ($(OS), Windows_NT)
NULL = 
else
NULL = nul
endif

#-----------------------------------------------------------------------------
# Files and Paths
#-----------------------------------------------------------------------------

# Search path for dependencies
VPATH = litestep;lsapi;utility

# Output directory
ifdef DEBUG
OUTPUT = Debug
else
OUTPUT = Release
endif

# Path to litestep.exe
EXE = $(OUTPUT)/litestep.exe

# Libraries that litestep.exe uses
EXELIBS = -L$(OUTPUT) -llsapi -lole32 -lshlwapi -luuid

# Object files for litestep.exe
EXEOBJS = \
	$(OUTPUT)/DataStore.o \
	$(OUTPUT)/DDEService.o \
	$(OUTPUT)/DDEStub.o \
	$(OUTPUT)/DDEWorker.o \
	$(OUTPUT)/litestep.o \
	$(OUTPUT)/litestep.res \
	$(OUTPUT)/localization.o \
	$(OUTPUT)/MessageManager.o \
	$(OUTPUT)/Module.o \
	$(OUTPUT)/ModuleManager.o \
	$(OUTPUT)/RecoveryMenu.o \
	$(OUTPUT)/safeptr.o \
	$(OUTPUT)/shellhlp.o \
	$(OUTPUT)/StartupRunner.o \
	$(OUTPUT)/TrayNotifyIcon.o \
	$(OUTPUT)/TrayService.o

# Path to lsapi.dll
DLL = $(OUTPUT)/lsapi.dll

# Path to lsapi.dll export definitions file
DLLDEF = lsapi/lsapi_mingw.def

# Path to lsapi.dll import library
DLLIMPLIB = $(OUTPUT)/liblsapi.a

# Libraries that lsapi.dll uses
DLLLIBS = -lole32 -lpng -lshlwapi -lz

# Object files for lsapi.dll
DLLOBJS = \
	$(OUTPUT)/aboutbox.o \
	$(OUTPUT)/BangCommand.o \
	$(OUTPUT)/BangManager.o \
	$(OUTPUT)/bangs.o \
	$(OUTPUT)/graphics.o \
	$(OUTPUT)/localization.o \
	$(OUTPUT)/lsapi.o \
	$(OUTPUT)/lsapi.res \
	$(OUTPUT)/lsapiInit.o \
	$(OUTPUT)/lsmultimon.o \
	$(OUTPUT)/match.o \
	$(OUTPUT)/MathEvaluate.o \
	$(OUTPUT)/MathParser.o \
	$(OUTPUT)/MathScanner.o \
	$(OUTPUT)/MathToken.o \
	$(OUTPUT)/MathValue.o \
	$(OUTPUT)/png_support.o \
	$(OUTPUT)/safeptr.o \
	$(OUTPUT)/settings.o \
	$(OUTPUT)/SettingsFileParser.o \
	$(OUTPUT)/SettingsIterator.o \
	$(OUTPUT)/SettingsManager.o \
	$(OUTPUT)/shellhlp.o \
	$(OUTPUT)/stubs.o

#-----------------------------------------------------------------------------
# Rules
#-----------------------------------------------------------------------------

# all targets
.PHONY: all
all: setup $(DLL) $(EXE)

# litestep.exe
$(EXE): setup $(EXEOBJS)
	$(CXX) -o $(EXE) $(LDFLAGS) $(EXEOBJS) $(EXELIBS)

# lsapi.dll
$(DLL): setup $(DLLOBJS) $(DLLDEF)
	$(DLLWRAP) --driver-name $(CXX) --def $(DLLDEF) --implib $(DLLIMPLIB) -o $(DLL) $(LDFLAGS) $(DLLOBJS) $(DLLLIBS)

# Setup environment
.PHONY: setup
setup:
	-@if not exist $(OUTPUT)/$(NULL) mkdir $(OUTPUT)

# Remove output files
.PHONY: clean
clean:
	-$(RM) $(EXE)
	-$(RM) $(DLL)
	-$(RM) $(DLLIMPLIB)
	-$(RM) $(OUTPUT)/*.o
	-$(RM) $(OUTPUT)/*.res

# Resources for litestep.exe
$(OUTPUT)/litestep.res: litestep/litestep.rc litestep/resource.h litestep/litestep.bmp litestep/litestep.ico
	$(RC) -Ilitestep $(RCFLAGS) -o $@ $<

# Resources for lsapi.dll
$(OUTPUT)/lsapi.res: lsapi/lsapi.rc lsapi/resource.h
	$(RC) -Ilsapi $(RCFLAGS) -o $@ $<

# Pattern rule to compile cpp files
$(OUTPUT)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

#-----------------------------------------------------------------------------
# Dependencies
#-----------------------------------------------------------------------------

$(OUTPUT)/DataStore.o: litestep/DataStore.cpp \
	litestep/DataStore.h \
	litestep/buildoptions.h \
	lsapi/lsapidefines.h \
	utility/common.h \
	utility/debug.hpp \
	utility/safeptr.h

$(OUTPUT)/DDEService.o: litestep/DDEService.cpp \
	litestep/buildoptions.h \
	litestep/DDEService.h \
	litestep/DDEWorker.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	utility/Base.h \
	utility/common.h \
	utility/debug.hpp \
	utility/IService.h \
	utility/safestr.h \
	utility/shellhlp.h

$(OUTPUT)/DDEStub.o: litestep/DDEStub.cpp \
	litestep/buildoptions.h \
	litestep/DDEStub.h \
	lsapi/lsapidefines.h \
	utility/Base.h \
	utility/common.h \
	utility/debug.hpp \
	utility/IService.h \
	utility/safestr.h

$(OUTPUT)/DDEWorker.o: litestep/DDEWorker.cpp \
	litestep/buildoptions.h \
	litestep/DDEWorker.h \
	litestep/resource.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	utility/common.h \
	utility/core.hpp \
	utility/debug.hpp \
	utility/localization.h \
	utility/safeptr.h \
	utility/safestr.h \
	utility/shellhlp.h

$(OUTPUT)/litestep.o: litestep/litestep.cpp \
	litestep/buildoptions.h \
	litestep/DataStore.h \
	litestep/DDEService.h \
	litestep/DDEStub.h \
	litestep/DDEWorker.h \
	litestep/litestep.h \
	litestep/MessageManager.h \
	litestep/Module.h \
	litestep/ModuleManager.h \
	litestep/RecoveryMenu.h \
	litestep/resource.h \
	litestep/StartupRunner.h \
	litestep/TrayAppBar.h \
	litestep/TrayNotifyIcon.h \
	litestep/TrayService.h \
	lsapi/BangCommand.h \
	lsapi/lsapi.h \
	lsapi/lsapiInit.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	lsapi/ThreadedBangCommand.h \
	utility/Base.h \
	utility/common.h \
	utility/core.hpp \
	utility/CriticalSection.h \
	utility/debug.hpp \
	utility/ILiteStep.h \
	utility/IManager.h \
	utility/IService.h \
	utility/localization.h \
	utility/macros.h \
	utility/safeptr.h \
	utility/safestr.h \
	utility/shellhlp.h

$(OUTPUT)/MessageManager.o: litestep/MessageManager.cpp \
	litestep/buildoptions.h \
	litestep/MessageManager.h \
	lsapi/lsapidefines.h \
	utility/common.h \
	utility/CriticalSection.h \
	utility/debug.hpp

$(OUTPUT)/Module.o: litestep/Module.cpp \
	litestep/buildoptions.h \
	litestep/Module.h \
	litestep/resource.h \
	lsapi/BangCommand.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	lsapi/ThreadedBangCommand.h \
	utility/Base.h \
	utility/common.h \
	utility/core.hpp \
	utility/debug.hpp \
	utility/localization.h \
	utility/macros.h \
	utility/safeptr.h \
	utility/safestr.h

$(OUTPUT)/ModuleManager.o: litestep/ModuleManager.cpp \
	litestep/buildoptions.h \
	litestep/Module.h \
	litestep/ModuleManager.h \
	litestep/resource.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	utility/Base.h \
	utility/common.h \
	utility/core.hpp \
	utility/debug.hpp \
	utility/ILiteStep.h \
	utility/IManager.h \
	utility/localization.h \
	utility/macros.h \
	utility/safeptr.h \
	utility/safestr.h

$(OUTPUT)/RecoveryMenu.o: litestep/RecoveryMenu.cpp \
	litestep/buildoptions.h \
	litestep/RecoveryMenu.h \
	litestep/resource.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	utility/common.h \
	utility/debug.hpp

$(OUTPUT)/StartupRunner.o: litestep/StartupRunner.cpp \
	litestep/buildoptions.h \
	litestep/StartupRunner.h \
	litestep/resource.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	utility/common.h \
	utility/core.hpp \
	utility/debug.hpp \
	utility/localization.h \
	utility/safeptr.h \
	utility/safestr.h \
	utility/shellhlp.h

$(OUTPUT)/TrayNotifyIcon.o: litestep/TrayNotifyIcon.cpp \
	litestep/buildoptions.h \
	litestep/TrayNotifyIcon.h \
	lsapi/lsapidefines.h \
	utility/common.h \
	utility/debug.hpp \
	utility/safestr.h

$(OUTPUT)/TrayService.o: litestep/TrayService.cpp \
	litestep/buildoptions.h \
	litestep/resource.h \
	litestep/TrayAppBar.h \
	litestep/TrayNotifyIcon.h \
	litestep/TrayService.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	utility/Base.h \
	utility/common.h \
	utility/core.hpp \
	utility/debug.hpp \
	utility/IService.h \
	utility/localization.h \
	utility/macros.h \
	utility/safeptr.h \
	utility/safestr.h \
	utility/shellhlp.h

$(OUTPUT)/aboutbox.o: lsapi/aboutbox.cpp \
	litestep/buildoptions.h \
	litestep/resource.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	utility/common.h \
	utility/core.hpp \
	utility/debug.hpp \
	utility/localization.h \
	utility/safeptr.h \
	utility/safestr.h

$(OUTPUT)/BangCommand.o: lsapi/BangCommand.cpp \
	litestep/buildoptions.h \
	litestep/resource.h \
	lsapi/BangCommand.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	lsapi/ThreadedBangCommand.h \
	utility/Base.h \
	utility/common.h \
	utility/core.hpp \
	utility/debug.hpp \
	utility/localization.h \
	utility/safeptr.h \
	utility/safestr.h

$(OUTPUT)/BangManager.o: lsapi/BangManager.cpp \
	litestep/buildoptions.h \
	lsapi/BangCommand.h \
	lsapi/BangManager.h \
	lsapi/lsapidefines.h \
	utility/Base.h \
	utility/common.h \
	utility/CriticalSection.h \
	utility/debug.hpp \
	utility/stringutility.h

$(OUTPUT)/bangs.o: lsapi/bangs.cpp \
	litestep/buildoptions.h \
	litestep/resource.h \
	lsapi/bangs.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	utility/common.h \
	utility/core.hpp \
	utility/debug.hpp \
	utility/localization.h \
	utility/safeptr.h \
	utility/safestr.h

$(OUTPUT)/graphics.o: lsapi/graphics.cpp \
	litestep/buildoptions.h \
	litestep/resource.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	lsapi/png_support.h \
	utility/common.h \
	utility/core.hpp \
	utility/debug.hpp \
	utility/localization.h \
	utility/safeptr.h \
	utility/safestr.h

$(OUTPUT)/lsapi.o: lsapi/lsapi.cpp \
	litestep/buildoptions.h \
	litestep/resource.h \
	lsapi/BangCommand.h \
	lsapi/BangManager.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsapiInit.h \
	lsapi/lsmultimon.h \
	lsapi/SettingsDefines.h \
	lsapi/SettingsIterator.h \
	lsapi/SettingsManager.h \
	utility/Base.h \
	utility/common.h \
	utility/core.hpp \
	utility/CriticalSection.h \
	utility/debug.hpp \
	utility/localization.h \
	utility/safeptr.h \
	utility/safestr.h \
	utility/shellhlp.h \
	utility/stringutility.h

$(OUTPUT)/lsapiInit.o: lsapi/lsapiInit.cpp \
	litestep/buildoptions.h \
	litestep/resource.h \
	lsapi/BangCommand.h \
	lsapi/BangManager.h \
	lsapi/bangs.h \
	lsapi/lsapidefines.h \
	lsapi/lsapi.h \
	lsapi/lsapiInit.h \
	lsapi/lsmultimon.h \
	lsapi/SettingsDefines.h \
	lsapi/SettingsIterator.h \
	lsapi/SettingsManager.h \
	utility/Base.h \
	utility/common.h \
	utility/core.hpp \
	utility/CriticalSection.h \
	utility/debug.hpp \
	utility/localization.h \
	utility/safeptr.h \
	utility/safestr.h \
	utility/shellhlp.h \
	utility/stringutility.h

$(OUTPUT)/lsmultimon.o: lsapi/lsmultimon.cpp \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h

$(OUTPUT)/match.o: lsapi/match.cpp \
	litestep/buildoptions.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	utility/common.h \
	utility/debug.hpp

$(OUTPUT)/MathEvaluate.o: lsapi/MathEvaluate.cpp

$(OUTPUT)/MathParser.o: lsapi/MathParser.cpp \
	litestep/buildoptions.h \
	lsapi/BangCommand.h \
	lsapi/BangManager.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsapiInit.h \
	lsapi/lsmultimon.h \
	lsapi/MathEvaluate.h \
	lsapi/MathException.h \
	lsapi/MathParser.h \
	lsapi/MathScanner.h \
	lsapi/MathToken.h \
	lsapi/MathValue.h \
	lsapi/SettingsDefines.h \
	lsapi/SettingsIterator.h \
	lsapi/SettingsManager.h \
	utility/Base.h \
	utility/common.h \
	utility/CriticalSection.h \
	utility/debug.hpp \
	utility/stringutility.h

$(OUTPUT)/MathScanner.o: lsapi/MathScanner.cpp \
	lsapi/MathException.h \
	lsapi/MathScanner.h \
	lsapi/MathToken.h

$(OUTPUT)/MathToken.o: lsapi/MathToken.cpp \
	lsapi/MathToken.h

$(OUTPUT)/MathValue.o: lsapi/MathValue.cpp \
	lsapi/MathValue.h

$(OUTPUT)/png_support.o: lsapi/png_support.cpp \
	litestep/buildoptions.h \
	lsapi/lsapidefines.h \
	lsapi/png_support.h \
	utility/common.h \
	utility/debug.hpp \
	utility/safeptr.h

$(OUTPUT)/settings.o: lsapi/settings.cpp \
	litestep/buildoptions.h \
	litestep/resource.h \
	lsapi/BangCommand.h \
	lsapi/BangManager.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsapiInit.h \
	lsapi/lsmultimon.h \
	lsapi/SettingsDefines.h \
	lsapi/SettingsIterator.h \
	lsapi/SettingsManager.h \
	utility/Base.h \
	utility/common.h \
	utility/core.hpp \
	utility/CriticalSection.h \
	utility/debug.hpp \
	utility/localization.h \
	utility/safeptr.h \
	utility/safestr.h \
	utility/stringutility.h

$(OUTPUT)/SettingsFileParser.o: lsapi/SettingsFileParser.cpp \
	litestep/buildoptions.h \
	litestep/resource.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	lsapi/MathEvaluate.h \
	lsapi/SettingsDefines.h \
	lsapi/SettingsFileParser.h \
	utility/common.h \
	utility/core.hpp \
	utility/debug.hpp \
	utility/localization.h \
	utility/safeptr.h \
	utility/safestr.h \
	utility/stringutility.h

$(OUTPUT)/SettingsIterator.o: lsapi/SettingsIterator.cpp \
	litestep/buildoptions.h \
	litestep/resource.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	lsapi/SettingsDefines.h \
	lsapi/SettingsIterator.h \
	lsapi/SettingsManager.h \
	utility/common.h \
	utility/core.hpp \
	utility/CriticalSection.h \
	utility/debug.hpp \
	utility/localization.h \
	utility/safeptr.h \
	utility/safestr.h \
	utility/stringutility.h

$(OUTPUT)/SettingsManager.o: lsapi/SettingsManager.cpp \
	litestep/buildoptions.h \
	litestep/resource.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	lsapi/MathEvaluate.h \
	lsapi/SettingsDefines.h \
	lsapi/SettingsFileParser.h \
	lsapi/SettingsIterator.h \
	lsapi/SettingsManager.h \
	utility/common.h \
	utility/core.hpp \
	utility/CriticalSection.h \
	utility/debug.hpp \
	utility/localization.h \
	utility/safeptr.h \
	utility/safestr.h \
	utility/shellhlp.h \
	utility/stringutility.h

$(OUTPUT)/stubs.o: lsapi/stubs.cpp \
	litestep/buildoptions.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	utility/common.h \
	utility/debug.hpp

$(OUTPUT)/localization.o: utility/localization.cpp \
	litestep/buildoptions.h \
	lsapi/lsapidefines.h \
	utility/common.h \
	utility/debug.hpp \
	utility/localization.h \
	utility/safeptr.h \
	utility/safestr.h

$(OUTPUT)/safeptr.o: utility/safeptr.cpp \
	litestep/buildoptions.h \
	lsapi/lsapidefines.h \
	utility/common.h \
	utility/debug.hpp \
	utility/safeptr.h

$(OUTPUT)/shellhlp.o: utility/shellhlp.cpp \
	litestep/buildoptions.h \
	litestep/resource.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	utility/common.h \
	utility/core.hpp \
	utility/debug.hpp \
	utility/localization.h \
	utility/safeptr.h \
	utility/safestr.h \
	utility/shellhlp.h

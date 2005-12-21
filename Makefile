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
LDFLAGS = -mwindows

# Resource compiler
RC = windres

# Resource compiler flags
RCFLAGS = -O coff

# dllwrap
DLLWRAP = dllwrap

# strip
STRIP = strip

# rm
RM = rm -f

#-----------------------------------------------------------------------------
# Files and Paths
#-----------------------------------------------------------------------------

# Search path for dependencies
VPATH = hook;litestep;lsapi;utility

# Output directory
ifdef DEBUG
OUTPUT = Debug
else
OUTPUT = Release
endif

# Path to litestep.exe
EXE = $(OUTPUT)/litestep.exe

# Libraries that litestep.exe uses
EXELIBS = -L$(OUTPUT) -lhook -llsapi -lole32 -lshlwapi -luuid

# Object files for litestep.exe
EXEOBJS = $(OUTPUT)/BangCommand.o \
	$(OUTPUT)/DataStore.o \
	$(OUTPUT)/DDEService.o \
	$(OUTPUT)/DDEStub.o \
	$(OUTPUT)/DDEWorker.o \
	$(OUTPUT)/HookManager.o \
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
DLLOBJS = $(OUTPUT)/aboutbox.o \
	$(OUTPUT)/BangCommand.o \
	$(OUTPUT)/BangManager.o \
	$(OUTPUT)/bangs.o \
	$(OUTPUT)/graphics.o \
	$(OUTPUT)/localization.o \
	$(OUTPUT)/lsapi.o \
	$(OUTPUT)/lsapi.res \
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
	$(OUTPUT)/SettingsEvalParser.o \
	$(OUTPUT)/SettingsFileParser.o \
	$(OUTPUT)/SettingsIterator.o \
	$(OUTPUT)/SettingsManager.o \
	$(OUTPUT)/shellhlp.o \
	$(OUTPUT)/stubs.o

# Path to hook.dll
HOOKDLL = $(OUTPUT)/hook.dll

# Path to hook.dll export definitions file
HOOKDLLDEF = hook/hook_mingw.def

# Path to hook.dll import library
HOOKDLLIMPLIB = $(OUTPUT)/libhook.a

# Libraries that hook.dll uses
HOOKDLLLIBS = 

# Object files for hook.dll
HOOKDLLOBJS = $(OUTPUT)/hook.o

#-----------------------------------------------------------------------------
# Rules
#-----------------------------------------------------------------------------

# litestep.exe
$(EXE): $(EXEOBJS) $(DLL) $(HOOKDLL)
	$(CXX) -o $(EXE) $(LDFLAGS) $(EXEOBJS) $(EXELIBS)
ifndef DEBUG
	$(STRIP) $(EXE)
endif

# lsapi.dll
$(DLL): $(DLLOBJS) $(DLLDEF)
	$(DLLWRAP) --driver-name $(CXX) --def $(DLLDEF) --implib $(DLLIMPLIB) -o $(DLL) $(LDFLAGS) $(DLLOBJS) $(DLLLIBS)
ifndef DEBUG
	$(STRIP) $(DLL)
endif

# hook.dll
$(HOOKDLL): $(HOOKDLLOBJS) $(HOOKDLLDEF)
	$(DLLWRAP) --driver-name $(CXX) --def $(HOOKDLLDEF) --implib $(HOOKDLLIMPLIB) -o $(HOOKDLL) $(LDFLAGS) $(HOOKDLLOBJS) $(HOOKDLLLIBS)
ifndef DEBUG
	$(STRIP) $(HOOKDLL)
endif

# Remove output files
.PHONY: clean
clean:
	-$(RM) $(EXE)
	-$(RM) $(DLL)
	-$(RM) $(DLLIMPLIB)
	-$(RM) $(HOOKDLL)
	-$(RM) $(HOOKDLLIMPLIB)
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

$(OUTPUT)/hook.o: hook/hook.cpp \
	hook/hook.h \
	litestep/buildoptions.h \
	litestep/litestep.h \
	lsapi/lsapidefines.h \
	utility/Base.h \
	utility/common.h  \
	utility/debug.hpp \
	utility/ILiteStep.h

$(OUTPUT)/BangManager.o: litestep/BangManager.cpp \
	litestep/BangManager.h \
	litestep/buildoptions.h \
	lsapi/BangCommand.h \
	lsapi/lsapidefines.h \
	utility/Base.h \
	utility/common.h \
	utility/CriticalSection.h \
	utility/debug.hpp \
	utility/stringutility.h

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

$(OUTPUT)/HookManager.o: litestep/HookManager.cpp \
	hook/hook.h \
	litestep/buildoptions.h \
	litestep/HookManager.h \
	litestep/resource.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	utility/common.h \
	utility/debug.hpp \
	utility/macros.h

$(OUTPUT)/litestep.o: litestep/litestep.cpp \
	hook/hook.h \
	litestep/buildoptions.h \
	litestep/DataStore.h \
	litestep/DDEService.h \
	litestep/DDEStub.h \
	litestep/DDEWorker.h \
	litestep/HookManager.h \
	litestep/litestep.h \
	litestep/MessageManager.h \
	litestep/Module.h \
	litestep/ModuleManager.h \
	litestep/RecoveryMenu.h \
	litestep/resource.h \
	litestep/StartupRunner.h \
	litestep/TrayService.h \
	lsapi/BangCommand.h \
	lsapi/lsapi.h \
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

$(OUTPUT)/TrayService.o: litestep/TrayService.cpp \
	litestep/buildoptions.h \
	litestep/TrayService.h \
	litestep/resource.h \
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
	litestep/BangManager.h \
	litestep/buildoptions.h \
	litestep/litestep.h \
	litestep/resource.h \
	lsapi/BangCommand.h \
	lsapi/bangs.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	lsapi/SettingsDefines.h \
	lsapi/SettingsIterator.h \
	lsapi/SettingsManager.h \
	utility/Base.h \
	utility/common.h \
	utility/core.hpp \
	utility/CriticalSection.h \
	utility/debug.hpp \
	utility/ILiteStep.h \
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

$(OUTPUT)/MathParser.o: lsapi/MathParser.cpp \
	litestep/buildoptions.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	lsapi/MathException.h \
	lsapi/MathParser.h \
	lsapi/MathScanner.h \
	lsapi/MathToken.h \
	lsapi/MathValue.h \
	lsapi/SettingsDefines.h \
	lsapi/SettingsIterator.h \
	lsapi/SettingsManager.h \
	utility/common.h \
	utility/debug.hpp \
	utility/CriticalSection.h \
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

$(OUTPUT)/SettingsEvalParser.o: lsapi/SettingsEvalParser.cpp \
	litestep/buildoptions.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	lsapi/SettingsEvalParser.h \
	utility/common.h \
	utility/debug.hpp \

$(OUTPUT)/SettingsFileParser.o: lsapi/SettingsFileParser.cpp \
	litestep/buildoptions.h \
	litestep/resource.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	lsapi/MathEvaluate.h \
	lsapi/SettingsDefines.h \
	lsapi/SettingsEvalParser.h \
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
	lsapi/SettingsEvalParser.h \
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

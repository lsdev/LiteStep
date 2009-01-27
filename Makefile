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

#CXXWARNING = -Wall -Wextra -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -Wpacked -Wpadded -Wredundant-decls -Wunreachable-code -Winline -Wdisabled-optimization
CXXWARNING = -Wall

# C++ compiler flags
ifdef DEBUG
CXXFLAGS = $(CXXWARNING) -DLSAPI_PRIVATE -D_DEBUG -g -ggdb
else
CXXFLAGS = $(CXXWARNING) -DLSAPI_PRIVATE -DNDEBUG
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
OUTPUT = Debug_MinGW
else
OUTPUT = Release_MinGW
endif

# Path to litestep.exe
EXE = $(OUTPUT)/litestep.exe

# Libraries that litestep.exe uses
EXELIBS = -L$(OUTPUT) -llsapi -lole32 -lshlwapi -luuid

# Object files for litestep.exe
EXEOBJS = \
	litestep/$(OUTPUT)/DataStore.o \
	litestep/$(OUTPUT)/DDEService.o \
	litestep/$(OUTPUT)/DDEStub.o \
	litestep/$(OUTPUT)/DDEWorker.o \
	litestep/$(OUTPUT)/litestep.o \
	litestep/$(OUTPUT)/MessageManager.o \
	litestep/$(OUTPUT)/Module.o \
	litestep/$(OUTPUT)/ModuleManager.o \
	litestep/$(OUTPUT)/RecoveryMenu.o \
	litestep/$(OUTPUT)/StartupRunner.o \
	litestep/$(OUTPUT)/TrayNotifyIcon.o \
	litestep/$(OUTPUT)/TrayService.o \
	litestep/$(OUTPUT)/WinMain.o

EXERES = litestep/$(OUTPUT)/litestep.res

# Path to lsapi.dll
DLL = $(OUTPUT)/lsapi.dll

# Path to lsapi.dll export definitions file
DLLDEF = lsapi/lsapi_mingw.def

# Path to lsapi.dll import library
DLLIMPLIB = $(OUTPUT)/liblsapi.a

# Libraries that lsapi.dll uses
DLLLIBS = -lole32 -lshlwapi

# Object files for lsapi.dll
DLLOBJS = \
	lsapi/$(OUTPUT)/aboutbox.o \
	lsapi/$(OUTPUT)/BangCommand.o \
	lsapi/$(OUTPUT)/BangManager.o \
	lsapi/$(OUTPUT)/bangs.o \
	lsapi/$(OUTPUT)/graphics.o \
	lsapi/$(OUTPUT)/lsapi.o \
	lsapi/$(OUTPUT)/lsapiInit.o \
	lsapi/$(OUTPUT)/lsmultimon.o \
	lsapi/$(OUTPUT)/match.o \
	lsapi/$(OUTPUT)/MathEvaluate.o \
	lsapi/$(OUTPUT)/MathParser.o \
	lsapi/$(OUTPUT)/MathScanner.o \
	lsapi/$(OUTPUT)/MathToken.o \
	lsapi/$(OUTPUT)/MathValue.o \
	lsapi/$(OUTPUT)/picopng.o \
	lsapi/$(OUTPUT)/png_support.o \
	lsapi/$(OUTPUT)/settings.o \
	lsapi/$(OUTPUT)/SettingsFileParser.o \
	lsapi/$(OUTPUT)/SettingsIterator.o \
	lsapi/$(OUTPUT)/SettingsManager.o \
	lsapi/$(OUTPUT)/stubs.o

DLLRES = lsapi/$(OUTPUT)/lsapi.res

UTILOBJS = \
	utility/$(OUTPUT)/debug.o \
	utility/$(OUTPUT)/shellhlp.o

#-----------------------------------------------------------------------------
# Rules
#-----------------------------------------------------------------------------

# all targets
.PHONY: all
all: setup $(DLL) $(EXE)

# litestep.exe
$(EXE): setup $(UTILOBJS) $(EXEOBJS) $(EXERES)
	$(CXX) -o $(EXE) $(LDFLAGS) $(UTILOBJS) $(EXEOBJS) $(EXERES) $(EXELIBS)

# lsapi.dll
$(DLL): setup $(UTILOBJS) $(DLLOBJS) $(DLLRES) $(DLLDEF)
	$(DLLWRAP) --driver-name $(CXX) --def $(DLLDEF) --implib $(DLLIMPLIB) -o $(DLL) $(LDFLAGS) $(UTILOBJS) $(DLLOBJS) $(DLLRES) $(DLLLIBS)

# Setup environment
.PHONY: setup
setup:
	@-if not exist $(OUTPUT)/$(NULL) mkdir $(OUTPUT)
	@-if not exist litestep/$(OUTPUT)/$(NULL) mkdir litestep\$(OUTPUT)
	@-if not exist lsapi/$(OUTPUT)/$(NULL) mkdir lsapi\$(OUTPUT)
	@-if not exist utility/$(OUTPUT)/$(NULL) mkdir utility\$(OUTPUT)

# Remove output files
.PHONY: clean
clean:
	@echo Cleaning output files
	@echo  $(OUTPUT)/ ...
	@-$(RM) $(EXE) $(DLL) $(DLLIMPLIB)
	@echo Cleaning intermediate files
	@echo  litestep/$(OUTPUT)/ ...
	@-$(RM) $(EXE) litestep/$(OUTPUT)/*.o litestep/$(OUTPUT)/*.d litestep/$(OUTPUT)/*.res
	@echo  lsapi/$(OUTPUT)/ ...
	@-$(RM) lsapi/$(OUTPUT)/*.o lsapi/$(OUTPUT)/*.d lsapi/$(OUTPUT)/*.res
	@echo  utility/$(OUTPUT)/ ...
	@-$(RM) utility/$(OUTPUT)/*.o utility/$(OUTPUT)/*.d
	@echo Done

# Resources for litestep.exe
litestep/$(OUTPUT)/litestep.res: litestep/litestep.rc litestep/resource.h litestep/litestep.bmp litestep/litestep.ico litestep/litestep.manifest
	$(RC) -Ilitestep $(RCFLAGS) -o $@ $<

# Resources for lsapi.dll
lsapi/$(OUTPUT)/lsapi.res: lsapi/lsapi.rc lsapi/resource.h
	$(RC) -Ilsapi $(RCFLAGS) -o $@ $<

# Pattern rule to compile cpp files
utility/$(OUTPUT)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c -o $@ $<

lsapi/$(OUTPUT)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c -o $@ $<

litestep/$(OUTPUT)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c -o $@ $<

#-----------------------------------------------------------------------------
# Dependencies
#-----------------------------------------------------------------------------
-include $(EXEOBJS:.o=.d)
-include $(DLLOBJS:.o=.d)
-include $(UTILOBJS:.o=.d)

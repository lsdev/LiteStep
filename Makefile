#-----------------------------------------------------------------------------
# Makefile for MinGW
#
# To make a release build:     make
# To make a debug build:       make DEBUG=1
# To clean up a release build: make clean
# To clean up a debug build:   make clean DEBUG=1
#
# While mingw32-make.exe will work with this makefile, it is overly verbose in
# warning messages during the clean target, and required several "tweaks" to
# support.
#
# We suggest using GNU Make 3.81 available from http://gnuwin32.sourceforge.net/
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Tools and Flags
#-----------------------------------------------------------------------------

# C++ compiler
CXX = g++

# C++ compiler flags
ifdef DEBUG
CXXFLAGS = -Wall -DLS_NO_EXCEPTION -D_DEBUG -g
else
CXXFLAGS = -O2 -Wall -DNDEBUG
endif

# Linker flags
ifdef DEBUG
LDFLAGS = 
else
LDFLAGS = -s
endif

# Resource compiler
RC = windres

# Resource compiler flags
RCFLAGS = -O coff

# dlltool
DLLTOOL = dlltool

# rm
RM = del /F

# md
MD = mkdir

ifeq ($(OS), Windows_NT)
NULL = 
else
NULL = nul
endif

#-----------------------------------------------------------------------------
# Files and Paths
#-----------------------------------------------------------------------------

# Output directory
ifdef DEBUG
OUTPUT = Debug_MinGW
else
OUTPUT = Release_MinGW
endif

# Path to litestep.exe
EXE = $(OUTPUT)\litestep.exe

# Libraries that litestep.exe uses
EXELIBS = -ladvapi32 -lkernel32 -lmsvcp60 -lmsvcrt -lole32 -lshell32 -lshlwapi -luser32 -luuid -L$(OUTPUT) -llsapi.dll

EXEMAP = $(OUTPUT)\litestep.map

# Object files for litestep.exe
EXEOBJS = \
	litestep\$(OUTPUT)\DataStore.o \
	litestep\$(OUTPUT)\DDEService.o \
	litestep\$(OUTPUT)\DDEStub.o \
	litestep\$(OUTPUT)\DDEWorker.o \
	litestep\$(OUTPUT)\HookManager.o \
	litestep\$(OUTPUT)\litestep.o \
	litestep\$(OUTPUT)\MessageManager.o \
	litestep\$(OUTPUT)\Module.o \
	litestep\$(OUTPUT)\ModuleManager.o \
	litestep\$(OUTPUT)\RecoveryMenu.o \
	litestep\$(OUTPUT)\StartupRunner.o \
	litestep\$(OUTPUT)\TrayNotifyIcon.o \
	litestep\$(OUTPUT)\TrayService.o

EXERES = litestep\$(OUTPUT)\litestep.res

EXERESFILES = \
	litestep\litestep.rc \
	litestep\resource.h \
	litestep\litestep.bmp \
	litestep\litestep.ico

# Path to lsapi.dll
DLL = $(OUTPUT)\lsapi.dll

DLLEXP = $(OUTPUT)\lsapi.exp

# Path to lsapi.dll import library
DLLIMPLIB = $(OUTPUT)\liblsapi.dll.a

# Libraries that lsapi.dll uses
DLLLIBS = -ladvapi32 -lgdi32 -lkernel32 -lmsvcrt -lpng13.dll -lshell32 -lshlwapi -luser32

DLLMAP = $(OUTPUT)\lsapi.map

# Object files for lsapi.dll
DLLOBJS = \
	lsapi\$(OUTPUT)\aboutbox.o \
	lsapi\$(OUTPUT)\BangCommand.o \
	lsapi\$(OUTPUT)\BangManager.o \
	lsapi\$(OUTPUT)\bangs.o \
	lsapi\$(OUTPUT)\graphics.o \
	lsapi\$(OUTPUT)\lsapi.o \
	lsapi\$(OUTPUT)\lsapiInit.o \
	lsapi\$(OUTPUT)\lsmultimon.o \
	lsapi\$(OUTPUT)\match.o \
	lsapi\$(OUTPUT)\MathEvaluate.o \
	lsapi\$(OUTPUT)\MathParser.o \
	lsapi\$(OUTPUT)\MathScanner.o \
	lsapi\$(OUTPUT)\MathToken.o \
	lsapi\$(OUTPUT)\MathValue.o \
	lsapi\$(OUTPUT)\png_support.o \
	lsapi\$(OUTPUT)\settings.o \
	lsapi\$(OUTPUT)\SettingsFileParser.o \
	lsapi\$(OUTPUT)\SettingsIterator.o \
	lsapi\$(OUTPUT)\SettingsManager.o \
	lsapi\$(OUTPUT)\stubs.o

DLLRES = lsapi\$(OUTPUT)\lsapi.res

DLLRESFILES = \
	lsapi\lsapi.rc \
	lsapi\resource.h

# Path to hook.dll
HOOKDLL = $(OUTPUT)\hook.dll

HOOKDLLEXP = $(OUTPUT)\hook.exp

# Path to hook.dll import library
HOOKDLLIMPLIB = $(OUTPUT)\libhook.dll.a

# Libraries that hook.dll uses
HOOKDLLLIBS = -ladvapi32 -lkernel32 -luser32

HOOKDLLMAP = $(OUTPUT)\hook.map

# Object files for hook.dll
HOOKDLLOBJS = \
	hook\$(OUTPUT)\hook.o

HOOKDLLRES = hook\$(OUTPUT)\hook.res

HOOKDLLRESFILES = \
	hook\hook.rc \
	hook\resource.h

# Object files for utility project
UTILOBJS = \
	utility\$(OUTPUT)\localization.o \
	utility\$(OUTPUT)\safeptr.o \
	utility\$(OUTPUT)\shellhlp.o

#-----------------------------------------------------------------------------
# Rules
#-----------------------------------------------------------------------------

# all targets (default)
.PHONY: all
all: setup $(DLL) $(HOOKDLL) $(EXE)

# litestep.exe
$(EXE): setup $(UTILOBJS) $(EXEOBJS) $(EXERES)
	$(CXX) $(LDFLAGS) -Wl,--subsystem,windows,-Map,$(EXEMAP) -o $(EXE) $(UTILOBJS) $(EXEOBJS) $(EXERES) $(EXELIBS)

# lsapi.dll
$(DLL): setup $(UTILOBJS) $(DLLOBJS) $(DLLRES)
# The only reason that we must use dlltool, is to generate an export file and
# import library which contains the correct stdcall naming fixups.
	$(DLLTOOL) --add-stdcall-underscore -e $(DLLEXP) -l $(DLLIMPLIB) -D $(DLL) $(UTILOBJS) $(DLLOBJS) $(DLLRES)
	$(CXX) $(DLLEXP) $(LDFLAGS) -shared -Wl,--subsystem,windows,-Map,$(DLLMAP) -o $(DLL) $(UTILOBJS) $(DLLOBJS) $(DLLRES) $(DLLLIBS)

# hook.dll
$(HOOKDLL): setup $(HOOKDLLOBJS) $(HOOKDLLRES)
# We actually don't need to use dlltool on hook.dll, but it provides access
# to the .exp and .a file for comparing against MSVC generated versions.
	$(DLLTOOL) --add-stdcall-underscore -e $(HOOKDLLEXP) -l $(HOOKDLLIMPLIB) -D $(HOOKDLL) $(HOOKDLLOBJS) $(HOOKDLLRES)
	$(CXX) $(HOOKDLLEXP) $(LDFLAGS) -nodefaultlibs -nostartfiles -shared -Wl,--subsystem,console,-Map,$(HOOKDLLMAP),-entry=__DllMainCRTStartup@12 -o $(HOOKDLL) $(HOOKDLLOBJS) $(HOOKDLLRES) $(HOOKDLLLIBS)

# Setup environment
.PHONY: setup
setup:
	@-if not exist $(OUTPUT)\$(NULL) $(MD) $(OUTPUT)
	@-if not exist litestep\$(OUTPUT)\$(NULL) $(MD) litestep\$(OUTPUT)
	@-if not exist lsapi\$(OUTPUT)\$(NULL) $(MD) lsapi\$(OUTPUT)
	@-if not exist hook\$(OUTPUT)\$(NULL) $(MD) hook\$(OUTPUT)
	@-if not exist utility\$(OUTPUT)\$(NULL) $(MD) utility\$(OUTPUT)

# Remove output files
.PHONY: clean
clean:
	@echo Cleaning output files
	@echo  $(OUTPUT)\ ...
	@-$(RM) $(EXE) $(EXEMAP) $(DLL) $(DLLMAP) $(DLLEXP) $(DLLIMPLIB) $(HOOKDLL) $(HOOKDLLMAP) $(HOOKDLLEXP) $(HOOKDLLIMPLIB)
	@echo Cleaning intermediate files
	@echo  litestep\$(OUTPUT)\ ...
	@-$(RM) litestep\$(OUTPUT)\*.o litestep\$(OUTPUT)\*.d $(EXERES)
	@echo  lsapi\$(OUTPUT)\ ...
	@-$(RM) lsapi\$(OUTPUT)\*.o lsapi\$(OUTPUT)\*.d $(DLLRES)
	@echo  hook\$(OUTPUT)\ ...
	@-$(RM) hook\$(OUTPUT)\*.o hook\$(OUTPUT)\*.d $(HOOKDLLRES)
	@echo  utility\$(OUTPUT)\ ...
	@-$(RM) utility\$(OUTPUT)\*.o utility\$(OUTPUT)\*.d
	@echo Done

# Resources for litestep.exe
$(EXERES): $(EXERESFILES)
	$(RC) -Ilitestep $(RCFLAGS) -o $@ $<

# Resources for lsapi.dll
$(DLLRES): $(DLLRESFILES)
	$(RC) -Ilsapi $(RCFLAGS) -o $@ $<

# Resources for hook.dll
$(HOOKDLLRES): $(HOOKDLLRESFILES)
	$(RC) -Ihook $(RCFLAGS) -o $@ $<

# Pattern rule to compile cpp files
utility\$(OUTPUT)\\%.o: utility\%.cpp
	$(CXX) $(CXXFLAGS) -MMD -c -o $@ $<

hook\$(OUTPUT)\\%.o: hook\%.cpp
	$(CXX) $(CXXFLAGS) -MMD -c -o $@ $<

lsapi\$(OUTPUT)\\%.o: lsapi\%.cpp
	$(CXX) $(CXXFLAGS) -MMD -DLSAPI_PRIVATE -DLSAPI_INTERNAL -c -o $@ $<

litestep\$(OUTPUT)\\%.o: litestep\%.cpp
	$(CXX) $(CXXFLAGS) -MMD -DLSAPI_PRIVATE -c -o $@ $<

#-----------------------------------------------------------------------------
# Dependencies
#-----------------------------------------------------------------------------
-include $(EXEOBJS:.o=.d)
-include $(DLLOBJS:.o=.d)
-include $(HOOKDLLOBJS:.o=.d)
-include $(UTILOBJS:.o=.d)

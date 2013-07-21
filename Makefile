#-----------------------------------------------------------------------------
# Makefile for MinGW
#
# To make a release build:     make
# To make a debug build:       make DEBUG=1
# To clean up a release build: make clean
# To clean up a debug build:   make clean DEBUG=1
#
# While mingw32-make.exe will work with this makefile we suggest using
# GNU Make 3.81 available from http://gnuwin32.sourceforge.net/
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
CXXFLAGS = $(CXXWARNING) -D_DEBUG -g
else
CXXFLAGS = -O2 $(CXXWARNING) -DNDEBUG
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
	litestep\$(OUTPUT)\FullscreenMonitor.o \
	litestep\$(OUTPUT)\litestep.o \
	litestep\$(OUTPUT)\MessageManager.o \
	litestep\$(OUTPUT)\Module.o \
	litestep\$(OUTPUT)\ModuleManager.o \
	litestep\$(OUTPUT)\RecoveryMenu.o \
	litestep\$(OUTPUT)\StartupRunner.o \
	litestep\$(OUTPUT)\TrayNotifyIcon.o \
	litestep\$(OUTPUT)\TrayService.o \
	litestep\$(OUTPUT)\WinMain.o

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
DLLLIBS = -ladvapi32 -lgdi32 -lkernel32 -lmsvcrt -lole32 -lshell32 -lshlwapi -luser32

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
	lsapi\$(OUTPUT)\match.o \
	lsapi\$(OUTPUT)\MathEvaluate.o \
	lsapi\$(OUTPUT)\MathParser.o \
	lsapi\$(OUTPUT)\MathScanner.o \
	lsapi\$(OUTPUT)\MathToken.o \
	lsapi\$(OUTPUT)\MathValue.o \
	lsapi\$(OUTPUT)\picopng.o \
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

# Object files for utility project
UTILOBJS = \
	utility\$(OUTPUT)\debug.o \
	utility\$(OUTPUT)\shellhlp.o

#-----------------------------------------------------------------------------
# Rules
#-----------------------------------------------------------------------------

# all targets (default)
.PHONY: all
all: setup $(DLL) $(EXE)

# litestep.exe
$(EXE): setup $(UTILOBJS) $(EXEOBJS) $(EXERES)
	$(CXX) $(LDFLAGS) -Wl,--subsystem,windows,-Map,$(EXEMAP) -o $(EXE) $(UTILOBJS) $(EXEOBJS) $(EXERES) $(EXELIBS)

# lsapi.dll
$(DLL): setup $(UTILOBJS) $(DLLOBJS) $(DLLRES)
# The only reason that we must use dlltool, is to generate an export file and
# import library which contains the correct stdcall naming fixups.
	$(DLLTOOL) --add-stdcall-underscore -e $(DLLEXP) -l $(DLLIMPLIB) -D $(DLL) $(UTILOBJS) $(DLLOBJS) $(DLLRES)
	$(CXX) $(DLLEXP) $(LDFLAGS) -shared -Wl,--subsystem,windows,-Map,$(DLLMAP) -o $(DLL) $(UTILOBJS) $(DLLOBJS) $(DLLRES) $(DLLLIBS)

# Setup environment
.PHONY: setup
setup:
	@-if not exist $(OUTPUT)\$(NULL) $(MD) $(OUTPUT)
	@-if not exist litestep\$(OUTPUT)\$(NULL) $(MD) litestep\$(OUTPUT)
	@-if not exist lsapi\$(OUTPUT)\$(NULL) $(MD) lsapi\$(OUTPUT)
	@-if not exist utility\$(OUTPUT)\$(NULL) $(MD) utility\$(OUTPUT)

# Remove output files
.PHONY: clean
clean:
	@echo Cleaning output files
	@echo  $(OUTPUT)\ ...
	@-$(RM) $(EXE) $(EXEMAP) $(DLL) $(DLLMAP) $(DLLEXP) $(DLLIMPLIB)
	@echo Cleaning intermediate files
	@echo  litestep\$(OUTPUT)\ ...
	@-$(RM) litestep\$(OUTPUT)\*.o litestep\$(OUTPUT)\*.d $(EXERES)
	@echo  lsapi\$(OUTPUT)\ ...
	@-$(RM) lsapi\$(OUTPUT)\*.o lsapi\$(OUTPUT)\*.d $(DLLRES)
	@echo  utility\$(OUTPUT)\ ...
	@-$(RM) utility\$(OUTPUT)\*.o utility\$(OUTPUT)\*.d
	@echo Done

# Resources for litestep.exe
$(EXERES): $(EXERESFILES)
	$(RC) -Ilitestep $(RCFLAGS) -o $@ $<

# Resources for lsapi.dll
$(DLLRES): $(DLLRESFILES)
	$(RC) -Ilsapi $(RCFLAGS) -o $@ $<

# Pattern rule to compile cpp files
#
# The sed mess does this with the auto-dependency (*.d) files:
#  1. strip line of colon and everything preceding it
#  2. strip whitespace from start of line
#  3. strip trailing whitespace and backslash from end of line
#  4. remove empty lines
#  5. if a space character is left anywhere in the line, replace it with a colon
#     and a new line. (splits apart multiple file names into individual lines)
#  6. add a trailing colon at the end of the line.
#
# (note: for some reason sed does not like + matching. odd.)
#
# If for some reason you don't have sed, just delete those lines, and the only
# issue you'll run into is that if you ever get the error "No rule to make
# target <...>" then you'll have to run a 'make clean' to fix the issue.
#
utility\$(OUTPUT)\\%.o: utility\%.cpp
	$(CXX) $(CXXFLAGS) -MMD -c -o $@ $<
	@sed -e "s/^[^:]*://" -e "s/^  *//" -e "s/ *\\$$//" -e "/^$$/ d" -e "s/  */:\n/g" -e "s/$$/:/" < utility/$(OUTPUT)/$*.d >> utility/$(OUTPUT)/$*.d

lsapi\$(OUTPUT)\\%.o: lsapi\%.cpp
	$(CXX) $(CXXFLAGS) -MMD -DLSAPI_PRIVATE -DLSAPI_INTERNAL -c -o $@ $<
	@sed -e "s/^[^:]*://" -e "s/^  *//" -e "s/ *\\$$//" -e "/^$$/ d" -e "s/  */:\n/g" -e "s/$$/:/" < lsapi/$(OUTPUT)/$*.d >> lsapi/$(OUTPUT)/$*.d

litestep\$(OUTPUT)\\%.o: litestep\%.cpp
	$(CXX) $(CXXFLAGS) -MMD -DLSAPI_PRIVATE -c -o $@ $<
	@sed -e "s/^[^:]*://" -e "s/^  *//" -e "s/ *\\$$//" -e "/^$$/ d" -e "s/  */:\n/g" -e "s/$$/:/" < litestep/$(OUTPUT)/$*.d >> litestep/$(OUTPUT)/$*.d

#-----------------------------------------------------------------------------
# Dependencies
#-----------------------------------------------------------------------------
-include $(EXEOBJS:.o=.d)
-include $(DLLOBJS:.o=.d)
-include $(UTILOBJS:.o=.d)

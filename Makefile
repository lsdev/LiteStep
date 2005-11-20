CXX = g++
CXXFLAGS = -Wall -D_WIN32_IE=0x0600 -DLSAPI_PRIVATE
LDFLAGS = -mwindows
RM = rm -f

ifdef DEBUG
CXXFLAGS += -g
endif

ifdef DEBUG
STRIP = touch
else
STRIP = strip
endif

EXE = litestep.exe
EXELIBS = -L. -lhook -llsapi -lole32 -lshlwapi -luuid
EXEOBJS = litestep/DataStore.o \
	litestep/DDEService.o \
	litestep/DDEStub.o \
	litestep/DDEWorker.o \
	litestep/HookManager.o \
	litestep/litestep.o \
	litestep/MessageManager.o \
	litestep/Module.o \
	litestep/ModuleManager.o \
	litestep/RecoveryMenu.o \
	litestep/StartupRunner.o \
	litestep/TrayService.o \
	lsapi/BangCommand.o \
	utility/localization.o \
	utility/safeptr.o \
	utility/shellhlp.o

DLL = lsapi.dll
DLLDEF = lsapi/lsapi_mingw.def
DLLIMPLIB = liblsapi.a
DLLLIBS = -lole32 -lpng -lshlwapi -lz
DLLOBJS = litestep/BangManager.o \
	lsapi/aboutbox.o \
	lsapi/BangCommand.o \
	lsapi/bangs.o \
	lsapi/graphics.o \
	lsapi/lsapi.o \
	lsapi/lsmultimon.o \
	lsapi/match.o \
	lsapi/png_support.o \
	lsapi/settings.o \
	lsapi/SettingsEvalParser.o \
	lsapi/SettingsFileParser.o \
	lsapi/SettingsIterator.o \
	lsapi/SettingsManager.o \
	lsapi/stubs.o \
	utility/localization.o \
	utility/safeptr.o \
	utility/shellhlp.o

HOOKDLL = hook.dll
HOOKDLLDEF = hook/hook_mingw.def
HOOKDLLIMPLIB = libhook.a
HOOKDLLLIBS = 
HOOKDLLOBJS = hook/hook.o

$(EXE): $(EXEOBJS) $(HOOKDLL) $(DLL)
	$(CXX) -o $(EXE) $(LDFLAGS) $(EXEOBJS) $(EXELIBS)
	$(STRIP) $(EXE)

$(HOOKDLL): $(HOOKDLLOBJS) $(HOOKDLLDEF)
	dllwrap --driver-name $(CXX) -o $(HOOKDLL) $(LDFLAGS) --def $(HOOKDLLDEF) --implib $(HOOKDLLIMPLIB) $(HOOKDLLOBJS) $(HOOKDLLLIBS)
	$(STRIP) $(HOOKDLL)

$(DLL): $(DLLOBJS) $(DLLDEF)
	dllwrap --driver-name $(CXX) -o $(DLL) $(LDFLAGS) --def $(DLLDEF) --implib $(DLLIMPLIB) $(DLLOBJS) $(DLLLIBS)
	$(STRIP) $(DLL)

.PHONY: clean
clean:
	-$(RM) hook/*.o
	-$(RM) litestep/*.o
	-$(RM) lsapi/*.o
	-$(RM) utility/*.o
	-$(RM) libhook.a
	-$(RM) liblsapi.a
	-$(RM) hook.dll
	-$(RM) lsapi.dll
	-$(RM) litestep.exe

.SUFFIXES:
.SUFFIXES: .cpp .o

hook/hook.o: hook/hook.cpp \
	hook/hook.h \
	litestep/buildoptions.h \
	litestep/litestep.h \
	lsapi/lsapidefines.h \
	utility/Base.h \
	utility/common.h  \
	utility/debug.hpp \
	utility/ILiteStep.h

litestep/BangManager.o: litestep/BangManager.cpp \
	litestep/BangManager.h \
	litestep/buildoptions.h \
	lsapi/BangCommand.h \
	lsapi/lsapidefines.h \
	utility/Base.h \
	utility/common.h \
	utility/CriticalSection.h \
	utility/debug.hpp \
	utility/stringutility.h

litestep/DataStore.o: litestep/DataStore.cpp \
	litestep/DataStore.h \
	litestep/buildoptions.h \
	lsapi/lsapidefines.h \
	utility/common.h \
	utility/debug.hpp \
	utility/safeptr.h

litestep/DDEService.o: litestep/DDEService.cpp \
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

litestep/DDEStub.o: litestep/DDEStub.cpp \
	litestep/buildoptions.h \
	litestep/DDEStub.h \
	lsapi/lsapidefines.h \
	utility/Base.h \
	utility/common.h \
	utility/debug.hpp \
	utility/IService.h \
	utility/safestr.h

litestep/DDEWorker.o: litestep/DDEWorker.cpp \
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

litestep/HookManager.o: litestep/HookManager.cpp \
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

litestep/litestep.o: litestep/litestep.cpp \
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

litestep/MessageManager.o: litestep/MessageManager.cpp \
	litestep/buildoptions.h \
	litestep/MessageManager.h \
	lsapi/lsapidefines.h \
	utility/common.h \
	utility/CriticalSection.h \
	utility/debug.hpp

litestep/Module.o: litestep/Module.cpp \
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

litestep/ModuleManager.o: litestep/ModuleManager.cpp \
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

litestep/RecoveryMenu.o: litestep/RecoveryMenu.cpp \
	litestep/buildoptions.h \
	litestep/RecoveryMenu.h \
	litestep/resource.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	utility/common.h \
	utility/debug.hpp

litestep/StartupRunner.o: litestep/StartupRunner.cpp \
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

litestep/TrayService.o: litestep/TrayService.cpp \
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

lsapi/aboutbox.o: lsapi/aboutbox.cpp \
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

lsapi/BangCommand.o: lsapi/BangCommand.cpp \
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

lsapi/bangs.o: lsapi/bangs.cpp \
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

lsapi/graphics.o: lsapi/graphics.cpp \
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

lsapi/lsapi.o: lsapi/lsapi.cpp \
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

lsapi/lsmultimon.o: lsapi/lsmultimon.cpp \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h

lsapi/match.o: lsapi/match.cpp \
	litestep/buildoptions.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	utility/common.h \
	utility/debug.hpp

lsapi/png_support.o: lsapi/png_support.cpp \
	litestep/buildoptions.h \
	lsapi/lsapidefines.h \
	lsapi/png_support.h \
	utility/common.h \
	utility/debug.hpp \
	utility/safeptr.h

lsapi/settings.o: lsapi/settings.cpp \
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

lsapi/SettingsEvalParser.o: lsapi/SettingsEvalParser.cpp \
	litestep/buildoptions.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	lsapi/SettingsEvalParser.h \
	utility/common.h \
	utility/debug.hpp \

lsapi/SettingsFileParser.o: lsapi/SettingsFileParser.cpp \
	litestep/buildoptions.h \
	litestep/resource.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
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

lsapi/SettingsIterator.o: lsapi/SettingsIterator.cpp \
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

lsapi/SettingsManager.o: lsapi/SettingsManager.cpp \
	litestep/buildoptions.h \
	litestep/resource.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
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

lsapi/stubs.o: lsapi/stubs.cpp \
	litestep/buildoptions.h \
	lsapi/lsapi.h \
	lsapi/lsapidefines.h \
	lsapi/lsmultimon.h \
	utility/common.h \
	utility/debug.hpp

utility/localization.o: utility/localization.cpp \
	litestep/buildoptions.h \
	lsapi/lsapidefines.h \
	utility/common.h \
	utility/debug.hpp \
	utility/localization.h \
	utility/safeptr.h \
	utility/safestr.h

utility/safeptr.o: utility/safeptr.cpp \
	litestep/buildoptions.h \
	lsapi/lsapidefines.h \
	utility/common.h \
	utility/debug.hpp \
	utility/safeptr.h

utility/shellhlp.o: utility/shellhlp.cpp \
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

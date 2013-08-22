# To make a release build:     make
# To clean up a release build: make clean
# To make a debug build:       make DEBUG=1
# To clean up a debug build:   make DEBUG=1 clean

CXX = g++
DLLWRAP = dllwrap
TARGET = Tutorial.dll
DEF = exports.def
LIBS = -llsapi

ifdef DEBUG
CXXFLAGS = -I"..\..\..\include" -Wall -D_DEBUG -g
LDFLAGS = -L"..\..\..\lib" -mwindows
OUTPUT_DIR = Debug
OBJ_DIR = Debug
else
CXXFLAGS = -I"..\..\..\include" -Wall -DNDEBUG -O2
LDFLAGS = -L"..\..\..\lib" -mwindows -s
OUTPUT_DIR = Release
OBJ_DIR = Release
endif

OBJS = $(OBJ_DIR)\Label.o \
	$(OBJ_DIR)\main.o \
	$(OBJ_DIR)\trace.o \
	$(OBJ_DIR)\utility.o

ifeq ($(OS), Windows_NT)
NULL = 
else
NULL = nul
endif

.PHONY: all directories clean

all: directories $(OUTPUT_DIR)\$(TARGET)

directories:
	-@if not exist $(OUTPUT_DIR)\$(NULL) mkdir $(OUTPUT_DIR)
	-@if not exist $(OBJ_DIR)\$(NULL) mkdir $(OBJ_DIR)

clean:
	@echo Deleting intermediate files
	-@del $(OBJ_DIR)\*.o
	@echo Deleting $(TARGET)
	-@del $(OUTPUT_DIR)\$(TARGET)

$(OUTPUT_DIR)\$(TARGET): $(DEF) $(OBJS)
	@echo Linking $(TARGET)
	@$(DLLWRAP) --driver-name $(CXX) $(LDFLAGS) --def $(DEF) $(OBJS) $(LIBS) -o $(OUTPUT_DIR)\$(TARGET)

$(OBJ_DIR)\Label.o: Label.cpp common.h Label.h msgcrack.h trace.h utility.h
	@echo Compiling $<
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)\main.o: main.cpp common.h Label.h trace.h
	@echo Compiling $<
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)\trace.o: trace.cpp trace.h
	@echo Compiling $<
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)\utility.o: utility.cpp common.h trace.h utility.h
	@echo Compiling $<
	@$(CXX) $(CXXFLAGS) -c $< -o $@

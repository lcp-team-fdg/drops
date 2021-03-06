# ------------------------------------------------
# Generic Makefile
#
# Author: Nigel Armstrong
# Date  : 2015-11-02
#
# Changelog :
#   2010-11-05 - first version
#   2011-08-10 - added structure : sources, objects, binaries
#                thanks to http://stackoverflow.com/users/128940/beta
#   2015-03-30 - CNU UAS Lab version
#   2015-11-02 - Project Pulusan version
# ------------------------------------------------

OS := $(shell uname)

# project name (generate executable with this name)
TARGET   = drops

CC       = g++-4.8
# compiling flags here
CXXFLAGS   += -Wall -I. -std=c++11

ifeq ($(OS), Darwin)
CC = g++
CXXFLAGS += -I /usr/local/Cellar/openssl/1.0.2e_1/include
endif

# libs
LIBS = sbpl boost_system cpprest ssl crypto

ifeq ($(OS), Darwin)
LIBS += boost_chrono boost_thread-mt
endif

# linking flags here
LFLAGS   = -Wall -I. -lm -L /usr/local/lib
LDLIBS  := $(addprefix -l,$(LIBS))

ifeq ($(OS), Darwin)
LFLAGS += -L /usr/local/Cellar/openssl/1.0.2e_1/lib
endif

# change these to set the proper directories where each files shoould be
SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.hpp)
INCLUDE_DIR := /usr/local/include/sbpl
CFLAGS   += $(foreach includedir,$(INCLUDE_DIR),-I$(includedir))
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
rm       = rm -f

.PHONEY: echo_start clean remove check-syntax astyle

all: astyle directories echo_start $(BINDIR)/$(TARGET)

debug: CXXFLAGS += -D_DEBUG -g
debug: CCFLAGS += -D_DEBUG -g
debug: astyle directories echo_start $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(LINK.cc) $^ -o $@ $(LFLAGS) $(LDLIBS)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp | $(OBJDIR)
	@$(CC) $(CXXFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

directories:
	@mkdir -p $(BINDIR)
	@mkdir -p $(OBJDIR)

echo_start:
	@echo "Compiling project..."

clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"

check-syntax:
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o nul -S ${CHK_SOURCES}

astyle:
	@echo "Styling style..."
	astyle --style=stroustrup --indent=spaces=4 -q -p -n -j --recursive "src/*.cpp" "src/*.hpp"

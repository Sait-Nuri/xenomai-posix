program_NAME := run
#program_C_SRCS := $(wildcard *.c)
program_CXX_SRCS := $(wildcard *.cpp)
#program_C_OBJS := ${program_C_SRCS:.c=.o}
program_CXX_OBJS := ${program_CXX_SRCS:.cpp=.o}
program_OBJS := $(program_CXX_OBJS) #$(program_C_OBJS) 
#program_INCLUDE_DIRS :=
#program_LIBRARY_DIRS :=
#program_LIBRARIES :=

#CPPFLAGS += $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir))
#LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))
#LDFLAGS += $(foreach library,$(program_LIBRARIES),-l$(library))

####### Compiler, tools and options
XENO_DESTDIR:=
XENO_CONFIG:=/usr/xenomai/bin/xeno-config

#--- POSIX ---
#XENO_POSIX_CFLAGS:=$(shell DESTDIR=$(XENO_DESTDIR) $(XENO_CONFIG) --skin=posix --cflags)
#XENO_POSIX_LIBS:=$(shell DESTDIR=$(XENO_DESTDIR) $(XENO_CONFIG) --skin=posix --ldflags)

#--- NATIVE ---
XENO_NATIVE_CFLAGS:=$(shell DESTDIR=$(XENO_DESTDIR) $(XENO_CONFIG) --skin=native --cflags)
XENO_NATIVE_LIBS:=$(shell DESTDIR=$(XENO_DESTDIR) $(XENO_CONFIG) --skin=native --ldflags)

CPPFLAGS = $(XENO_POSIX_CFLAGS) $(XENO_NATIVE_CFLAGS)
CFLAGS   = $(XENO_POSIX_CFLAGS) $(XENO_NATIVE_CFLAGS)
LDFLAGS  = $(XENO_POSIX_LIBS) $(XENO_NATIVE_LIBS)
CC       = gcc
CXX      = g++


.PHONY: all clean distclean

all: $(program_NAME)

$(program_NAME): $(program_OBJS)
	$(CXX) $(CPPFLAGS) $(program_OBJS) $(LDFLAGS) -o  $(program_NAME)

clean:
	@- $(RM) $(program_NAME)
	@- $(RM) $(program_OBJS)

distclean: clean

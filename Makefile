
# Gabi's Makefile v3.0

# ---------------- Configuration -----------------------------------------------

## Name of the project. This will be the name of the executable placed in the
## executable directory.
PROJECT_NAME = lisilisk
## Root source directory. Contains the c implementation files.
SRC_DIR = src
## Root include directory. Contains the c header files. Passed with -I to
## the compiler.
INC_DIR = inc unstandard/inc
## Build diectory. Will contain object and binary files linked in the final
## executable
OBJ_DIR = build
## Executable directory. Contains the final binary file.
EXC_DIR = bin
## resources directory
RES_DIR = res

## compiler
CC = gcc
## resource packer
RESPACKER = ld

## compilation flags
CFLAGS += -Wall -Wextra -Wpedantic -Werror
CFLAGS += -Wno-error=unused-function  -Wno-error=unused-variable -Wno-error=unused-parameter
CFLAGS += -g -std=c2x

## linker flags
LFLAGS += -Lunstandard/bin -lunstandard
LFLAGS += -lGL `sdl2-config --libs`
LFLAGS += -lSDL2_image
LFLAGS += -lm

## archiver flags to build the project library
ARFLAGS = rvcs

## additional flags for defines
DFLAGS +=

## resource packing flags
RESFLAGS = -r -b binary -z noexecstack

include rules.mk

# specific to this project : pack the libraries together for easier distribution
lib::
	make -C unstandard
	mv $(LIBRARY_ARCHIVE) $(LIBRARY_ARCHIVE).standalone
	ar cqT $(LIBRARY_ARCHIVE) $(LIBRARY_ARCHIVE).standalone unstandard/bin/libunstandard.a && echo -e 'create $(LIBRARY_ARCHIVE)\naddlib $(LIBRARY_ARCHIVE)\nsave\nend' | ar -M
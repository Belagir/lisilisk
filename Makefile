
# Gabi's Makefile v3.0

# ---------------- Configuration -----------------------------------------------

## Name of the project. This will be the name of the executable placed in the
## executable directory.
PROJECT_NAME = lisilisk
## Root source directory. Contains the c implementation files.
SRC_DIR = src
## Project dependencies. Expected to have a makefile producing a library named from the folder name.
SUBPROJECTS = subprojects/3dful subprojects/resourceful
## Root include directory. Contains the c header files. Passed with -I to
## the compiler.
INC_DIR = inc unstandard/inc subprojects/resourceful/inc subprojects/3dful/inc
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
CFLAGS += -Wno-error=unused-function  -Wno-error=unused-variable
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

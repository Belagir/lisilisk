
## path to the project library
LIBRARY_ARCHIVE = $(EXC_DIR)/lib$(PROJECT_NAME).a

## path to executable name
TARGET = $(EXC_DIR)/$(PROJECT_NAME)
##
TARGET_DEP_LOC = $(addprefix -L, $(addsuffix /bin, $(SUBPROJECTS)))
TARGET_DEP_LIB = $(addprefix -l, $(notdir $(SUBPROJECTS)))

## list of all c files without their path
SRC := $(notdir $(shell find $(SRC_DIR) -name *.c))
## list of all duplicate c files to enforce uniqueness of filenames
DUPL_SRC := $(strip $(shell echo $(SRC) | tr ' ' '\n' | sort | uniq -d))
## list of all target object files with their path
OBJ := $(addprefix $(OBJ_DIR)/, $(patsubst %.c, %.o, $(SRC)))

## where to find c files : all unique directories in SRC_DIR which contain a c
## file
vpath %.c $(sort $(dir $(shell find $(SRC_DIR) -name *.c)))

ifdef RES_DIR
## list of all resources files without their directory
RES := $(notdir $(shell find $(RES_DIR)/ -type f))
## list of all target binaries resource files to include in the binary
RES_BIN := $(addprefix $(OBJ_DIR)/, $(addsuffix .resbin, $(RES)))

## where to find resource files
vpath % $(sort $(dir $(shell find $(RES_DIR)/ -type f)))
endif

## makefile-managed directories
BUILD_DIRS = $(EXC_DIR) $(OBJ_DIR)

## additional compilation option for includes
ARGS_INCL = $(addprefix -I, $(INC_DIR))

# --------------- Rules --------------------------------------------------------

.PHONY: all check clean count_lines

# -------- compilation -----------------

all: check $(SUBPROJECTS) $(BUILD_DIRS) $(TARGET) | count_lines

lib: check $(BUILD_DIRS) $(LIBRARY_ARCHIVE)

$(LIBRARY_ARCHIVE): $(OBJ) $(RES_BIN)
	$(AR) $(ARFLAGS) $@ $(OBJ) $(RES_BIN)

$(TARGET): $(OBJ) $(RES_BIN)
	$(CC) $^ -o $@  $(TARGET_DEP_LOC) $(TARGET_DEP_LIB) $(LFLAGS)

$(OBJ_DIR)/%.o: %.c
	$(CC) -c $? -o $@ $(ARGS_INCL) $(CFLAGS) $(DFLAGS)

$(OBJ_DIR)/%.resbin: %
	$(RESPACKER) $(RESFLAGS) $? -o $@

# -------- dir management --------------

$(BUILD_DIRS):
	mkdir -p $@

clean:
	rm -Rf $(BUILD_DIRS)

# -------- internal dependencies -------

$(SUBPROJECTS): FORCE
	make -C $@ lib

FORCE:

# -------- sanity -----------------------

check:
ifdef DUPL_SRC
	$(error duplicate filenames: $(DUPL_SRC))
endif

count_lines:
ifneq (, $(shell which cloc))
	@cloc --hide-rate --quiet $(SRC_DIR) $(INC_DIR)
endif

# ----  ----  ----  ----  ----  ----

 # Senteurs d'été doux
 # Claquement brefs des touches
 # Segmentation fault

EXE = exec

SRC_DIR = src
OBJ_DIR = obj


# For better performance:
PROCESSOR_ARCH = -march=native
OPENMP = -fopenmp


# Choice of graphic library:

# Remove this line if SDL2 isn't installed or to be used:
GRAPHIC_LIB = SDL2

ifeq ($(GRAPHIC_LIB), SDL2)

	GRAPHIC_FLAGS = `sdl2-config --cflags`
	GRAPHIC_LINKS = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf
endif


CC = gcc
CPPFLAGS =
CFLAGS = -std=c99 -Wall -O2 $(PROCESSOR_ARCH) $(GRAPHIC_FLAGS) $(OPENMP)
LDFLAGS = # -static # <- library included, bigger executables.
LDLIBS = $(GRAPHIC_LINKS) -lm $(OPENMP)


SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)


# The following names are not associated with files:
.PHONY: all clean

# All executables to be created:
all: $(EXE)

# Linking the program:
$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Compiling other source files:
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Cleaning with 'make clean' the object files:
clean:
	rm -fv $(EXE) $(OBJ_DIR)/*

# Files to compile as part of the project
OBJS = main.c chip8.c

# Compiler
CC = gcc

# Compiler flags
COMPILER_FLAGS = -w -ggdb

# Linker flags
LINKER_FLAGS = -lSDL2

# Executable output name
TARGET = chip8er

# Target that compiles the executable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(TARGET)

# Files to compile as part of the project
OBJS = main.c mem.c cpu.c

# Compiler
CC = gcc

# Compiler flags
COMPILER_FLAGS = -w

# Linker flags
LINKER_FLAGS = -lSDL2

# Executable output name
TARGET = chip8er

# Target that compiles the executable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(TARGET)
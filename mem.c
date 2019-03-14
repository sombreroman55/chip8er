#include "mem.h"

uint8_t memory[MEM_SIZE] = {0};

void mem_write(uint8_t addr, uint8_t byte)
{
    memory[addr] = byte;
}

uint8_t mem_read(uint8_t addr)
{
    return memory[addr];
}

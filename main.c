/* main.c
 * Main entry point for program
 */

#include <SDL2/SDL.h>
#include "chip8.h"

int main (int argc, char** argv)
{
    Chip8 cpu;
    cpu_init(&cpu);
    cpu_execute(&cpu);
    printf("PC: 0x%04X\n", cpu.PC);
    return 0;
}

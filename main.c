/* main.c
 * Main entry point for program
 */

#include <SDL2/SDL.h>
#include "mem.h"
#include "cpu.h"

int main (int argc, char** argv)
{
    CPU cpu;
    cpu_init(&cpu);
    printf("0x%04X\n", cpu.PC);
    printf("0x%04X\n", cpu.SP);
    printf("0x%04X\n", cpu.I);
    printf("0x%02X\n", cpu.DT);
    printf("0x%02X\n", cpu.ST);
    return 0;
}

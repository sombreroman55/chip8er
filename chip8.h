/* chip8.h
 * Chip8 "CPU" interface
 */

#ifndef _CHIP8_H_
#define _CHIP8_H_

#include <stdint.h>
#include <stdbool.h>

#define NUM_REGS    0x10
#define NUM_KEYS    0x10
#define STACK_SIZE  0x10
#define FONT_OFFSET 0x50
#define MEM_SIZE    0x1000
#define PROG_START  0x200
#define ETI_START   0x600

#define SCREEN_W    64
#define SCREEN_H    32
#define VIEW_RATIO  8
#define VIDRAM_SIZE (SCREEN_W*SCREEN_H/VIEW_RATIO)

typedef union
{
  uint8_t memory[MEM_SIZE];
  struct
  {
    uint8_t  V[NUM_REGS];       /* 16 general purpose 8-bit registers */
    uint16_t I;                 /* Memory storage register */
    uint16_t PC;                /* Program counter */
    uint16_t SP;                /* Stack pointer */
    uint16_t stack[STACK_SIZE];
    uint8_t  DT;                /* Delay timer register */
    uint8_t  ST;                /* Sound timer register */
    uint8_t  keys[NUM_KEYS];
    uint8_t  vidram[SCREEN_W*SCREEN_H/VIEW_RATIO];
  };
} Chip8;

Chip8* chip8_init     (void);
int    chip8_load_file(Chip8* cpu, const char* rom);
void   chip8_execute  (Chip8* cpu);

#endif /* _CHIP8_H_ */

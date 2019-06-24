/* cpu.h
 * Chip8 "CPU" interface
 */

#ifndef _CHIP8_H_
#define _CHIP8_H_

#include <stdint.h>

#define SCREEN_W    64
#define SCREEN_H    32
#define VIDRAM_SIZE 0x800
#define FONT_OFFSET 0x50
#define MEM_SIZE    0x1000
#define MEM_OFFSET  0x200

#define PRE_MASK  0xF000
#define NNN_MASK  0x0FFF
#define N_MASK    0x000F
#define X_MASK    0x0F00
#define Y_MASK    0x00F0
#define KK_MASK   0x00FF

typedef struct
{
  uint8_t memory[MEM_SIZE];
  uint8_t  V[0x10];  /* 16 general purpose 8-bit registers */
  uint16_t I;       /* Memory storage register */
  uint16_t PC;      /* Program counter */
  uint16_t SP;      /* Stack pointer */
  uint16_t stack[16];
  uint8_t  DT;       /* Delay timer register */
  uint8_t  ST;       /* Sound timer register */
  uint8_t  keys[0x10];
  uint8_t  update_screen;
  uint8_t  vidram[VIDRAM_SIZE];
} Chip8;

int  chip8_init     (Chip8* const cpu);
int  chip8_load_file(Chip8* const cpu, const char* rom);
void chip8_execute  (Chip8* const cpu);

#endif /* _CHIP8_H_ */

/* cpu.h
 * Chip8 "CPU" interface
 */

#ifndef _CHIP8_H_
#define _CHIP8_H_

#include <stdint.h>

#define W 64
#define H 32
#define VIDRAM_SIZE W*H
#define MEM_SIZE 0x1000

#define PRE_MASK  0xF000
#define NNN_MASK  0x0FFF
#define N_MASK    0x000F
#define X_MASK    0x0F00
#define Y_MASK    0x00F0
#define KK_MASK   0x00FF

typedef struct
{
  union
  {
    uint8_t memory[MEM_SIZE];
    struct
    {
      uint8_t finst[2];
      uint8_t V[0x10];  /* 16 general purpose 8-bit registers */
      uint16_t PC;      /* Program counter */
      uint16_t SP;      /* Stack pointer */
      uint16_t stack[12];
      uint16_t I;       /* Memory storage register */
      uint8_t DT;       /* Delay timer register */
      uint8_t ST;       /* Sound timer register */
      uint8_t font[0xF][5];
      uint8_t keys[16];
      uint8_t vidram[VIDRAM_SIZE];
      uint8_t waiting;
    };
  };
} Chip8;

void chip8_execute(Chip8* cpu);
void chip8_init(Chip8* cpu);

#endif /* _CHIP8_H_ */

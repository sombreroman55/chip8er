/* cpu.h
 * Chip8 "CPU" interface
 */

#ifndef _CPU_H_
#define _CPU_H_

#include <stdint.h>

#define NNN_MASK  0x0FFF
#define N_MASK    0x000F
#define X_MASK    0x0F00
#define Y_MASK    0x00F0
#define KK_MASK   0x00FF

typedef struct
{
    uint8_t V[0x10];  /* 16 general purpose 8-bit registers */
    uint16_t PC;      /* Program counter */
    uint16_t SP;      /* Stack pointer */
    uint16_t I;       /* Memory storage register */
    uint8_t DT;       /* Delay timer register */
    uint8_t ST;       /* Sound timer register */
} CPU;

void cpu_execute(CPU* cpu);
void cpu_init(CPU* cpu);

#endif /* _CPU_H_ */

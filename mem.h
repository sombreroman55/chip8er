/* mem.h
 * Chip8 memory interface
 */

#ifndef _MEM_H_
#define _MEM_H_

#include <stdint.h>

#define MEM_SIZE      0x1000
#define STACK_START   0x52
#define PROGRAM_START 0x200

extern uint8_t memory[MEM_SIZE];

void mem_write(uint8_t addr, uint8_t byte);
uint8_t mem_read(uint8_t addr);

#endif /* _MEM_H_ */

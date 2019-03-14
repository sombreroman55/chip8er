/* cpu.c
 * Chip8 "CPU" implementation
 */

#include <string.h>
#include "cpu.h"
#include "mem.h"

void cpu_init(CPU* cpu)
{
    memset(cpu->V, 0, sizeof(uint8_t) * 0x10);
    cpu->PC = 0x200;
    cpu->SP = 0x52;
    cpu->I = 0;
    cpu->DT = 0;
    cpu->ST = 0;
}

void cpu_execute(CPU* cpu)
{
    while (1)
    {
        uint16_t instruction  = mem_read(cpu->PC);
        uint16_t inst_prefix  = instruction & 0xF000;
        uint16_t inst_nnn_val = instruction & NNN_MASK;
        uint16_t inst_n_val   = instruction & N_MASK;
        uint16_t inst_x_val   = instruction & X_MASK;
        uint16_t inst_y_val   = instruction & Y_MASK;
        uint16_t inst_kk_val  = instruction & KK_MASK;
        switch(inst_prefix)
        {
        case 0x0:
            {
                if (instruction == 0x00E0) /* CLS */
                {
                    ; /* TBD - Clear the display */
                }
                else if (instruction == 0x00EE) /* RET */
                {
                    cpu->PC = mem_read(cpu->SP);
                    cpu->SP -= 2;
                }
                else /* SYS addr */
                {
                    ; /* Machine code routine at addr. Unused */
                }
                break;
            }
        case 0x1: /* JP addr */
            {
                cpu->PC = inst_nnn_val;
                break;
            }
        case 0x2: /* CALL addr */
            {
                cpu->SP += 2;
                mem_write(cpu->SP,    cpu->PC);
                mem_write(cpu->SP+1, (cpu->PC >> 8));
                cpu->PC = inst_nnn_val;
                break;
            }
        case 0x3: /* SE Vx, byte */
            {
                if (cpu->V[(inst_x_val >> 8)] == inst_kk_val)
                    cpu->PC += 2;
                break;
            }
        case 0x4: /* SNE Vx, byte */
            {
                if (cpu->V[(inst_x_val >> 8)] != inst_kk_val)
                    cpu->PC += 2;
                break;
            }
        case 0x5: /* SE Vx, Vy */
            {
                if (cpu->V[(inst_x_val >> 8)] == cpu->V[(inst_y_val >> 4)])
                    cpu->PC += 2;
                break;
            }
        case 0x6: /* LD Vx, byte */
            {
                cpu->V[(inst_x_val >> 8)] == inst_kk_val;
                break;
            }
        case 0x7: /* ADD Vx, byte */
            {
                cpu->V[(inst_x_val >> 8)] += inst_kk_val;
                break;
            }
        case 0x8: /* SE Vx, byte */
            {
                break;
            }
        case 0x9: /* SNE Vx, Vy */
            {
                if (cpu->V[(inst_x_val >> 8)] != cpu->V[(inst_y_val >> 4)])
                    cpu->PC += 2;
                break;
            }
        case 0xA: /* LD I, addr */
            {
                cpu->I = inst_nnn_val;
                break;
            }
        case 0xB: /* JP V0, addr */
            {
                cpu->PC = cpu->V[0] + inst_nnn_val;
                break;
            }
        case 0xC: /* RND Vx, byte */
            {
                break;
            }
        case 0xD: /* DRW Vx, Vy, nibble */
            {
                break;
            }
        case 0xE:
            {
                switch(inst_kk_val)
                {
                case 0x9E: /* SKP Vx */
                    break;
                case 0xA1: /* SKNP Vx */
                    break;
                }
                break;
            }
        case 0xF:
            {
                switch (inst_kk_val)
                {
                case 0x07: /* LD Vx, DT */
                    cpu->V[(inst_x_val >> 8)] = cpu->DT;
                    break;
                case 0x0A: /* LD Vx, K */
                    /* TBD */
                    break;
                case 0x15: /* LD DT, Vx */
                    cpu->DT = cpu->V[(inst_x_val >> 8)];
                    break;
                case 0x18: /* LD ST, Vx */
                    cpu->ST = cpu->V[(inst_x_val >> 8)];
                    break;
                case 0x1E: /* ADD I, Vx */
                    /* TBD */
                    break;
                case 0x29: /* LD F, Vx */
                    /* TBD */
                    break;
                case 0x33: /* LD B, Vx */
                    /* TBD */
                    break;
                case 0x55: /* LD [I], Vx */
                    /* TBD */
                    break;
                case 0x65: /* LD Vx, [I] */
                    /* TBD */
                    break;
                default:
                    printf("Unrecognized opcode!");
                    break;
                }
                break;
            }
        default:
            printf("Unrecognized opcode!");
            break;
        }
    }
}

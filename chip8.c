/* cpu.c
 * Chip8 "CPU" implementation
 */

#include <SDL2/SDL.h>
#include <string.h>
#include <time.h>   /* For RNG seed */
#include <stdlib.h> /* For RNG */
#include <stdio.h>
#include "chip8.h"

void cpu_init(Chip8* cpu)
{
    memset(cpu->vidram, 0, sizeof(uint8_t) * VIDRAM_SIZE);
    cpu->finst[0] = 0x12;
    cpu->finst[1] = 0x00;
    cpu->PC = 0x00;
    cpu->SP = 0x00;
    cpu->I = 0x00;
    int i;
    for (i = 0; i < 0x10; i++)
    {
        cpu->V[i] = 0x00;
    }

    /* Install the font */
    /* TODO: Make this more efficient */
    /* 0 */
    cpu->font[0x0][0] = 0xF0;
    cpu->font[0x0][1] = 0x90;
    cpu->font[0x0][2] = 0x90;
    cpu->font[0x0][3] = 0x90;
    cpu->font[0x0][4] = 0xF0;
    /* 1 */
    cpu->font[0x1][0] = 0x20;
    cpu->font[0x1][1] = 0x60;
    cpu->font[0x1][2] = 0x20;
    cpu->font[0x1][3] = 0x20;
    cpu->font[0x1][4] = 0x70;
    /* 2 */
    cpu->font[0x2][0] = 0xF0;
    cpu->font[0x2][1] = 0x10;
    cpu->font[0x2][2] = 0xF0;
    cpu->font[0x2][3] = 0x80;
    cpu->font[0x2][4] = 0xF0;
    /* 3 */
    cpu->font[0x3][0] = 0xF0;
    cpu->font[0x3][1] = 0x10;
    cpu->font[0x3][2] = 0xF0;
    cpu->font[0x3][3] = 0x10;
    cpu->font[0x3][4] = 0xF0;
    /* 4 */
    cpu->font[0x4][0] = 0x90;
    cpu->font[0x4][1] = 0x90;
    cpu->font[0x4][2] = 0xF0;
    cpu->font[0x4][3] = 0x10;
    cpu->font[0x4][4] = 0x10;
    /* 5 */
    cpu->font[0x5][0] = 0xF0;
    cpu->font[0x5][1] = 0x80;
    cpu->font[0x5][2] = 0xF0;
    cpu->font[0x5][3] = 0x10;
    cpu->font[0x5][4] = 0xF0;
    /* 6 */
    cpu->font[0x6][0] = 0xF0;
    cpu->font[0x6][1] = 0x80;
    cpu->font[0x6][2] = 0xF0;
    cpu->font[0x6][3] = 0x90;
    cpu->font[0x6][4] = 0xF0;
    /* 7 */
    cpu->font[0x7][0] = 0xF0;
    cpu->font[0x7][1] = 0x10;
    cpu->font[0x7][2] = 0x20;
    cpu->font[0x7][3] = 0x40;
    cpu->font[0x7][4] = 0x40;
    /* 8 */
    cpu->font[0x8][0] = 0xF0;
    cpu->font[0x8][1] = 0x90;
    cpu->font[0x8][2] = 0xF0;
    cpu->font[0x8][3] = 0x90;
    cpu->font[0x8][4] = 0xF0;
    /* 9 */
    cpu->font[0x9][0] = 0xF0;
    cpu->font[0x9][1] = 0x90;
    cpu->font[0x9][2] = 0xF0;
    cpu->font[0x9][3] = 0x10;
    cpu->font[0x9][4] = 0xF0;
    /* A */
    cpu->font[0xA][0] = 0xF0;
    cpu->font[0xA][1] = 0x90;
    cpu->font[0xA][2] = 0xF0;
    cpu->font[0xA][3] = 0x90;
    cpu->font[0xA][4] = 0x90;
    /* B */
    cpu->font[0xB][0] = 0xE0;
    cpu->font[0xB][1] = 0x90;
    cpu->font[0xB][2] = 0xE0;
    cpu->font[0xB][3] = 0x90;
    cpu->font[0xB][4] = 0xE0;
    /* C */
    cpu->font[0xC][0] = 0xF0;
    cpu->font[0xC][1] = 0x80;
    cpu->font[0xC][2] = 0x80;
    cpu->font[0xC][3] = 0x80;
    cpu->font[0xC][4] = 0xF0;
    /* D */
    cpu->font[0xD][0] = 0xE0;
    cpu->font[0xD][1] = 0x90;
    cpu->font[0xD][2] = 0x90;
    cpu->font[0xD][3] = 0x90;
    cpu->font[0xD][4] = 0xE0;
    /* E */
    cpu->font[0xE][0] = 0xF0;
    cpu->font[0xE][1] = 0x80;
    cpu->font[0xE][2] = 0xF0;
    cpu->font[0xE][3] = 0x80;
    cpu->font[0xE][4] = 0xF0;
    /* F */
    cpu->font[0xF][0] = 0xF0;
    cpu->font[0xF][1] = 0x80;
    cpu->font[0xF][2] = 0xF0;
    cpu->font[0xF][3] = 0x80;
    cpu->font[0xF][4] = 0x80;
}

void cpu_execute(Chip8* cpu)
{
    srand(time(NULL)); /* Initialize RNG */
    int x = 1;
    while (x--)
    {
        uint16_t instruction  =  (cpu->memory[cpu->PC] << 8) 
            | cpu->memory[cpu->PC+1];
        uint16_t inst_prefix  = (instruction & PRE_MASK) >> 12;
        uint16_t inst_nnn_val = (instruction & NNN_MASK);
        uint16_t inst_n_val   = (instruction & N_MASK);
        uint16_t inst_x_val   = (instruction & X_MASK) >> 8;
        uint16_t inst_y_val   = (instruction & Y_MASK) >> 4;
        uint16_t inst_kk_val  = (instruction & KK_MASK);
        printf("Instruction: 0x%04X\n", instruction);
        cpu->PC += 2; /* Increment program counter */
        uint16_t p;
        switch(inst_prefix)
        {
            case 0x0:
                switch(inst_kk_val)
                {
                    case 0xE0: /* CLS */
                        memset(cpu->vidram, 0, sizeof(uint8_t) * VIDRAM_SIZE);
                        break;

                    case 0xEE: /* RET */
                        cpu->PC = cpu->stack[cpu->SP--];
                        break;
                }
                break;

            case 0x1: /* JP addr */
                cpu->PC = inst_nnn_val;
                break;

            case 0x2: /* CALL addr */
                cpu->stack[++cpu->SP] = cpu->PC;
                cpu->PC = inst_nnn_val;
                break;

            case 0x3: /* SE Vx, byte */
                if (cpu->V[inst_x_val] == inst_kk_val)
                    cpu->PC += 2;
                break;

            case 0x4: /* SNE Vx, byte */
                if (cpu->V[inst_x_val] != inst_kk_val)
                    cpu->PC += 2;
                break;

            case 0x5: /* SE Vx, Vy */
                if (cpu->V[inst_x_val] == cpu->V[inst_y_val])
                    cpu->PC += 2;
                break;

            case 0x6: /* LD Vx, byte */
                cpu->V[inst_x_val] == inst_kk_val;
                break;

            case 0x7: /* ADD Vx, byte */
                cpu->V[inst_x_val] += inst_kk_val;
                break;

            case 0x8:
                switch(inst_n_val)
                {
                    case 0x0: /* LD Vx, Vy */
                        cpu->V[inst_x_val] = cpu->V[inst_y_val];
                        break;

                    case 0x1: /* OR Vx, Vy */
                        cpu->V[inst_x_val] |= cpu->V[inst_y_val];
                        break;

                    case 0x2: /* AND Vx, Vy */
                        cpu->V[inst_x_val] &= cpu->V[inst_y_val];
                        break;

                    case 0x3: /* XOR Vx, Vy */
                        cpu->V[inst_x_val] ^= cpu->V[inst_y_val];
                        break;

                    case 0x4: /* ADD Vx, Vy */
                        {
                            int p = cpu->V[inst_x_val] + cpu->V[inst_y_val];
                            cpu->V[0xF] = (p >> 8);
                            cpu->V[inst_x_val] = p & 0xFF;
                        }
                        break;

                    case 0x5: /* SUB Vx, Vy */
                        cpu->V[inst_x_val] -= cpu->V[inst_y_val];
                        {
                            int p = cpu->V[inst_x_val] - cpu->V[inst_y_val];
                            cpu->V[0xF] = !(p >> 8);
                            cpu->V[inst_x_val] = p & 0xFF;
                        }
                        break;

                    case 0x6: /* SHR Vx, Vy */
                        cpu->V[0xF] = cpu->V[inst_x_val] & 0x1;
                        cpu->V[inst_x_val] >> 1;
                        break;

                    case 0x7: /* SUBN Vx, Vy */
                        {
                            int p = cpu->V[inst_y_val] - cpu->V[inst_x_val];
                            cpu->V[0xF] = !(p >> 8);
                            cpu->V[inst_x_val] = p & 0xFF;
                        }               
                        break;

                    case 0xE: /* SHL Vx, Vy */
                        cpu->V[0xF] = cpu->V[inst_x_val] & 0x80;
                        cpu->V[inst_x_val] << 1;
                        break;
                }
                break;
            case 0x9: /* SNE Vx, Vy */
                if (cpu->V[inst_x_val] != cpu->V[inst_y_val])
                    cpu->PC += 2;
                break;

            case 0xA: /* LD I, addr */
                cpu->I = inst_nnn_val;
                break;

            case 0xB: /* JP V0, addr */
                cpu->PC = cpu->V[0] + inst_nnn_val;
                break;

            case 0xC: /* RND Vx, byte */
                {
                    uint8_t r = (uint8_t)(rand() & 0xFF);
                    cpu->V[inst_x_val] = r & inst_kk_val;
                }
                break;

            case 0xD: /* DRW Vx, Vy, nibble */
                {
                    int i;
                    cpu->V[0xF] = 0;
                    for (i = 0; i < inst_n_val; i++)
                    {
                        cpu->vidram[(inst_x_val % W) + ((inst_y_val % H) * H)] ^= cpu->memory[cpu->I+i];
                        if (cpu->vidram[(inst_x_val % W) + ((inst_y_val % W) * H)])
                            cpu->V[0xF] = 1;
                        cpu->V[0xF] = !(cpu->vidram[(inst_x_val % W) + ((inst_y_val % W) * H)]);
                    }
                }
                break;

            case 0xE:
                switch(inst_kk_val)
                {
                    case 0x9E: /* SKP Vx */
                        if (cpu->keys[cpu->V[inst_x_val]])
                            cpu->PC += 2;
                        break;

                    case 0xA1: /* SKNP Vx */
                        if (!cpu->keys[cpu->V[inst_x_val]])
                            cpu->PC += 2;
                        break;
                }
                break;

            case 0xF:
                switch (inst_kk_val)
                {
                    case 0x07: /* LD Vx, DT */
                        cpu->V[inst_x_val] = cpu->DT;
                        break;

                    case 0x0A: /* LD Vx, K */
                        cpu->waiting = 0x80;
                        break;

                    case 0x15: /* LD DT, Vx */
                        cpu->DT = cpu->V[inst_x_val];
                        break;

                    case 0x18: /* LD ST, Vx */
                        cpu->ST = cpu->V[inst_x_val];
                        break;

                    case 0x1E: /* ADD I, Vx */
                        cpu->I += cpu->V[inst_x_val];
                        break;

                    case 0x29: /* LD F, Vx */
                        cpu->I = cpu->font[cpu->V[inst_x_val] & 0xF];
                        break;

                    case 0x33: /* LD B, Vx */
                        {
                            int x = cpu->V[inst_x_val];
                            int place = 2;
                            while (x)
                            {
                                cpu->memory[cpu->I+place--] = x % 10;
                                x /= 10;
                            }
                        }
                        break;

                    case 0x55: /* LD [I], Vx */
                        {
                            int i;
                            for (i = 0; i < inst_x_val; i++)
                            {
                                cpu->memory[cpu->I++] = cpu->V[i];
                            }
                        }
                        break;

                    case 0x65: /* LD Vx, [I] */
                        {
                            int i;
                            for (i = 0; i < inst_x_val; i++)
                            {
                                cpu->V[i] = cpu->memory[cpu->I++];
                            }
                        }
                        break;
                }
                break;

            default:
                printf("Unrecognized opcode!");
                break;
        }
    }
}

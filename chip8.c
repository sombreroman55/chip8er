/* chip8.c
 * Chip8 "CPU" implementation
 */

#include <string.h>
#include <time.h>   /* For RNG seed */
#include <stdlib.h> /* For RNG and malloc */
#include <stdio.h>
#include "chip8.h"

#define PRE_MASK  0xF000
#define PRE_SHIFT 12

#define NNN_MASK  0x0FFF
#define N_MASK    0x000F

#define X_MASK    0x0F00
#define X_SHIFT   8

#define Y_MASK    0x00F0
#define Y_SHIFT   4

#define KK_MASK   0x00FF

static const uint8_t chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
    0x20, 0x60, 0x20, 0x20, 0x70,  // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
    0xF0, 0x80, 0xF0, 0x80, 0x80   // F
};

Chip8* chip8_init(void)
{
    Chip8* cpu = malloc(sizeof(Chip8));
    memset(cpu->display.vidram, 0, sizeof(cpu->display.vidram));
    cpu->PC = PROG_START;
    cpu->SP = 0x00;
    cpu->I  = 0x00;
    cpu->DT = 0x00;
    cpu->ST = 0x00;
    int i;
    for (i = 0; i < NUM_REGS; i++)
    {
        cpu->V[i] = 0x00;
    }

    /* Install the font */
    memcpy(&cpu->memory[FONT_OFFSET], chip8_fontset, sizeof(chip8_fontset));
    printf("Chip8 initialized\n");
    return cpu;
}

int chip8_load_file(Chip8* cpu, const char* rom)
{
    FILE* file = fopen(rom, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "Error! Cannot open ROM file!\n");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    unsigned long sz = ftell(file);
    rewind(file);

    if (sz > MEM_SIZE - PROG_START)
    {
        fprintf(stderr, "Error! ROM filesize too large!\n");
        return -1;
    }

    if (fread(&cpu->memory[PROG_START], 1, sz, file) != sz)
    {
        fprintf(stderr, "Error while reading ROM file!\n");
        return -1;
    }

    fclose(file);

    printf("ROM loaded\n");
    return 0;
}

void chip8_execute(Chip8* cpu)
{
    uint16_t instruction = (cpu->memory[cpu->PC] << 8) | cpu->memory[cpu->PC+1];
    uint16_t prefix      = (instruction & PRE_MASK) >> PRE_SHIFT;
    uint16_t nnn         = (instruction & NNN_MASK);
    uint16_t n           = (instruction & N_MASK);
    uint16_t x           = (instruction & X_MASK) >> X_SHIFT;
    uint16_t y           = (instruction & Y_MASK) >> Y_SHIFT;
    uint16_t kk          = (instruction & KK_MASK);
    cpu->PC += 2; /* Increment program counter */
    int i;
    switch(prefix)
    {
        case 0x0:
            switch(kk)
            {
                case 0xE0: /* CLS */
                    memset(cpu->display.vidram, 0, sizeof(cpu->display.vidram));
                    cpu->display.dirty = true;
                    break;

                case 0xEE: /* RET */
                    cpu->PC = cpu->stack[--cpu->SP];
                    break;
            }
            break;

        case 0x1: /* JP addr */
            cpu->PC = nnn;
            break;

        case 0x2: /* CALL addr */
            cpu->stack[cpu->SP++] = cpu->PC;
            cpu->PC = nnn;
            break;

        case 0x3: /* SE Vx, byte */
            if (cpu->V[x] == kk)
                cpu->PC += 2;
            break;

        case 0x4: /* SNE Vx, byte */
            if (cpu->V[x] != kk)
                cpu->PC += 2;
            break;

        case 0x5: /* SE Vx, Vy */
            if (cpu->V[x] == cpu->V[y])
                cpu->PC += 2;
            break;

        case 0x6: /* LD Vx, byte */
            cpu->V[x] = kk;
            break;

        case 0x7: /* ADD Vx, byte */
            cpu->V[x] += kk;
            break;

        case 0x8:
            switch(n)
            {
                case 0x0: /* LD Vx, Vy */
                    cpu->V[x] = cpu->V[y];
                    break;

                case 0x1: /* OR Vx, Vy */
                    cpu->V[x] |= cpu->V[y];
                    break;

                case 0x2: /* AND Vx, Vy */
                    cpu->V[x] &= cpu->V[y];
                    break;

                case 0x3: /* XOR Vx, Vy */
                    cpu->V[x] ^= cpu->V[y];
                    break;

                case 0x4: /* ADD Vx, Vy */
                    cpu->V[0xF] = cpu->V[x] + cpu->V[y] > 0xFF;
                    cpu->V[x]  += cpu->V[y];
                    break;

                case 0x5: /* SUB Vx, Vy */
                    cpu->V[0xF] = (uint8_t)(cpu->V[x] >= cpu->V[y]);
                    cpu->V[x]  -= cpu->V[y];
                    break;

                case 0x6: /* SHR Vx, Vy */
                    cpu->V[0xF] = cpu->V[x] & 0x1;
                    cpu->V[x] >>= 1;
                    break;

                case 0x7: /* SUBN Vx, Vy */
                    cpu->V[0xF] = (uint8_t)(cpu->V[x] <= cpu->V[y]);
                    cpu->V[x]   = cpu->V[y] - cpu->V[x];
                    break;

                case 0xE: /* SHL Vx, Vy */
                    cpu->V[0xF] = cpu->V[x] >> 7 & 0x1;
                    cpu->V[x] <<= 1;
                    break;
            }
            break;

        case 0x9: /* SNE Vx, Vy */
            if (cpu->V[x] != cpu->V[y])
                cpu->PC += 2;
            break;

        case 0xA: /* LD I, addr */
            cpu->I = nnn;
            break;

        case 0xB: /* JP V0, addr */
            cpu->PC = cpu->V[0x0] + nnn;
            break;

        case 0xC: /* RND Vx, byte */
            cpu->V[x] = (uint8_t)(rand() & 0xFF) & kk;
            break;

        case 0xD: /* DRW Vx, Vy, nibble */
            cpu->V[0xF] = 0;
            int px = cpu->V[x] & 63;
            int py = cpu->V[y] & 31;
            for (int i = 0; i < n; i++)
            {
                int yy = py + i;
                if (yy >= 32) break;
                uint8_t sprite = cpu->memory[cpu->I + i];
                for (int j = 0; j < 8; j++)
                {
                    uint8_t xx = px + j;
                    if (xx >= 64) break;
                    uint8_t pixel = (sprite & (1 << (7 - j))) >> (7 - j);

                    cpu->V[0xF] |= (cpu->display.vidram[yy][xx] & pixel);
                    cpu->display.vidram[yy][xx] ^= pixel;
                }
            }
            cpu->display.dirty = true;
            break;

        case 0xE:
            switch(kk)
            {
                case 0x9E: /* SKP Vx */
                    if (cpu->keys[cpu->V[x]])
                        cpu->PC += 2;
                    break;

                case 0xA1: /* SKNP Vx */
                    if (!cpu->keys[cpu->V[x]])
                        cpu->PC += 2;
                    break;
            }
            break;

        case 0xF:
            switch (kk)
            {
                case 0x07: /* LD Vx, DT */
                    cpu->V[x] = cpu->DT;
                    break;

                case 0x0A: /* LD Vx, K */
                    {
                        int pressed = 0;
                        while (!pressed)
                        {
                            for (i = 0; i < 16; i++)
                            {
                                if (cpu->keys[i])
                                {
                                    cpu->V[x] = i;
                                    pressed = 1;
                                    break;
                                }
                            }
                        }

                    }
                    break;

                case 0x15: /* LD DT, Vx */
                    cpu->DT = cpu->V[x];
                    break;

                case 0x18: /* LD ST, Vx */
                    cpu->ST = cpu->V[x];
                    break;

                case 0x1E: /* ADD I, Vx */
                    cpu->V[0xF] = cpu->I + cpu->V[x] > 0xFFF;
                    cpu->I += cpu->V[x];
                    break;

                case 0x29: /* LD F, Vx */
                    cpu->I = FONT_OFFSET + ((cpu->V[x] & 0x0F) * 5);
                    break;

                case 0x33: /* LD B, Vx */
                    cpu->memory[cpu->I + 2] = (cpu->V[x]/1)   % 10;
                    cpu->memory[cpu->I + 1] = (cpu->V[x]/10)  % 10;
                    cpu->memory[cpu->I + 0] = (cpu->V[x]/100) % 10;
                    break;

                case 0x55: /* LD [I], Vx */
                    for (i = 0; i <= x; i++)
                    {
                        cpu->memory[cpu->I + i] = cpu->V[i];
                    }
                    break;

                case 0x65: /* LD Vx, [I] */
                    for (i = 0; i <= x; i++)
                    {
                        cpu->V[i] = cpu->memory[cpu->I + i];
                    }
                    break;
            }
            break;

        default:
            printf("Unrecognized opcode!");
            break;
    }
}

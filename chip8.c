/* cpu.c
 * Chip8 "CPU" implementation
 */

#include <string.h>
#include <time.h>   /* For RNG seed */
#include <stdlib.h> /* For RNG */
#include <stdio.h>
#include "chip8.h"

static const uint8_t chip8_fontset[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

int chip8_init(Chip8* cpu)
{
    memset(cpu->vidram, 0, VIDRAM_SIZE);
    cpu->PC = MEM_OFFSET;
    cpu->SP = 0x00;
    cpu->I  = 0x00;
    cpu->DT = 0x00;
    cpu->ST = 0x00;
    cpu->update_screen = 0x00;
    int i;
    for (i = 0; i < 0x10; i++)
    {
        cpu->V[i] = 0x00;
    }

    /* Install the font */
    memcpy(&cpu->memory[FONT_OFFSET], chip8_fontset, sizeof(chip8_fontset));
    return 0;
}

int  chip8_load_file(Chip8* const cpu, const char* rom)
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

    if (sz > MEM_SIZE - MEM_OFFSET)
    {
        fprintf(stderr, "Error! ROM filesize too large!\n");
        return -1;
    }

    if (fread(&cpu->memory[MEM_OFFSET], 1, sz, file) != sz)
    {
        fprintf(stderr, "Error while reading ROM file!\n");
        return -1;
    }

    fclose(file);

    return 0;
}

void chip8_execute(Chip8* cpu)
{
    uint16_t instruction  =  (cpu->memory[cpu->PC] << 8) 
        | cpu->memory[cpu->PC+1];
    uint16_t prefix  = (instruction & PRE_MASK) >> 12;
    uint16_t nnn = (instruction & NNN_MASK);
    uint16_t n   = (instruction & N_MASK);
    uint16_t x   = (instruction & X_MASK) >> 8;
    uint16_t y   = (instruction & Y_MASK) >> 4;
    uint16_t kk  = (instruction & KK_MASK);
    cpu->PC += 2; /* Increment program counter */
    int i;
    switch(prefix)
    {
        case 0x0:
            switch(kk)
            {
                case 0xE0: /* CLS */
                    memset(cpu->vidram, 0, VIDRAM_SIZE);
                    cpu->update_screen = 0x1;
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
            cpu->V[x] == kk;
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
                    cpu->V[x] += cpu->V[y];
                    break;

                case 0x5: /* SUB Vx, Vy */
                    cpu->V[0xF] = cpu->V[x] >= cpu->V[y];
                    cpu->V[x] -= cpu->V[y];
                    break;

                case 0x6: /* SHR Vx, Vy */
                    cpu->V[0xF] = cpu->V[x] & 0x1;
                    cpu->V[x] >>= 1;
                    break;

                case 0x7: /* SUBN Vx, Vy */
                    cpu->V[0xF] = cpu->V[x] <= cpu->V[y];
                    cpu->V[x] = cpu->V[y] - cpu->V[x];
                    break;

                case 0xE: /* SHL Vx, Vy */
                    cpu->V[0xF] = cpu->V[x] >> 7;
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
            for (int yline = 0; yline < n; yline++)
            {
                uint8_t sprite = cpu->memory[cpu->I + yline];
                for (int xline = 0; xline < 8; xline++)
                {
                    int px = (cpu->V[x] + xline) & 63;
                    int py = (cpu->V[y] + yline) & 31;
                    int pos = 64 * py + px;
                    int pixel = (sprite >> (7 - xline)) & 0x1;

                    cpu->V[0xF] |= (cpu->vidram[pos] & pixel);
                    cpu->vidram[pos] ^= pixel;
                }
            }
            cpu->update_screen = 0x1;
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
                        for (i = 0; i < 16; i++)
                        {
                            if (cpu->keys[i])
                            {
                                cpu->V[x] = i;
                                pressed = 1;
                                break;
                            }
                        }

                        /* Do this until a key is pressed */
                        if (!pressed)
                        {
                            cpu->PC -= 2;
                            return;
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
                    cpu->I = FONT_OFFSET + (cpu->V[x] & 0xF) * 5;
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

    /* Decrement timers */
    if (cpu->DT > 0) { cpu->DT--; }
    if (cpu->ST > 0) { if (cpu->ST == 1) printf("poop\n"); cpu->ST--; }
}

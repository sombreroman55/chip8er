/* main.c
 * Main entry point for program
 */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "chip8.h"

#define SCREEN_W 640
#define SCREEN_H 320
#define CPU_HZ   500

int main (int argc, char** argv)
{
    srand(time(NULL));
    if (argc != 2)
    {
        fprintf(stderr, "Invalid number of arguments!\nUsage: ./chip8er <rom>\n");
        return -1;
    }

    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return -1;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_SetHint(SDL_HINT_BMP_SAVE_LEGACY_FORMAT, "1");


    /* Create SDL Window */
    SDL_Window* window = SDL_CreateWindow("chip8er",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            SCREEN_W,
            SCREEN_H,
            SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        SDL_Log("Failed to create SDL window: %s", SDL_GetError());
        return -1;
    }

    SDL_SetWindowMinimumSize(window, SCREEN_W, SCREEN_H);


    /* Create SDL Renderer */
    SDL_Renderer* renderer = SDL_CreateRenderer(
            window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        SDL_Log("Failed to create SDL renderer: %s", SDL_GetError());
        return -1;
    }

    /* Print renderer info */
    SDL_RendererInfo rend_info;
    SDL_GetRendererInfo(renderer, &rend_info);
    const int vsync    = rend_info.flags & SDL_RENDERER_PRESENTVSYNC;
    const int hw_accel = rend_info.flags & SDL_RENDERER_ACCELERATED;
    SDL_Log("Using %s video renderer.\n"
            "VSYNC present:         %s\n"
            "Hardware Acceleration: %s\n", rend_info.name,
            ((vsync) ? "YES" : "NO"), ((hw_accel) ? "YES" : "NO"));

    /* Set up Chip8 system */
    Chip8* cpu = chip8_init();
    if (chip8_load_file(cpu, argv[1]) != 0)
    {
        SDL_Log("Failed to load rom %s.", argv[1]);
        return -1;
    }

    SDL_Event ev;
    bool quit = false;
    while (!quit)
    {
        while (SDL_PollEvent(&ev) != 0)
        {
            if (ev.type == SDL_QUIT)
            {
                quit = 1;
            }
            else if (ev.type == SDL_KEYDOWN)
            {
                switch (ev.key.keysym.sym)
                {
                    case SDLK_1: cpu->keys[0x1] = 0x1; break;
                    case SDLK_2: cpu->keys[0x2] = 0x1; break;
                    case SDLK_3: cpu->keys[0x3] = 0x1; break;
                    case SDLK_4: cpu->keys[0xC] = 0x1; break;
                    case SDLK_a: cpu->keys[0x4] = 0x1; break;
                    case SDLK_z: cpu->keys[0x5] = 0x1; break;
                    case SDLK_e: cpu->keys[0x6] = 0x1; break;
                    case SDLK_r: cpu->keys[0xD] = 0x1; break;
                    case SDLK_q: cpu->keys[0x7] = 0x1; break;
                    case SDLK_s: cpu->keys[0x8] = 0x1; break;
                    case SDLK_d: cpu->keys[0x9] = 0x1; break;
                    case SDLK_f: cpu->keys[0xE] = 0x1; break;
                    case SDLK_w: cpu->keys[0xA] = 0x1; break;
                    case SDLK_x: cpu->keys[0x0] = 0x1; break;
                    case SDLK_c: cpu->keys[0xB] = 0x1; break;
                    case SDLK_v: cpu->keys[0xF] = 0x1; break;
                }
            }
            else if (ev.type == SDL_KEYUP)
            {
                switch (ev.key.keysym.sym)
                {
                    case SDLK_1: cpu->keys[0x1] = 0x0; break;
                    case SDLK_2: cpu->keys[0x2] = 0x0; break;
                    case SDLK_3: cpu->keys[0x3] = 0x0; break;
                    case SDLK_4: cpu->keys[0xC] = 0x0; break;
                    case SDLK_a: cpu->keys[0x4] = 0x0; break;
                    case SDLK_z: cpu->keys[0x5] = 0x0; break;
                    case SDLK_e: cpu->keys[0x6] = 0x0; break;
                    case SDLK_r: cpu->keys[0xD] = 0x0; break;
                    case SDLK_q: cpu->keys[0x7] = 0x0; break;
                    case SDLK_s: cpu->keys[0x8] = 0x0; break;
                    case SDLK_d: cpu->keys[0x9] = 0x0; break;
                    case SDLK_f: cpu->keys[0xE] = 0x0; break;
                    case SDLK_w: cpu->keys[0xA] = 0x0; break;
                    case SDLK_x: cpu->keys[0x0] = 0x0; break;
                    case SDLK_c: cpu->keys[0xB] = 0x0; break;
                    case SDLK_v: cpu->keys[0xF] = 0x0; break;
                }
            }
        }

        int y_scale = SCREEN_H / VIDRAM_H;
        int x_scale = SCREEN_W / VIDRAM_W;
        SDL_Rect pixel = {0, 0, x_scale, y_scale};

        for (int i = 0; i < 16; i++)
        {
            chip8_execute(cpu);
        }

        /* Update screen if necessary */
        if (cpu->display.dirty)
        {
            for (int i = 0; i < VIDRAM_H; i++)
            {
                for (int j = 0; j < VIDRAM_W; j++)
                {
                    SDL_SetRenderDrawColor(renderer,
                                           0xFF * cpu->display.vidram[i][j],
                                           0xFF * cpu->display.vidram[i][j],
                                           0xFF * cpu->display.vidram[i][j],
                                           0xFF);
                    pixel.x = j * x_scale;
                    pixel.y = i * y_scale;
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }
            cpu->display.dirty = false;
            SDL_RenderPresent(renderer);
        }

        /* Decrement timers */
        if (cpu->DT) cpu->DT--;
        if (cpu->ST) cpu->ST--;
        SDL_Delay(1000/60);
    }

    /* Free SDL resources and quit */
    free(cpu);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

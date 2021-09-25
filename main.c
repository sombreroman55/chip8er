/* main.c
 * Main entry point for program
 */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "chip8.h"

uint8_t colors = {0x000000, 0xFFFFFF};

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
            SCREEN_W * 10,
            SCREEN_H * 10,
            SDL_WINDOW_RESIZABLE);
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


    /* Create SDL texture */
    SDL_Texture* texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_TARGET,
            SCREEN_W,
            SCREEN_H);
    if (texture == NULL)
    {
        SDL_Log("Failed to create SDL texture: %s", SDL_GetError());
        return -1;
    }


    /* Clear out texture */
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);


    /* Set up Chip8 system */
    Chip8* cpu = chip8_init();
    if (chip8_load_file(cpu, argv[1]) != 0) 
    { 
        SDL_Log("Failed to load rom %s.", argv[1]);
        return -1; 
    }

    uint32_t timer = SDL_GetTicks();
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

        /* Tick CPU every 10ms */
        int pixels[VIDRAM_SIZE];
        if (SDL_GetTicks() - timer >= 10)
        {
            int i;
            for (i = 0; i < 3; i++)
            {
                chip8_execute(cpu);
            }

            /* Update screen if necessary */
            for (i = 0; i < VIDRAM_SIZE; i++)
            {
                pixels[i] = 0xFFFFFF * ((cpu->vidram[i/8] >> (7 - i%8)) & 1);
            }
        }

        SDL_UpdateTexture(texture, NULL, pixels, 4*SCREEN_W);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_Delay(1000/60);
    }

    /* Free SDL resources and quit */
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

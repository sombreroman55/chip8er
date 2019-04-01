/* main.c
 * Main entry point for program
 */

#include <SDL2/SDL.h>
#include "chip8.h"

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480

int main (int argc, char** argv)
{
    Chip8 cpu;          /* Chip8 system */
    cpu_init(&cpu);

    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;

    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        sprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        window = SDL_CreateWindow("Chip8er", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            sprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        }
        else
        {
            screenSurface = SDL_GetWindowSurface(window);
            SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xAB, 0xCD, 0xEF));
            SDL_UpdateWindowSurface(window);
            SDL_Delay(2000);
        }
    }
    SDL_DestroyWindow(window);
    SDL_Quit();

    //cpu_execute(&cpu);
    //printf("PC: 0x%04X\n", cpu.PC);
    return 0;
}

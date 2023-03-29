#ifndef DISPLAY
#define DISPLAY

#include <SDL2/SDL.h>
#include "Chip8.h"

#define PX_SCALE 20

typedef struct {
    SDL_Window* win;
    SDL_Renderer* rd;
} WindowWrap;

WindowWrap Chip8Display_Init();

void Chip8Display_Clear(Chip8* ch8);

void Chip8Display_Show(Chip8* ch8, WindowWrap* wrap, void (*update)(Chip8*));

void Chip8Display_Pixel(Chip8* ch8, int x, int y);

void Chip8Display_Sprite(Chip8* ch8, uint8_t* spr, int x, int y, int h);

#endif
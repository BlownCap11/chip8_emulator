#include <stdio.h>
#include <stdlib.h>

#include "../Include/Display.h"
#include "../Include/Chip8.h"

WindowWrap Chip8Display_Init() {
    SDL_Init(SDL_INIT_VIDEO);

    WindowWrap wrap;

    // Window Initialization
    wrap.win = SDL_CreateWindow(
        "Chip8 Display",
        0, 0,
        WIDTH*PX_SCALE,
        HEIGHT*PX_SCALE,
        SDL_WINDOW_SHOWN
    );
    wrap.rd = SDL_CreateRenderer(wrap.win, -1, SDL_RENDERER_ACCELERATED);

    return wrap;
}

void Chip8Display_Clear(Chip8* ch8) {
    for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                ch8->framebuffer[y][x] = 0;
            }
        }
}

void Chip8Display_Show(Chip8* ch8, WindowWrap* wrap, void (*update)(Chip8*)) {
    SDL_Event ev;
    int quit = 0;
    while (!quit) {
        SDL_SetRenderDrawColor(wrap->rd, 0, 0, 0, 255);
        SDL_RenderClear(wrap->rd);

        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {
                quit = 1;
            }
            if (ev.type == SDL_KEYDOWN) {
                if (ev.key.keysym.sym == SDLK_0) {
                    ch8->keydown = 0;
                }
                if (ev.key.keysym.sym == SDLK_1) {
                    ch8->keydown = 1;
                }
                if (ev.key.keysym.sym == SDLK_2) {
                    ch8->keydown = 2;
                }
                if (ev.key.keysym.sym == SDLK_3) {
                    ch8->keydown = 3;
                }
                if (ev.key.keysym.sym == SDLK_4) {
                    ch8->keydown = 4;
                }
                if (ev.key.keysym.sym == SDLK_5) {
                    ch8->keydown = 5;
                }
                if (ev.key.keysym.sym == SDLK_6) {
                    ch8->keydown = 6;
                }
                if (ev.key.keysym.sym == SDLK_7) {
                    ch8->keydown = 7;
                }
                if (ev.key.keysym.sym == SDLK_8) {
                    ch8->keydown = 8;
                }
                if (ev.key.keysym.sym == SDLK_9) {
                    ch8->keydown = 9;
                }
                if (ev.key.keysym.sym == SDLK_a) {
                    ch8->keydown = 0xA;
                }
                if (ev.key.keysym.sym == SDLK_b) {
                    ch8->keydown = 0xB;
                }
                if (ev.key.keysym.sym == SDLK_c) {
                    ch8->keydown = 0xC;
                }
                if (ev.key.keysym.sym == SDLK_d) {
                    ch8->keydown = 0xD;
                }
                if (ev.key.keysym.sym == SDLK_e) {
                    ch8->keydown = 0xE;
                }
                if (ev.key.keysym.sym == SDLK_f) {
                    ch8->keydown = 0xF;
                }
            }
            if (ev.type == SDL_KEYUP) {
                ch8->keydown = 0;
            }
        }

        // Update Display
        SDL_SetRenderDrawColor(wrap->rd, 0, 255, 0, 255);
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                if (ch8->framebuffer[y][x] > 0) {
                    SDL_Rect pix = {x*PX_SCALE, y*PX_SCALE, PX_SCALE, PX_SCALE};
                    SDL_RenderDrawRect(wrap->rd, &pix);
                    SDL_RenderFillRect(wrap->rd, &pix);
                }
            }
        }

        // Custom Update
        if (update != NULL) {
            update(ch8);
        }

        SDL_RenderPresent(wrap->rd);
        SDL_Delay(5);
    }
}

void Chip8Display_Pixel(Chip8* ch8, int x, int y) {
    x %= WIDTH;
    y %= HEIGHT;
    if (ch8->framebuffer[y][x]) {
        ch8->regs[VF] = 1;
    } else {
        ch8->regs[VF] = 0;
    }
    ch8->framebuffer[y][x] ^= 1;
}

void Chip8Display_Sprite(Chip8* ch8, uint8_t* spr, int x, int y, int h) {
    uint8_t sprite_x = x % WIDTH;
    uint8_t sprite_y = y % HEIGHT;
    for (int row = 0; row < h; row++) {
        uint8_t row_byte = spr[row];
        for (int col = 0; col < 8; col++) {
            uint8_t sprite_px = row_byte & (0x80 >> col);
            if (sprite_px) {
                Chip8Display_Pixel(ch8, sprite_x+col, sprite_y+row);
            }
        }
    }
}
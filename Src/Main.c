#include <stdio.h>
#include <stdlib.h>

#include "../Include/Display.h"
#include "../Include/Chip8.h"
#include "../Include/Fonts.h"
#include "../Include/Debug.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        puts("Usage: ./Chip8 <rom>");
        exit(EXIT_FAILURE);
    }

    Chip8 ch8 = Chip8_Init();
    WindowWrap ch8_display = Chip8Display_Init();

    Chip8Fonts_Load(&ch8);
    Chip8_LoadRom(&ch8, argv[1]);

    Chip8Display_Clear(&ch8);
    Chip8Display_Show(&ch8, &ch8_display, Chip8_Cycle);

    free(ch8.memory);
    return 0;
}
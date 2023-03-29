#include <stdio.h>

#include "../Include/Debug.h"

void Chip8Debug_PrintFB(Chip8* ch8) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (ch8->framebuffer[y][x]) {
                printf("#");
            } else {
                printf("-");
            }
        }
        printf("\n");
    }
}

void Chip8Debug_PrintMem(Chip8* ch8, int16_t start, int16_t n) {
    for (int i = start; i < n; i++) {
        printf("0x%x => %x\n", i, ch8->memory[i]);
    }
}

void Chip8Debug_PrintReg(Chip8* ch8) {
    printf(
        "PC: %x SP: %x Index: %x Delay: %x\n",
        ch8->pc, ch8->sp, ch8->index, ch8->delay
    );
    for (int i = 0; i < 16; i++) {
        printf("V%x: %x\n", i, ch8->regs[i]);
    }
}

void Chip8Debug_Step(Chip8* ch8) {
    printf("> ");
    char c = getchar();
    if (c == 'p') {
        Chip8Debug_PrintMem(ch8, ROM_START, 1024);
    } else if (c == 'r') {
        Chip8Debug_PrintReg(ch8);
    }
}
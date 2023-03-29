#ifndef C_DEBUG
#define C_DEBUG

#include "Chip8.h"

void Chip8Debug_PrintFB(Chip8* ch8);

void Chip8Debug_PrintMem(Chip8* ch8, int16_t start, int16_t n);

void Chip8Debug_PrintReg(Chip8* ch8);

void Chip8Debug_Step(Chip8* ch8);

#endif
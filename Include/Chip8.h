#ifndef CHIP8
#define CHIP8

#include <stdint.h>

#define WIDTH 64
#define HEIGHT 32

#define MEMORY_SIZE 4096
#define ROM_START 0x200
#define STACK_SIZE 1024

#define PrintError_(err) { printf("[Error] %s\n", err); exit(EXIT_FAILURE); }

typedef struct {
    uint8_t*    memory;      // 4K Heap Allocated
    uint16_t    pc;          // Program Counter
    uint16_t    index;       // Index Register
    uint8_t     regs[16];    // 16 8-bit Registers
    uint16_t    stack[1024]; // 2-byte Entries
    uint8_t     sp;          // 4-bit Stack Pointer
    uint8_t     framebuffer[HEIGHT][WIDTH];
    int delay;
    uint8_t keyup;           // Key Recently Released
    uint8_t keydown;         // Key Recently Pressed
} Chip8;

// Registers
enum {
    V0 = 0x0,
    V1, V2, V3, V4,
    V5, V6, V7, V8,
    V9, VA, VB, VC,
    VD, VE, VF,
};

//  Flags
#define FLG_DRAW 0xDA
#define FLG_COLL 0xCF

// Functions
Chip8 Chip8_Init();

void Chip8_LoadRom(Chip8* ch8, const char* path);

void Chip8_Cycle(Chip8* ch8);

#endif
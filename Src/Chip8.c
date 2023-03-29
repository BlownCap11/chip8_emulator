#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "../Include/Chip8.h"
#include "../Include/Display.h"
#include "../Include/Fonts.h"

Chip8 Chip8_Init() {
    Chip8 ch8;
    ch8.memory = malloc(MEMORY_SIZE);
    ch8.index  = 0;
    ch8.pc     = ROM_START;
    ch8.sp     = 0;
    ch8.delay  = 0;
    // Registers
    for (int r = 0; r < 16; r++) {
        ch8.regs[r] = 0;
    }
    // Stack
    for (int off = 0; off < STACK_SIZE; off++) {
        ch8.stack[off] = 0;
    }
    return ch8;
}

void Chip8_LoadRom(Chip8* ch8, const char* path) {
    FILE* rom = fopen(path, "rb");
    if (rom == NULL) {
        PrintError_("ROM not found");
    }
    // Finding Length of ROM
    fseek(rom, 0, SEEK_END);
    int rom_len = ftell(rom);
    rewind(rom);
    // Checking ROM Size
    if (rom_len > (MEMORY_SIZE-0x200)) {
        PrintError_("ROM Size too big");
    }
    // Loading ROM
    unsigned char* rom_content = malloc(rom_len*sizeof(unsigned char));
    fread(rom_content, sizeof(unsigned char), rom_len, rom);
    memcpy(ch8->memory+ROM_START, rom_content, rom_len);
    free(rom_content);
}

void Chip8_Cycle(Chip8* ch8) {
    // Fetch
    if (ch8->pc >= 4096) {
        PrintError_("Program Counter Overflowed!");
    }
    uint16_t opcode = ch8->memory[ch8->pc] << 8 | ch8->memory[ch8->pc+1];
    ch8->pc += 2;
    
    // Decrement the timer
    if (ch8->delay > 0) {
        ch8->delay--;
    }

    // Decode & Execute
    switch (opcode & 0xF000) {
        case 0x0000: {
            switch (opcode & 0xFF) {
                case 0xE0: {
                    Chip8Display_Clear(ch8);
                    break;
                }
                case 0xEE: {
                    // Subroutine Return
                    if (ch8->sp > 0) {
                        ch8->pc = ch8->stack[ch8->sp-1];
                        ch8->sp--;
                    }
                    break;
                }
                default: {
                    printf("Unknown 0x0000: %x\n", opcode);
                    break;
                }
            }
            break;
        }
        case 0x1000: {
            // Jump
            uint16_t dest = opcode & 0xFFF;
            ch8->pc = dest;
            break;
        }
        case 0x2000: {
            // Subroutine call
            uint16_t subr = opcode & 0xFFF;
            ch8->stack[ch8->sp] = ch8->pc;
            ch8->sp++;
            ch8->pc = subr;
            break;
        }
        case 0x3000: {
            // Skip if vx == kk
            uint8_t vx = (opcode & 0xF00) >> 8;
            uint8_t val = (opcode & 0xFF);
            if (ch8->regs[vx] == val) {
                ch8->pc += 2;
            }
            break;
        }
        case 0x4000: {
            // Skip if vx != kk
            uint8_t vx = (opcode & 0xF00) >> 8;
            uint8_t val = (opcode & 0xFF);
            if (ch8->regs[vx] != val) {
                ch8->pc += 2;
            }
            break;
        }
        case 0x5000: {
            // Skip if vx == vy
            uint8_t vx = (opcode & 0xF00) >> 8;
            uint8_t vy = (opcode & 0xF0)  >> 4;
            if (ch8->regs[vx] == ch8->regs[vy]) {
                ch8->pc += 2;
            }
            break;
        }
        case 0x6000: {
            // Load Reg
            uint8_t reg = (opcode & 0xF00) >> 8;
            uint8_t val = opcode & 0xFF;
            ch8->regs[reg] = val;
            break;
        }
        case 0x7000: {
            // Add reg
            uint8_t reg = (opcode & 0xF00) >> 8;
            uint8_t val = opcode & 0xFF;
            ch8->regs[reg] += val;
            break;
        }
        case 0x8000: {
            uint8_t vx = (opcode & 0xF00) >> 8;
            uint8_t vy = (opcode & 0xF0)  >> 4;
            switch (opcode & 0xF) {
                case 0x0: {
                    ch8->regs[vx] = ch8->regs[vy];
                    break;
                }
                case 0x1: {
                    ch8->regs[vx] |= ch8->regs[vy];
                    break;
                }
                case 0x2: {
                    ch8->regs[vx] &= ch8->regs[vy];
                    break;
                }
                case 0x3: {
                    ch8->regs[vx] ^= ch8->regs[vy];
                    break;
                }
                case 0x4: {
                    uint16_t sum = ch8->regs[vx] + ch8->regs[vy];
                    if (sum > 255) {
                        ch8->regs[VF] = 1;
                    } else {
                        ch8->regs[VF] = 0;
                    }
                    ch8->regs[vx] = sum & 0xFF;
                    break;
                }
                case 0x5: {
                    if (ch8->regs[vx] > ch8->regs[vy]) {
                        ch8->regs[VF] = 1;
                    } else {
                        ch8->regs[VF] = 0;
                    }
                    ch8->regs[vx] -= ch8->regs[vy];
                    break;
                }
                case 0x6: {
                    ch8->regs[VF] = (ch8->regs[vx] & 0x1);
                    ch8->regs[vx] >>= 1;
                    break;
                }
                case 0x7: {
                    if (ch8->regs[vy] > ch8->regs[vx]) {
                        ch8->regs[VF] = 1;
                    } else {
                        ch8->regs[VF] = 0;
                    }
                    ch8->regs[vx] = ch8->regs[vy] - ch8->regs[vx];
                    break;
                }
                case 0xE: {
                    ch8->regs[VF] = (ch8->regs[vx] & 0x80) >> 7;
                    ch8->regs[vx] <<= 1;
                    break;
                }
                default: {
                    printf("Unknown 0x8000: %x\n", opcode);
                    break;
                }
            }
            break;
        }
        case 0x9000: {
            uint8_t vx = (opcode & 0xF00) >> 8;
            uint8_t vy = (opcode & 0xF0)  >> 4;
            if (ch8->regs[vx] != ch8->regs[vy]) {
                ch8->pc += 2;
            }
            break;
        }
        case 0xA000: {
            // Set Index reg
            ch8->index = opcode & 0xFFF;
            break;
        }
        case 0xB000: {
            uint16_t dest = (opcode & 0xFFF);
            ch8->pc = dest + ch8->regs[V0];
            break;
        }
        case 0xC000: {
            uint8_t vx = (opcode & 0xF00) >> 8;
            uint8_t byte = opcode & 0xFF;
            uint8_t r  = 23;
            ch8->regs[vx] = r & byte;
            break;
        }
        case 0xD000: {
            // Draw
            uint8_t sprite_len = opcode & 0xF;
            uint8_t sprite_x   = ch8->regs[(opcode & 0xF00) >> 8];
            uint8_t sprite_y   = ch8->regs[(opcode & 0xF0)  >> 4];
            uint8_t* spritesheet = ch8->memory+ch8->index;
            Chip8Display_Sprite(ch8, spritesheet, sprite_x, sprite_y, sprite_len);
            break;
        }
        case 0xE000: {
            uint8_t vx = (opcode & 0xF00) >> 8;
            switch (opcode & 0xFF) {
                case 0x9E: {
                    if (vx == ch8->keydown) {
                        ch8->pc += 2;
                    }
                    break;
                }
                case 0xA1: {
                    if (vx != ch8->keydown) {
                        ch8->pc += 2;
                    }
                    break;
                }
                default: {
                    printf("Unknown 0xE000: %x\n", opcode);
                    break;
                }
            }
            break;
        }
        case 0xF000: {
            uint8_t vx = (opcode & 0xF00) >> 8;
            switch (opcode & 0xFF) {
                case 0x07: {
                    ch8->regs[vx] = ch8->delay;
                    break;
                }
                case 0x0A: {
                    uint8_t c_key = ch8->keydown;
                    if (ch8->keydown == c_key) {
                        ch8->pc -= 2;
                    } else {
                        ch8->regs[vx] = ch8->keydown;
                    }
                    break;
                }
                case 0x15: {
                    ch8->delay = ch8->regs[vx];
                    break;
                }
                case 0x18: {
                    printf("BEEEP!\n");
                    break;
                }
                case 0x1E: {
                    ch8->index += ch8->regs[vx];
                    break;
                }
                case 0x29: {
                    uint8_t digit = ch8->regs[vx];
                    ch8->index = FONT_START + (5 * digit);
                    break;
                }
                case 0x33: {
                    uint8_t val = ch8->regs[vx];
                    ch8->memory[ch8->index+2] = val % 10;
                    val /= 10;
                    ch8->memory[ch8->index+1] = val % 10;
                    val /= 10;
                    ch8->memory[ch8->index] = val % 10;
                    break;
                }
                case 0x55: {
                    for (uint8_t i = 0; i <= vx; i++) {
                        ch8->memory[ch8->index+i] = ch8->regs[i];
                    }
                    break;
                }
                case 0x65: {
                    for (uint8_t i = 0; i <= vx; i++) {
                        ch8->regs[i] = ch8->memory[ch8->index+i];
                    }
                    break;
                }
                default: {
                    printf("Unknown 0xF000: %x\n", opcode);
                    break;
                }
            }
            break;
        }
        default: {
            printf("Unknown: %x\n", opcode);
            break;
        }
    }
}
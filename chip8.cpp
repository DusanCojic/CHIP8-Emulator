#ifndef CHIP8
#define CHIP8

#include "chip8.h"

#include <fstream>

Chip8::Chip8() {
    // fontset values
    const uint8_t fontset[16][5] = {
        {0xF0, 0x90, 0x90, 0x90, 0xF0}, // 0
        {0x20, 0x60, 0x20, 0x20, 0x70}, // 1
        {0xF0, 0x10, 0xF0, 0x80, 0xF0}, // 2
        {0xF0, 0x10, 0xF0, 0x10, 0xF0}, // 3
        {0x90, 0x90, 0xF0, 0x10, 0x10}, // 4
        {0xF0, 0x80, 0xF0, 0x10, 0xF0}, // 5
        {0xF0, 0x80, 0xF0, 0x90, 0xF0}, // 6
        {0xF0, 0x10, 0x20, 0x40, 0x40}, // 7
        {0xF0, 0x90, 0xF0, 0x90, 0xF0}, // 8
        {0xF0, 0x90, 0xF0, 0x10, 0xF0}, // 9
        {0xF0, 0x90, 0xF0, 0x90, 0x90}, // A
        {0xE0, 0x90, 0xE0, 0x90, 0xE0}, // B
        {0xF0, 0x80, 0x80, 0x80, 0xF0}, // C
        {0xE0, 0x90, 0x90, 0x90, 0xE0}, // D
        {0xF0, 0x80, 0xF0, 0x80, 0xF0}, // E
        {0xF0, 0x80, 0xF0, 0x80, 0x80}  // F
    };

    pc = 0x200; // starting address
    index = 0;

    delay_timer = 0;
    sound_timer = 0;

    memset(v, 0, sizeof(v)); // initialize all V registers to 0
    memset(keyboard, 0, sizeof(keyboard)); // initialize keyboard to 0
    memset(memory, 0, sizeof(memory)); // initialize memory to 0
    memset(display, 0, sizeof(display)); // initialize empty display

    sp = 0; // first empty stack location
    memset(stack, 0, sizeof(stack)); // initialize empty stack

    // load fontset in memory
    uint16_t fontset_start_addr = 0x050;
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 5; j++)
            memory[fontset_start_addr + i*5 + j] = fontset[i][j];
}

bool Chip8::load_rom(std::string path) {
    if (path.empty()) return false;

    std::ifstream rom(path, std::ios_base::binary);
    if (!rom) {
        std::cout << "Cannot open a file" << std::endl;
        return false;
    }

    uint16_t start_addr = 0x200; // start address in memory
    char ch;
    while (rom.read(&ch, 1)) {
        if (start_addr > 4096)
            return false;

        uint8_t byte = (uint8_t)ch;
        memory[start_addr++] = byte;
    }

    return true;
}

#endif
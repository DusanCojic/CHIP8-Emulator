#include <iostream>
#include "chip8.h"

int main() {
    Chip8 chip8;

    chip8.load_rom(R"(D:\Projects\CHIP8 Emulator\roms\IBM Logo.ch8)");

    return 0;
}
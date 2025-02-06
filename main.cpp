#include <iostream>
#include "chip8.h"

#include <SDL3/SDL.h>

int main() {
    Chip8 chip8;

    chip8.load_rom(R"(D:\Projects\CHIP8 Emulator\roms\delay_timer_test.ch8)");

    chip8.emulate();

    return 0;
}
#include <iostream>
#include <string>
#include "chip8.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Program requires an argument" << endl;
        return 1;
    }

    Chip8 chip8;

    std::string rom_path = "./roms/" + string(argv[1]) + ".ch8";
    if (!chip8.load_rom(rom_path))
        return 1;

    chip8.emulate();

    return 0;
}
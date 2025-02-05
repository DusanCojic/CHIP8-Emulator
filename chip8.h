#include <iostream>
#include <cstdint>
#include <cstring>

class Chip8 {
private:
    uint8_t memory[4096]; // 4KB of memory
    bool display[32][64]; // monochomatic 32x64 diplsay
    bool draw_flag; // not to rerender if display did not change

    uint16_t pc; // program counter
    uint16_t index; // index register

    uint16_t stack[16]; // stack
    uint8_t sp; // stack pointer

    uint8_t v[16]; // V0-VF general purpose registers

    uint8_t delay_timer;
    uint8_t sound_timer;

    bool keyboard[4][4]; // keyboard grid


    void single_cycle(); // emulates single cycle of the CPU
public:
    Chip8(); // constructor

    bool load_rom(std::string); // loading the rom file

    void emulate(); // emulate the process
};
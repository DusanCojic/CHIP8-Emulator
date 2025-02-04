#include <iostream>
#include <cstdint>
#include <cstring>

class Chip8 {
private:
    uint8_t memory[4096]; // 4KB of memory
    bool display[64][32]; // monochomatic 64x32 diplsay

    uint16_t pc; // program counter
    uint16_t index; // index register

    uint16_t stack[16]; // stack
    uint8_t sp; // stack pointer

    uint8_t v[16]; // V0-VF general purpose registers

    uint8_t delay_timer;
    uint8_t sound_timer;

    bool keyboard[4][4]; // keyboard grid

public:
    Chip8(); // constructor

    bool load_rom(std::string); // loading the rom file

    void single_cycle(); // emulates single cycle of the CPU

    // instructions
    void clear_scr();
    void jump();
    void set_VX();
    void add_to_VX();
    void set_index();
    void draw();
};
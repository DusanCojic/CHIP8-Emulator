#include "chip8.h"

#include <fstream>
#include <SDL3/SDL.h>
#include <random>

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
    draw_flag = false;

    sp = 0; // first empty stack location
    memset(stack, 0, sizeof(stack)); // initialize empty stack

    // load fontset in memory
    uint16_t fontset_start_addr = 0x050;
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 5; j++)
            memory[fontset_start_addr + i*5 + j] = fontset[i][j];

    srand(time(NULL));
}

bool Chip8::load_rom(std::string path) {
    if (path.empty()) return false;

    std::ifstream rom(path, std::ios_base::binary);
    if (!rom) {
        std::cout << "Cannot open a file" << std::endl;
        return false;
    }

    uint16_t addr = 0x200; // start address in memory
    char ch;
    while (rom.read(&ch, 1)) {
        if (addr > 4096)
            return false;

        uint8_t byte = (uint8_t)ch;
        memory[addr++] = byte;
    }

    return true;
}

void Chip8::single_cycle() {
    uint16_t op = (memory[pc] << 8) | memory[pc + 1];

    uint16_t addr;
    uint8_t reg, reg1, reg2, val;
    uint8_t op_subcode;

    uint8_t mask, rnd;

    uint8_t op_code = op >> 12;
    switch (op_code) {
        case 0:
            switch (op) {
                case 0x00E0:
                    memset(display, 0, sizeof(display));
                    draw_flag = true;

                    pc += 2;
                    break;

                case 0x00EE:
                    if (sp > 0) {
                        sp--;
                        pc = stack[sp];
                    }

                    pc += 2;
                    break;
            }

            break;

        case 1:
            addr = op & 0x0FFF;
            pc = addr;

            break;

        case 2:
            addr = op & 0x0FFF;
            if (sp < 16) {
                stack[sp] = pc;
                sp++;
            }

            pc = addr;

            break;

        case 3:
            reg = (op & 0x0F00) >> 8;
            val = op & 0x00FF;

            if (v[reg] == val)
                pc += 2;
            
            pc += 2;
            break;

        case 4:
            reg = (op & 0x0F00) >> 8;
            val = op & 0x00FF;

            if (v[reg] != val)
                pc += 2;
            
            pc += 2;
            break;

        case 5:
            reg1 = (op & 0x0F00) >> 8;
            reg2 = (op & 0x00F0) >> 4;

            if (v[reg1] == v[reg2])
                pc += 2;
            
            pc += 2;
            break;

        case 6:
            reg = (op & 0x0F00) >> 8;
            val = op & 0x00FF;

            v[reg] = val;

            pc += 2;
            break;

        case 7:
            reg = (op & 0x0F00) >> 8;
            val = op & 0x00FF;

            v[reg] += val;

            pc += 2;
            break;

        case 8:
            op_subcode = op & 0x000F; // last 4 bits
            switch (op_subcode) {
                case 0:
                    reg1 = (op & 0x0F00) >> 8;
                    reg2 = (op & 0x00F0) >> 4;

                    v[reg1] = v[reg2];

                    pc += 2;
                    break;

                case 1:
                    reg1 = (op & 0x0F00) >> 8;
                    reg2 = (op & 0x00F0) >> 4;

                    v[reg1] = v[reg1] | v[reg2];
                    v[0xF] = 0;

                    pc += 2;
                    break;

                case 2:
                    reg1 = (op & 0x0F00) >> 8;
                    reg2 = (op & 0x00F0) >> 4;

                    v[reg1] = v[reg1] & v[reg2];
                    v[0xF] = 0;

                    pc += 2;
                    break;

                case 3:
                    reg1 = (op & 0x0F00) >> 8;
                    reg2 = (op & 0x00F0) >> 4;

                    v[reg1] = v[reg1] ^ v[reg2];
                    v[0xF] = 0;

                    pc += 2;
                    break;

                case 4: {
                    reg1 = (op & 0x0F00) >> 8;
                    reg2 = (op & 0x00F0) >> 4;

                    if (v[reg1] + v[reg2] > 0xFF)
                        v[0xF] = 1;
                    else
                        v[0xF] = 0;

                    v[reg1] += v[reg2];
                    v[reg1] = (uint8_t)v[reg1];

                    pc += 2;
                    break;
                }

                case 5:
                    reg1 = (op & 0x0F00) >> 8;
                    reg2 = (op & 0x00F0) >> 4;

                    if (v[reg1] < v[reg2])
                        v[0xF] = 0;
                    else
                        v[0xF] = 1;

                    v[reg1] = (uint8_t)(v[reg1] - v[reg2]);

                    pc += 2;
                    break;

                case 6:
                    reg = (op & 0x0F00) >> 8;

                    v[0xF] = v[reg] & 0x01; 
                    v[reg] >>= 1;

                    pc += 2;
                    break;

                case 7:
                    reg1 = (op & 0x0F00) >> 8;
                    reg2 = (op & 0x00F0) >> 4;

                    if (v[reg1] > v[reg2])
                        v[0xF] = 0;
                    else
                        v[0xF] = 1;
                    
                    v[reg1] = (uint8_t)(v[reg2] - v[reg1]);

                    pc += 2;
                    break;

                case 14:
                    reg = (op & 0x0F00) >> 8;

                    v[0xF] = (uint8_t)(v[reg] >> 7);
                    v[reg] <<= 1;

                    pc += 2;
                    break;
            }

            break;

        case 9:
            reg1 = (op & 0x0F00) >> 8;
            reg2 = (op & 0x00F0) >> 4;

            if (v[reg1] != v[reg2])
                pc += 2;

            pc += 2;
            break;

        case 10:
            addr = op & 0x0FFF;
            index = addr;

            pc += 2;
            break;
        
        case 11:
            addr = op & 0x0FFF;
            pc = addr + v[0];

            break;

        case 12:
            reg = (op & 0x0F00) >> 8;
            mask = op & 0x00FF;

            rnd = rand() % 0x100; // between 0 and 256
            rnd &= mask;

            v[reg] = rnd;

            pc += 2;
            break;

        case 13: {
            reg1 = (op & 0x0F00) >> 8; // register where X coordinate is stored
            reg2 = (op & 0x00F0) >> 4; // register where Y coorfinate is stored
            uint8_t height = op & 0x000F; // N
            uint8_t width = 8; // every sprite is 8 pixels wide

            // read coordinates
            int x = v[reg1];
            int y = v[reg2];

            v[0xF] = 0;  // Reset collision flag

            for (int i = 0; i < height; i++) {
                uint8_t pixel = memory[index + i];

                for (int j = 0; j < width; j++) {
                    if ((pixel & (0x80 >> j)) != 0) {
                        // calculate coorinates
                        int xpos = (x + j) % 64;
                        int ypos = (y + i) % 32;

                        // if pixel was changed from set to unset, set collision flag
                        if (display[ypos][xpos] == 1)
                            v[0xF] = 1;

                        // xoring
                        display[ypos][xpos] ^= 1;
                    }
                }
            }

            // set draw flag
            draw_flag = true;
            pc += 2;
            break;
        }

        case 14:
            op_subcode = op & 0x00FF;
            switch (op_subcode) {
                case 0x9E:
                    reg = (op & 0x0F00) >> 8; // key

                    if (keyboard[v[reg]] == 1)
                        pc += 2;
                        
                    pc += 2;
                    break;

                case 0xA1:
                    reg = (op & 0x0F00) >> 8; // key
                    if (keyboard[v[reg]] == 0)
                        pc += 2;

                    pc += 2;
                    break;
            }

            break;

        case 15: {
            op_subcode = op & 0x00FF;
            switch (op_subcode) {
                case 0x07: {
                    reg = (op & 0x0F00) >> 8;
                    v[reg] = delay_timer;

                    pc += 2;
                    break;
                }

                case 0x0A: {
                    reg = (op & 0x0F00) >> 8;
                    bool key_pressed = false;

                    for (int i = 0; i < 16; i++) {
                        if (keyboard[i]) {
                            key_pressed = true;
                            v[reg] = (uint8_t)i;
                        }
                    }

                    if (key_pressed)
                        pc += 2;

                    break;
                }

                case 0x15: {
                    reg = (op & 0x0F00) >> 8;
                    delay_timer = v[reg];

                    pc += 2;
                    break;
                }

                case 0x18: {
                    reg = (op & 0x0F00) >> 8;
                    sound_timer = v[reg];

                    pc += 2;
                    break;
                }

                case 0x1E: {
                    reg = (op & 0x0F00) >> 8;

                    if (index + v[reg] > 0xFFF)
                        v[0xF] = 1;
                    else
                        v[0xF] = 0;

                    index = (uint16_t)(index + v[reg]);

                    pc += 2;
                    break;
                }

                case 0x29: {
                    reg = (op & 0x0F00) >> 8;
                    index = 0x050 + v[reg] * 0x5; // each char is 5 locations long

                    pc += 2;
                    break;
                }

                case 0x33: {
                    reg = (op & 0x0F00) >> 8;

                    // 255 -> memory[index] = 2, memory[index + 1] = 5, memory[index + 2] = 5
                    memory[index] = (uint8_t)(v[reg] / 100);
                    memory[index + 1] = (uint8_t)((uint8_t)(v[reg] / 10) % 10);
                    memory[index + 2] = (uint8_t)(v[reg] % 10);

                    pc += 2;
                    break;
                }

                case 0x55: {
                    reg = (op & 0x0F00) >> 8;

                    for (int i = 0; i <= reg; i++) {
                        memory[index + i] = v[i];
                    }

                    index = (uint16_t)(index + reg + 1);
                    
                    pc += 2;
                    break;
                }

                case 0x65: {
                    reg = (op & 0x0F00) >> 8;

                    for (int i = 0; i <= reg; i++)
                        v[i] = memory[index + i];

                    index = (uint16_t)(index + reg + 1);

                    pc += 2;
                    break;
                }
            }

            break;
        }
    }

    if (delay_timer > 0) delay_timer--;
    if (sound_timer > 0) sound_timer--;
}

void Chip8::emulate() {
    SDL_Init(SDL_INIT_VIDEO); // initializing SDL

    SDL_Window* window = SDL_CreateWindow("CHIP8", 640, 320, 0); // creating a window

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL); // createing a window renderer

    bool running = true;
    SDL_Event event;

    const int PIXEL_SIZE = 10; // upcale 1 pixel to 10 for bigger picture
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) // if close button pressed
                running = false;
        }

        single_cycle(); // emulate one cycle

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // draw pixels
        if (draw_flag) {
            for (int y = 0; y < 32; y++) {
                for (int x = 0; x < 64; x++) {
                    if (display[y][x] == 1)
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white
                    else
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black

                    SDL_FRect rect = { (float)(x * PIXEL_SIZE), (float)(y * PIXEL_SIZE), PIXEL_SIZE, PIXEL_SIZE }; // create rectangle
                    SDL_RenderFillRect(renderer, &rect); // draw rectangle
                }
            }

            draw_flag = false; // reset drawing flag
            SDL_RenderPresent(renderer);
        }

        SDL_Delay(2);
    }

    // Destroy all SDL components
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
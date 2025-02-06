# CHIP-8 Emulator

This is a CHIP-8 emulator written in C++ using SDL3 for graphics. CHIP-8 is an interpreted programming language used on early home computers, notably the COSMAC VIP and Telmac 1800. The emulator allows you to run CHIP-8 games and programs, emulating the original hardware functionality on modern systems.

## Features

- Emulates the full CHIP-8 instruction set.
- Loads and runs CHIP-8 ROM files.
- Simple graphics display with a 64x32 pixel resolution.
- Keyboard input mapping to simulate the CHIP-8 keypad.
- Sound has not yet been implemented

## Architecture

The CHIP-8 architecture consists of the following components:

- **Memory:** 4KB of RAM, from 0x000 to 0xFFF, which stores the program, data, and stack. 
- **Registers:** 16 general-purpose 8-bit registers (`V0` to `VF`), used for arithmetic and logical operations.
- **Stack:** Used to store return addresses during subroutine calls. The stack can hold up to 16 return addresses.
- **Timers:** 2 8-bit timers (`delay_timer` and `sound_timer`) that decrease at 60Hz.
- **Graphics:** 64x32 pixel display for rendering the graphics. The display is monochrome (black/white).
- **Keypad:** 16 keys that simulate user input (mapped to your keyboard).

## CHIP-8 Instruction Set

Here’s the complete CHIP-8 instruction set:

| Opcode  | Instruction                                | Description |
| ------- | ------------------------------------------ | ----------- |
| `00E0`  | `CLS` (Clear the screen)                  | Clears the screen. |
| `00EE`  | `RET` (Return from subroutine)            | Returns from a subroutine by popping the top of the stack into the program counter. |
| `1NNN`  | `JP NNN` (Jump to address NNN)            | Jumps to the address `NNN`. |
| `2NNN`  | `CALL NNN` (Call subroutine at NNN)       | Calls the subroutine at address `NNN`, pushing the return address onto the stack. |
| `3XNN`  | `SE VX, NN` (Skip if VX == NN)            | Skips the next instruction if register `Vx` is equal to `NN`. |
| `4XNN`  | `SNE VX, NN` (Skip if VX != NN)           | Skips the next instruction if register `Vx` is not equal to `NN`. |
| `5XY0`  | `SE VX, VY` (Skip if VX == VY)            | Skips the next instruction if register `Vx` equals register `Vy`. |
| `6XNN`  | `LD VX, NN` (Set VX to NN)                | Sets register `Vx` to the value `NN`. |
| `7XNN`  | `ADD VX, NN` (Add NN to VX)               | Adds the value `NN` to register `Vx`. |
| `8XY0`  | `LD VX, VY` (Set VX to VY)                | Sets register `Vx` to the value of register `Vy`. |
| `8XY1`  | `OR VX, VY` (VX = VX OR VY)               | Performs a bitwise OR on registers `Vx` and `Vy`, storing the result in `Vx`. |
| `8XY2`  | `AND VX, VY` (VX = VX AND VY)             | Performs a bitwise AND on registers `Vx` and `Vy`, storing the result in `Vx`. |
| `8XY3`  | `XOR VX, VY` (VX = VX XOR VY)             | Performs a bitwise XOR on registers `Vx` and `Vy`, storing the result in `Vx`. |
| `8XY4`  | `ADD VX, VY` (VX = VX + VY, VF = carry)   | Adds the value of `Vy` to `Vx` and sets the `VF` register to 1 if there is a carry, otherwise 0. |
| `8XY5`  | `SUB VX, VY` (VX = VX - VY, VF = borrow)  | Subtracts `Vy` from `Vx` and sets `VF` to 0 if there's a borrow, otherwise 1. |
| `8XY6`  | `SHR VX {, VY}` (VX = VX >> 1)            | Shifts `Vx` right by one bit, storing the result in `Vx`. |
| `8XY7`  | `SUBN VX, VY` (VX = VY - VX, VF = borrow) | Subtracts `Vx` from `Vy`, and sets `VF` to 0 if there's a borrow, otherwise 1. |
| `8XYE`  | `SHL VX {, VY}` (VX = VX << 1)            | Shifts `Vx` left by one bit, storing the result in `Vx`. |
| `9XY0`  | `SNE VX, VY` (Skip if VX != VY)           | Skips the next instruction if `Vx` is not equal to `Vy`. |
| `ANNN`  | `LD I, NNN` (Set I to address NNN)        | Sets the `I` register to the address `NNN`. |
| `BNNN`  | `JP V0, NNN` (Jump to NNN + V0)           | Jumps to the address `NNN` plus the value in `V0`. |
| `CXNN`  | `RND VX, NN` (VX = random AND NN)         | Sets `Vx` to a random number AND `NN`. |
| `DXYN`  | `DRW VX, VY, N` (Draw sprite)             | Draws a sprite at position `(Vx, Vy)` with a width of `N` pixels, and stores the sprite in memory at `I`. |
| `EX9E`  | `SKP VX` (Skip if key pressed)            | Skips the next instruction if the key in register `Vx` is pressed. |
| `EXA1`  | `SKNP VX` (Skip if key not pressed)       | Skips the next instruction if the key in register `Vx` is not pressed. |
| `FX07`  | `LD VX, DT` (Set VX to delay timer)      | Sets `Vx` to the current value of the delay timer. |
| `FX0A`  | `LD VX, K` (Wait for key press)           | Waits for a key to be pressed and stores the value in `Vx`. |
| `FX15`  | `LD DT, VX` (Set delay timer to VX)      | Sets the delay timer to the value in register `Vx`. |
| `FX18`  | `LD ST, VX` (Set sound timer to VX)      | Sets the sound timer to the value in register `Vx`. |
| `FX1E`  | `ADD I, VX` (I = I + VX)                  | Adds the value of register `Vx` to the `I` register. |
| `FX29`  | `LD F, VX` (Set I to sprite location)    | Sets `I` to the location of the sprite for the character in `Vx`. |
| `FX33`  | `LD B, VX` (Store BCD of VX at I)        | Stores the binary-coded decimal representation of `Vx` at memory locations `I`, `I+1`, and `I+2`. |
| `FX55`  | `LD [I], VX` (Store registers in memory)  | Stores the values of registers `V0` through `Vx` at memory starting from address `I`. |
| `FX65`  | `LD VX, [I]` (Read registers from memory) | Reads values into registers `V0` through `Vx` from memory starting at address `I`. |

## Controls

- Use the following keys to simulate the CHIP-8 keypad:
  - 1, 2, 3, 4 for `1, 2, 3, C` on the CHIP-8 keypad.
  - Q, W, E, R for `4, 5, 6, D`.
  - A, S, D, F for `7, 8, 9, E`.
  - Z, X, C, V for `A, 0, B, F`.

### Prerequisites

- Make and g++ compiler for C++
- A CHIP-8 ROM file to test the emulator (you can find couple of ROMS in /roms folder).
    - if you download more roms, just put them in roms folder 

## Getting Started

To run the CHIP-8 emulator, do this:
```
make
./chip8 name_of_rom_from_roms_folder_without_ch8
```
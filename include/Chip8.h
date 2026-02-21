#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
#include <random>
#include "Stack.h"

constexpr int MEMORY_SIZE { 4096 };
constexpr int FONTSET_START_ADDRESS = 0x50;
constexpr int FONT_SIZE = 80;
constexpr int STACK_SIZE = 16;
constexpr int REG_SIZE = 16;
constexpr int PROGRAM_START = 0x200;
constexpr int SCREEN_WIDTH = 64;
constexpr int SCREEN_HEIGHT = 32;


extern const uint8_t font[FONT_SIZE];


class Chip8 {
private:
    std::mt19937 randGen;
    std::uniform_int_distribution<uint16_t> randByte;
    uint16_t PC;
    uint8_t RAM[MEMORY_SIZE];
    Stack cpuStack;
    uint16_t I;
    uint8_t variableReg[REG_SIZE];

    const bool ambgInstr = true; // Follows the COSMAC VIP interpreter
    
    uint16_t fetch();

    void execute(uint16_t opcode);

public:
    uint8_t delayTimer;
    uint8_t soundTimer;
    bool keypad[16];
    uint8_t gfx[SCREEN_WIDTH * SCREEN_HEIGHT];
    bool waitingForKey = false;
    uint8_t keyRegister = 0;
    bool drawFlag = true;

    Chip8();

    void reset();


    bool loadGame(const char* filename);

    void cycle();
    void updateTimers();

    void provideKeyInput(uint8_t key);
};


#endif
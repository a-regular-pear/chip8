#include "Chip8.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include <vector>

extern const uint8_t font[FONT_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


Chip8::Chip8() {
    reset();
}

void Chip8::reset() {
    randGen.seed(std::random_device{}());
    randByte = std::uniform_int_distribution<uint16_t>(0, 255); 
    PC = PROGRAM_START;
    I = 0;
    delayTimer = 0;
    soundTimer = 0;
    drawFlag = true;
    waitingForKey = false;
    keyRegister = 0;
    std::fill(std::begin(RAM), std::end(RAM), 0);
    std::fill(std::begin(variableReg), std::end(variableReg), 0);
    std::fill(std::begin(keypad), std::end(keypad), false);
    std::fill(std::begin(gfx), std::end(gfx), 0);
    
    cpuStack = Stack{};
    
    for (int i = 0; i < FONT_SIZE; i++) {
        RAM[FONTSET_START_ADDRESS + i] = font[i];
    }
}

bool Chip8::loadGame(const char* filename)
{
    // Open stream to end of file to get size immediately
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (file.is_open())
    {
        // Get size of file and allocate a buffer to hold it
        std::streampos size = file.tellg();
        // Prevent loading past available RAM (4096 - 512 = 3584 bytes)
        long loadSize = (size > 3584) ? 3584 : (long)size;
        std::vector<char> buffer(loadSize);

        // Go back to the beginning of the file and read it
        file.seekg(0, std::ios::beg);
        file.read(buffer.data(), loadSize);
        file.close();

        // Load the buffer into CHIP-8 memory starting at 0x200
        for (long i = 0; i < loadSize; i++)
        {
            // Safety: Don't write past the end of RAM (usually 4096 bytes)
            if((0x200 + i) < 4096) 
            {
                RAM[0x200 + i] = (uint8_t)buffer[i];
            }
        }

        // Clean up dynamic buffer
        std::cout << "ROM loaded successfully! Size: " << loadSize << " bytes\n";
        return true;
    }
    
    std::cerr << "Failed to open ROM: " << filename << '\n';
    return false;
}

void Chip8::cycle() {
    if (!waitingForKey) {
        uint16_t op = fetch();
        execute(op);
    }
}

void Chip8::updateTimers() {
    if (delayTimer > 0) delayTimer--;
    if (soundTimer > 0) soundTimer--;
}

void Chip8::provideKeyInput(uint8_t key) {
    if (waitingForKey) {
        variableReg[keyRegister] = key;
        waitingForKey = false;
    }
}

uint16_t Chip8::fetch(){
        uint16_t opcode = (RAM[PC] << 8) | RAM[PC+1];
        PC += 2;
        return opcode;
}

void Chip8::execute(uint16_t opcode) {
    uint16_t nibble = opcode >> 12;
    uint16_t X = (opcode & 0x0F00) >> 8;
    uint16_t Y = (opcode & 0x00F0) >> 4;
    uint16_t N = (opcode & 0x000F);
    uint16_t NN = (opcode & 0x00FF);
    uint16_t NNN = (opcode & 0x0FFF);
    switch (nibble)
    {
    case 0x0:
        switch (NN)
        {
        case 0xE0:
            std::fill(std::begin(gfx), std::end(gfx), 0);
            drawFlag = true;
            break;
        case 0xEE:
            PC = cpuStack.pop();
            break;
        default:
            break;
        }
        break;
    
    case 0x1:
        PC = NNN;
        break;

    case 0x2:
        cpuStack.push(PC);
        PC = NNN;
        break;

    case 0x3:
        if(variableReg[X] == NN)
            PC += 2;
        break;
    case 0x4:
        if(variableReg[X] != NN)
            PC += 2;
        break;
    case 0x5:
        if(variableReg[X] == variableReg[Y]) 
            PC += 2;
        break;
    case 0x6:
        variableReg[X] = NN;
        break;
    case 0x7:
        variableReg[X] += NN;
        break;
    case 0x8:
        switch (N)
        {
        case 0:
            variableReg[X] = variableReg[Y];
            break;
        case 1:
            variableReg[X] |= variableReg[Y];
            break;
        case 2:
            variableReg[X] &= variableReg[Y];
            break;
        case 3:
            variableReg[X] ^= variableReg[Y];
            break;
        case 4: {
            uint16_t sum = variableReg[X] + variableReg[Y];
            variableReg[X] = sum & 0xFF;
            variableReg[0xF] = (sum > 255) ? 1 : 0;
            break;
        }
        case 5:
            (variableReg[X] >= variableReg[Y]) ? variableReg[0xF] = 1 : variableReg[0xF] = 0;
            variableReg[X] -= variableReg[Y];
            break;
        case 6:
            if(ambgInstr)
                variableReg[X] = variableReg[Y];
            variableReg[0xF] =variableReg[X] & 1;
            variableReg[X] >>= 1;
            break;
        case 7:
            (variableReg[Y] >= variableReg[X]) ? variableReg[0xF] = 1 : variableReg[0xF] = 0;
            variableReg[X] = variableReg[Y] - variableReg[X];
            break;
        case 0xE:
            if(ambgInstr)
                variableReg[X] = variableReg[Y];
            variableReg[0xF] = (variableReg[X] >> 7) & 1;
            variableReg[X] <<= 1;
            break;
        default:
            break;
        } 
        break;
        
    case 0x9:
        if(variableReg[X] != variableReg[Y]) 
            PC += 2;
        break;    
    case 0xA:
        I = NNN;
        break;
    case 0xB:
        if(ambgInstr) {
            PC = NNN + variableReg[0];
        } else {
            PC =  NNN + variableReg[X];
        }
        break;
    case 0xC:
        variableReg[X] = (randByte(randGen)) & NN;
        break;      
    case 0xD:
    {
        uint8_t xCord = variableReg[X] % 64;
        uint8_t yCord = variableReg[Y] % 32;
        variableReg[0xF] = 0;
        
        for(uint16_t i = 0; i < N; i++) {
            uint8_t drawY = (yCord + i);
            if (drawY >= 32) break; // Clip vertically
            uint8_t sprite = RAM[I + i];

            for(uint8_t j = 0; j < 8; j++) {
                uint8_t drawX = (xCord + j);
                if (drawX >= 64) break; // Clip horizontally
                
                bool bit = (sprite >> (7-j)) & 1;
                int pixelIndex = drawX + (drawY * SCREEN_WIDTH);
                
                if(bit == 1) {
                    if(gfx[pixelIndex] == 1) {
                        variableReg[0xF] = 1; // Collision
                    }
                    gfx[pixelIndex] ^= 1;

                }
            }
        }
        drawFlag = true;
        break;
    }
    case 0xE:
        switch (NN)
        {
        case 0x9E:
            if(keypad[variableReg[X]] == true) 
                PC += 2;
            
            break;
        case 0xA1:
            if(keypad[variableReg[X]] != true) 
                PC += 2;
            
            break;
        default:
            break;
        }
        break;
    case 0xF:
        switch (NN)
        {
        case 0x07:
            variableReg[X] = delayTimer;
            break;
        case 0x15:
            delayTimer = variableReg[X];
            break;
        case 0x18:
            soundTimer = variableReg[X];
            break;
        case 0x1E:
            I += variableReg[X];
            break;
        case 0x0A:
            waitingForKey = true;
            break;
        case 0x29:
            I = 0x50 + (variableReg[X] * 5);
            break;
        case 0x33:
            RAM[I] = (variableReg[X] / 100)  % 10;
            RAM[I+1] = (variableReg[X] / 10)  % 10;
            RAM[I+2] = (variableReg[X])  % 10;
            break;
        case 0x55:
            for(uint16_t i = 0; i <= X; i++) {
                RAM[I+i] = variableReg[i];
            }
            if (ambgInstr) I += X + 1;
            break;
        case 0x65:
            for(uint16_t i = 0; i <= X; i++) {
                variableReg[i] = RAM[I+i];
            }
            if (ambgInstr) I += X + 1;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}
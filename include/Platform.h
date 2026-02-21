#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>
#include "Chip8.h"

// Audio configuration
const int SAMPLE_RATE = 44100;
const int AMPLITUDE = 3000;
const int TONE_HZ = 440; // Frequency of the beep
const uint32_t tick = 1000/60;
constexpr int SCALE = 20;
constexpr uint32_t OFF = 0xFF000000;
constexpr uint32_t ON = 0xFFFFFF;

class Platform {

    SDL_Window* gWindow = nullptr;
    SDL_Renderer* gRenderer = nullptr;
    SDL_Texture* gTexture = nullptr;
    // SDL Audio Callback to generate a square wave
    static void audioCallback(void* userdata, uint8_t* stream, int len);
    uint8_t getChip8Key(SDL_Keycode key);
public:
    bool init();

    void close();

    bool processInput(Chip8& chip8);
    void updateDisplay(const uint8_t* gfx);
    void setAudio(bool playing);
};

#endif
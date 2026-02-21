#include "Platform.h"

void Platform::audioCallback(void* /*userdata*/, uint8_t* stream, int len) {
    static uint32_t audio_sample_pos = 0;
    int16_t* audio_data = (int16_t*)stream;
    int samples = len / 2; // 16-bit audio = 2 bytes per sample

    for (int i = 0; i < samples; i++) {
        int half_period = SAMPLE_RATE / TONE_HZ / 2;
        if ((audio_sample_pos / half_period) % 2 == 0) {
            audio_data[i] = AMPLITUDE;
        } else {
            audio_data[i] = -AMPLITUDE;
        }
        audio_sample_pos++;
    }
}

uint8_t Platform::getChip8Key(SDL_Keycode key) {
    switch (key) {
    case SDLK_1: return 0x0;
    case SDLK_2: return 0x1;
    case SDLK_3: return 0x2;
    case SDLK_4: return 0xC;
    case SDLK_q: return 0x4;
    case SDLK_w: return 0x5;
    case SDLK_e: return 0x6;
    case SDLK_r: return 0xD;
    case SDLK_a: return 0x7;
    case SDLK_s: return 0x8;
    case SDLK_d: return 0x9;
    case SDLK_f: return 0xE;
    case SDLK_z: return 0xA;
    case SDLK_x: return 0x0;
    case SDLK_c: return 0xB;
    case SDLK_v: return 0xF;
    default: return 0xFF;
    }   
}

bool Platform::init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Create window
        gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCALE*SCREEN_WIDTH, SCALE*SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            gTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
            

            SDL_AudioSpec want, have;
            SDL_zero(want);
            want.freq = SAMPLE_RATE;
            want.format = AUDIO_S16SYS;
            want.channels = 1; // Mono
            want.samples = 2048;
            want.callback = audioCallback; // Attach the generator function

            if (SDL_OpenAudio(&want, &have) != 0) {
                printf("Failed to open audio: %s\n", SDL_GetError());
                success = false;
            }

        }
    }

    return success;
}

void Platform::close()
{
    SDL_DestroyTexture(gTexture);
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;
    gTexture = NULL;
    SDL_CloseAudio(); 
    SDL_Quit();
}

bool Platform::processInput(Chip8& chip8) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) return true;
        
        if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
            uint8_t key = getChip8Key(e.key.keysym.sym);
            if (key != 0xFF) {
                bool isPressed = (e.type == SDL_KEYDOWN);
                chip8.keypad[key] = isPressed;

                if (isPressed) {
                    chip8.provideKeyInput(key);
                }
            }
        }
    }
    return false;
}

void Platform::updateDisplay(const uint8_t* gfx) {
    uint32_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) {
        pixels[i] = gfx[i] ? ON : OFF;
    }
    SDL_UpdateTexture(gTexture, NULL, pixels, SCREEN_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(gRenderer);
    SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
    SDL_RenderPresent(gRenderer);
}

void Platform::setAudio(bool playing) {
    SDL_PauseAudio(playing ? 0 : 1);
}
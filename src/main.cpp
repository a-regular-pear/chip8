#include "Chip8.h"
#include "Platform.h"

int main( )
{

    Platform platform;
    Chip8 chip8;

    //Initialize SDL
    if( !platform.init() )
    {
        std::cerr << "Failed to initialize!\n";
        return -1;
    }
    
    
    if(!chip8.loadGame("ROMS/ROM"))
        return -2;

    bool quit = false;
    uint32_t lastTimerUpdate = SDL_GetTicks();
    while(!quit){
        quit = platform.processInput(chip8);
        uint32_t currentTime = SDL_GetTicks();

        if(currentTime - lastTimerUpdate > tick) {
            for(int k = 0; k < 11; k++) 
            {
                chip8.cycle();        
            }
            
            chip8.updateTimers();
            platform.setAudio(chip8.soundTimer > 0);
                
                // Hardware accelerated rendering block
            if (chip8.drawFlag) {
                platform.updateDisplay(chip8.gfx);
                chip8.drawFlag = false;
            }
            lastTimerUpdate = currentTime;
            
        }
        SDL_Delay(1);
    }
}
    


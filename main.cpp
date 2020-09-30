#include <iostream>
#include <chrono>
#include <thread>
#include "stdint.h"
#include "SDL2/SDL.h"


#include "fire8.hpp"

using namespace std;

uint8_t keymap[16] = {
    SDLK_x,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v,
};

int main(int argc, char **argv) {

    if (argc != 2) {
        cout << "Usage: ./fire8 <ROM file>" << endl;
        return 1;
    }

    Chip8 chip8 = Chip8();          // Initialise Chip8

    int w = 1024;                   // Window width
    int h = 512;                    // Window height

    SDL_Window* window = NULL;

    if ( SDL_Init(SDL_INIT_EVERYTHING) < 0 ) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        exit(1);
    }
    window = SDL_CreateWindow(
            "CHIP-8 Emulator",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            w, h, SDL_WINDOW_SHOWN
    );
    if (window == NULL){
        printf( "Window could not be created! SDL_Error: %s\n",
                SDL_GetError() );
        exit(2);
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, w, h);

    SDL_Texture* sdlTexture = SDL_CreateTexture(renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            64, 32);

    uint32_t pixels[2048];


    load:
    if (!chip8.load(argv[1]))
        return 2;

    while (true) {
        chip8.emulate_cycle();

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) exit(0);

            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                    exit(0);

                if (e.key.keysym.sym == SDLK_F1)
                    goto load;      // *gasp*, a goto statement!

                for (int i = 0; i < 16; ++i) {
                    if (e.key.keysym.sym == keymap[i]) {
                        chip8.key[i] = 1;
                    }
                }
            }
            if (e.type == SDL_KEYUP) {
                for (int i = 0; i < 16; ++i) {
                    if (e.key.keysym.sym == keymap[i]) {
                        chip8.key[i] = 0;
                    }
                }
            }
        }

        if (chip8.drawFlag) {
            chip8.drawFlag = false;

            for (int i = 0; i < 2048; ++i) {
                uint8_t pixel = chip8.gfx[i];
                pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
            }
            SDL_UpdateTexture(sdlTexture, NULL, pixels, 64 * sizeof(Uint32));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }

        std::this_thread::sleep_for(std::chrono::microseconds(1200));

    }
}

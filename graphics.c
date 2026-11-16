#include "graphics.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"
#include "chip8.h"
#include "stdio.h"
#include "string.h"
#include <SDL3/SDL_rect.h>

#define ESCALA 10
#define CICLOS_POR_CUADRO 10
#define MS_POR_CUADRO 16

static const SDL_Keycode teclas[16] = {
    SDLK_X, SDLK_1, SDLK_2, SDLK_3, SDLK_Q, SDLK_W, SDLK_E, SDLK_A,
    SDLK_S, SDLK_D, SDLK_Z, SDLK_C, SDLK_4, SDLK_R, SDLK_F, SDLK_V};

void drawGfx(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  for (int y = 0; y < 32; y++) {
    for (int x = 0; x < 64; x++) {
      if (gfx[y * 64 + x]) {
        SDL_FRect pixel = {(float)(x * ESCALA), (float)(y * ESCALA)};
        SDL_RenderFillRect(renderer, &pixel);
      }
    }
  }
  SDL_RenderPresent(renderer);
}

int graphics(void) {

  SDL_Window *window = SDL_CreateWindow("CHIP8", 640, 320, 0);

  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

  bool running = true;
  SDL_Event e;

  while (running) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_EVENT_QUIT) {
        running = false;
      }

      if (e.type == SDL_EVENT_KEY_DOWN || e.type == SDL_EVENT_KEY_UP) {
        bool presionada = (e.type == SDL_EVENT_KEY_DOWN);

        if (e.key.key == SDLK_ESCAPE) {
          running = false;
        }

        for (int i = 0; i < 16; i++) {
          if (e.key.key == teclas[i]) {
            key[i] = presionada;
          }
        }
      }
    }

    for (int i = 0; i < CICLOS_POR_CUADRO; i++) {
      emulateCycle();
    }
    timers();

    drawGfx(renderer);
    SDL_Delay(MS_POR_CUADRO);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();

  return 0;
}

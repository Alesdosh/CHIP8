
#include "graphics.h"
#include "utils.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <stdio.h>
#include <string.h>

#define SDLK_Q 0x00000071u
#define SDLK_W 0x00000077u
#define SDLK_E 0x00000065u
#define SDLK_R 0x00000072u

#define SDLK_A 0x00000061u
#define SDLK_S 0x00000073u
#define SDLK_F 0x00000066u
#define SDLK_D 0x00000064u // sprites 4x5, por lo menos los numeros
#define SDLK_F 0x00000066u

#define SDLK_Z 0x0000007au
#define SDLK_X 0x00000078u
#define SDLK_C 0x00000063u
#define SDLK_V 0x00000076u

#define SDLK_1 0x00000031u
#define SDLK_2 0x00000032u
#define SDLK_3 0x00000033u
#define SDLK_4 0x00000034u

int drawSprite() { return 0; }

int drawNumbers(SDL_Renderer *renderer, int *codigos) {
  // pixeles 20x20

  int positionX = 40; // posicion inicial
  int positionY = 40;

  bool salto = false;

  for (int x = 0; x < 8; x++) {

    for (int y = 0; y < 4; y++) { // cambiar 4 por n cuando se quiera dibujar un
                                  // sprite y no un numero

      int bit = (codigos[x] >> (7 - y)) & 1;

      if (bit) {
        // DIBUJAR
        SDL_FRect square = {positionX, positionY, 20, 20};

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);

        SDL_RenderFillRect(renderer, &square);

        positionX += 20;

      } else {
        // SALTAR

        positionX += 20;
      }

      if (y == 3) { // al penultimo

        salto = true;

        // cambiar el binario
      }

      if (salto == true) {
        positionX = 40;
        positionY += 20;
        salto = false;
      }
    }
  }

  return 0;
}

int graphics(void) {

  int codigos[5];

  // Manejar error por si sdl falla en iniciar
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    printf("Error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow("CHIP8", 640, 320, 0);

  if (!window) {
    printf("Error creando ventana: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  bool running = true;

  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

  SDL_Event e;

  while (running) {

    while (SDL_PollEvent(&e)) {

      if (e.type == SDL_EVENT_QUIT) {
        running = false;
      }

      if (e.type == SDL_EVENT_KEY_DOWN) { // Se presiono una tecla?
        if (e.key.key == SDLK_1) {
          codigos[0] = 0x20;
          codigos[1] = 0x60;
          codigos[2] = 0x20;
          codigos[3] = 0x20;
          codigos[4] = 0x70;
        }
        if (e.key.key == SDLK_2) {
          codigos[0] = 0xF0;
          codigos[1] = 0x10;
          codigos[2] = 0xF0;
          codigos[3] = 0x80;
          codigos[4] = 0xF0;
        }
        if (e.key.key == SDLK_3) {
          codigos[0] = 0xF0;
          codigos[1] = 0x10;
          codigos[2] = 0xF0;
          codigos[3] = 0x10;
          codigos[4] = 0xF0;
        }
        if (e.key.key == SDLK_4) {
          codigos[0] = 0xF0;
          codigos[1] = 0x80;
          codigos[2] = 0x80;
          codigos[3] = 0x80;
          codigos[4] = 0xF0;
        }

        if (e.key.key == SDLK_Q) {
          codigos[0] = 0x90;
          codigos[1] = 0x90;
          codigos[2] = 0xF0;
          codigos[3] = 0x10;
          codigos[4] = 0x10;
        }
        if (e.key.key == SDLK_W) {
          codigos[0] = 0xF0;
          codigos[1] = 0x80;
          codigos[2] = 0xF0;
          codigos[3] = 0x10;
          codigos[4] = 0xF0;
        }
        if (e.key.key == SDLK_E) {
          codigos[0] = 0xF0;
          codigos[1] = 0x80;
          codigos[2] = 0xF0;
          codigos[3] = 0x90;
          codigos[4] = 0xF0;
        }
        if (e.key.key == SDLK_R) {
          codigos[0] = 0xE0;
          codigos[1] = 0x90;
          codigos[2] = 0x90;
          codigos[3] = 0x90;
          codigos[4] = 0xE0;
        }

        if (e.key.key == SDLK_A) {
          codigos[0] = 0xF0;
          codigos[1] = 0x10;
          codigos[2] = 0x20;
          codigos[3] = 0x40;
          codigos[4] = 0x40;
        }
        if (e.key.key == SDLK_S) {
          codigos[0] = 0xF0;
          codigos[1] = 0x90;
          codigos[2] = 0xF0;
          codigos[3] = 0x90;
          codigos[4] = 0xF0;
        }
        if (e.key.key == SDLK_D) {
          codigos[0] = 0xF0;
          codigos[1] = 0x90;
          codigos[2] = 0xF0;
          codigos[3] = 0x10;
          codigos[4] = 0xF0;
        }

        if (e.key.key == SDLK_F) {
          codigos[0] = 0xF0;
          codigos[1] = 0x80;
          codigos[2] = 0xF0;
          codigos[3] = 0x80;
          codigos[4] = 0xF0;
        }

        if (e.key.key == SDLK_Z) {
          codigos[0] = 0xF0;
          codigos[1] = 0x90;
          codigos[2] = 0xF0;
          codigos[3] = 0x90;
          codigos[4] = 0x90;
        }
        if (e.key.key == SDLK_X) {
          codigos[0] = 0xF0;
          codigos[1] = 0x90;
          codigos[2] = 0x90;
          codigos[3] = 0x90;
          codigos[4] = 0xF0;
        }
        if (e.key.key == SDLK_C) {
          codigos[0] = 0xE0;
          codigos[1] = 0x90;
          codigos[2] = 0xE0;
          codigos[3] = 0x90;
          codigos[4] = 0xE0;
        }
        if (e.key.key == SDLK_V) {
          codigos[0] = 0xF0;
          codigos[1] = 0x80;
          codigos[2] = 0xF0;
          codigos[3] = 0x80;
          codigos[4] = 0x80;
        }
      }

      SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

      SDL_RenderClear(renderer);

      drawNumbers(renderer, codigos);

      SDL_RenderPresent(renderer);
    }
  }

  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);

  SDL_Quit();

  return 0;
}

#include "chip8.h"
#include "graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(void) {

  initialize(); // inicialización de chip8
  loadGame();   // cargar juego
  graphics();   // graficos e input inicializados
}

#include "chip8.h"
#include "graphics.h"
#include <stdio.h>
#include <string.h>

int main(void) {

  initialize(); // inicialización de chip8
  loadGame();   // cargar juego
  graphics();   // graficos e input inicializados
}

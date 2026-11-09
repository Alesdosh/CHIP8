#include "chip8.h"
#include "graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(void) {

  initialize(); // inicialización de chip8
  graphics();   // graficos e input inicializados

  srand((unsigned)time(
      NULL)); // inicializador de la semilla para numeros aleatorios

  bool correr = true;
  while (correr) { // Proceso de ejecución
  }
}

#include "utils.h"
#include <stdio.h>
#include <string.h>

char *hexa_a_bin(int hex) { // 0xF0

  static char binaryString[33];
  int index = 0;
  int index2 = 0;

  static char string2[33];

  sprintf(binaryString, "%032b", hex);

  // eliminar ceros que van antes del 1
  // recorrer el arreglo y encontrar el primer 1

  for (int e = 0; e < strlen(binaryString); e++) {
    // encontrar el index del primer 1

    if (binaryString[e] == '1') {
      index = e; // encontrado

      break;
    }
  }

  for (int q = 0; q < strlen(binaryString); q++) {

    if (q < index) {
      // saltar el 0

    } else {
      // agregar el 1 al nuevo string
      if (strlen(string2) > 4) {
        break;
      } else {

        string2[index2] = binaryString[q];
        index2++;
      }
    }
  }

  return string2;
}

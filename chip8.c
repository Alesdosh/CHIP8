#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

unsigned char chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

unsigned short opcode;      // almacenamiento de opcodes, 2 bytes de longitud
unsigned char memory[4096]; // definicion de la memoria, 4kb para el chip8
unsigned char V[16]; // definicion de los registros disponibles, van de V0 a VF
unsigned short I;    // registro index
unsigned short pc;   // program counter posicion donde se encuentra el programa
                     // en la lista de instrucciones
unsigned char delay_timer;
unsigned char sound_timer;
unsigned short stack[16]; // PILA
unsigned short sp;        // Puntero de la posicion en la PILA
unsigned short key[16];
unsigned char gfx[SCREEN_WIDTH * SCREEN_HEIGHT];

void initialize(void) {

  // teclas disponibles para presionarse

  pc = 0x200;
  opcode = 0;
  I = 0;
  sp = 0;
  delay_timer = 0;
  sound_timer = 0;

  memset(gfx, 0, sizeof gfx);
  memset(stack, 0, sizeof stack);
  memset(V, 0, sizeof V);
  memset(key, 0, sizeof key);
  memset(memory, 0, sizeof memory);

  for (int i = 0; i < 80;
       i++) { // cargar el fontset en memoria, cada caracter ocupa 5 bytes, es
    memory[i] =
        chip8_fontset[i]; // por eso que se carga el fontset hasta 80 bytes,
                          // porque todo el fontset inicial es de 16 caracteres
  }

  srand((unsigned)time(NULL));
}

void timers(void) { // esta función solo se encargará de restar en 1 a los timers si
                    // son mayores a 0
  if (delay_timer > 0) {
    delay_timer--;
  }
  if (sound_timer > 0) {
    sound_timer--;
  }
}

void loadGame() { // cargar el juego
  long bufferSize;

  FILE *file =
      fopen("YusepeCombat_1.1v.ch8",
            "rb"); // rb para abrir en modo lectura binaria
                   // fseek para mover el puntero al final del archivo vaya
  if (!file) {
    fprintf(stderr, "No se pudo abrir YusepeCombat_1.1v.ch8\n");
    exit(1);
  }

  fseek(file, 0, SEEK_END);
  // ftell para obtenerla posicion actual que equivale al tamaño total en bytes
  // del archivo

  bufferSize = ftell(file);

  if (bufferSize <= 0 || bufferSize > 4096 - 0x200) {
    fprintf(stderr, "La ROM no cabe en la memoria del CHIP-8\n");
    fclose(file);
    exit(1);
  }

  rewind(file);
  fread(memory + 0x200, 1, bufferSize, file);
  fclose(file);
}

void emulateCycle() {
  // OBTENER, DECODIFICAR, EJECUTAR
  // es la serie de pasos que seguira esta funcion

  opcode = memory[pc] << 8 | memory[pc + 1];

  switch (opcode & 0xF000) {
    // leer los primeros 4 bits osea la primera letra del
    // opcode, para saber que opcode es el que se va a ejecutar
    // opcodes que se pueden deducir con los primeros 4 bits

  case 0x0000: // opcodes que se deduciran con los ultimos 4 bits osea la ultima
               // letra
    switch (opcode) {
    case 0x00E0: // LIMPIAR PANTALLA
      memset(gfx, 0, sizeof gfx); // se pone al arreglo de la pantalla en ceros
      pc += 2;
      break;

    case 0x00EE:
      sp--;
      pc = stack[sp] + 2;
      break;

    default:
      pc += 2;
      break;
    }
    break;

  case 0xA000:
    I = opcode & 0x0FFF;
    pc += 2;
    break;

  case 0xB000: {
    int nnn = opcode & 0x0FFF;

    pc = (V[0] + nnn) & 0xFFF;
    break;
  }

  case 0x1000: //  salta a la posicion NNN

    pc = opcode & 0x0FFF;

    break;

  case 0x2000: // llama a la subrutina NNN
    stack[sp] = pc;
    sp++;
    pc = opcode & 0x0FFF;
    break;

  case 0x3000: {
    int x = (opcode & 0x0F00) >> 8; // solo obtener x
    int nn = opcode & 0x00FF;       // solo obtener los ultimos dos

    pc += (V[x] == nn) ? 4 : 2;
    break;
  }

  case 0x4000: {
    int x = (opcode & 0x0F00) >> 8; // solo obtener x
    int kk = opcode & 0x00FF;       // solo obtener los ultimos dos

    pc += (V[x] != kk) ? 4 : 2;
    break;
  }

  case 0x5000: {
    int x = (opcode & 0x0F00) >> 8;
    int y = (opcode & 0x00F0) >> 4;
    int n = opcode & 0x000F;

    pc += (n == 0 && V[x] == V[y]) ? 4 : 2;
    break;
  }

  case 0x6000: {
    // FX55 Almacena el contenido de V0 a VX en la memoria empezando por la
    // dirección I
    int x = (opcode & 0x0F00) >> 8;
    int kk = opcode & 0x00FF;

    V[x] = kk;
    pc += 2;
    break;
  }

  case 0x7000: {
    int x = (opcode & 0x0F00) >> 8;
    int kk = opcode & 0x00FF;

    V[x] += kk;
    pc += 2;
    break;
  }

  case 0x8000: {
    int x = (opcode & 0x0F00) >> 8;
    int y = (opcode & 0x00F0) >> 4;

    switch (opcode & 0x000F) {
    case 0x0000:
      V[x] = V[y];
      break;

    case 0x0001: // bitwise OR
      V[x] = V[x] | V[y];
      break;

    case 0x0002: // bitwise AND
      V[x] = V[x] & V[y];
      break;

    case 0x0003:
      V[x] = V[x] ^ V[y];
      break;

    case 0x0004: {
      int suma = V[x] + V[y];
      V[x] = suma & 0xFF;
      V[0xF] = suma > 0xFF;
      break;
    }

    case 0x0005: {
      int noPrestamo = V[x] >= V[y];
      V[x] = V[x] - V[y];
      V[0xF] = noPrestamo;
      break;
    }

    case 0x0006: {
      int lsb = V[y] & 0x1;
      V[x] = V[y] >> 1;
      V[0xF] = lsb;
      break;
    }

    case 0x0007: {
      int noPrestamo = V[y] >= V[x];
      V[x] = V[y] - V[x];
      V[0xF] = noPrestamo;
      break;
    }

    case 0x000E: {
      int msb = (V[y] >> 7) & 0x1;
      V[x] = V[y] << 1;
      V[0xF] = msb;
      break;
    }

    default:
      break;
    }
    pc += 2;
    break;
  }

  case 0x9000: {
    int n = opcode & 0x000F;
    int x = (opcode & 0x0F00) >> 8;
    int y = (opcode & 0x00F0) >> 4;

    pc += (n == 0 && V[x] != V[y]) ? 4 : 2;
    break;
  }

  case 0xC000: {
    int x = (opcode & 0x0F00) >> 8;
    int kk = opcode & 0x00FF;

    V[x] = rand() & kk;
    pc += 2;
    break;
  }

  case 0xD000: {
    int x = (opcode & 0x0F00) >> 8;
    int y = (opcode & 0x00F0) >> 4;
    int n = opcode & 0x000F;

    int px0 = V[x] % SCREEN_WIDTH;
    int py0 = V[y] % SCREEN_HEIGHT;
    V[0xF] = 0;

    for (int fila = 0; fila < n; fila++) {
      unsigned char sprite = memory[(I + fila) & 0xFFF];

      for (int col = 0; col < 8; col++) {
        if (sprite & (0x80 >> col)) {
          int px = px0 + col;
          int py = py0 + fila;

          if (px >= SCREEN_WIDTH || py >= SCREEN_HEIGHT) {
            continue;
          }

          int idx = py * SCREEN_WIDTH + px;
          if (gfx[idx]) {
            V[0xF] = 1;
          }
          gfx[idx] ^= 1;
        }
      }
    }
    pc += 2;
    break;
  }

  case 0xE000: {
    int x = (opcode & 0x0F00) >> 8;

    switch (opcode & 0x00FF) {
    case 0x9E:
      // Si valor de V[x] coincide con la tecla presionada, se salta a la
      // siguiente instruccio
      pc += key[V[x] & 0xF] ? 4 : 2;
      break;

    case 0xA1:
      pc += key[V[x] & 0xF] ? 2 : 4;
      break;

    default:
      pc += 2;
      break;
    }
    break;
  }

  case 0xF000: {
    int x = (opcode & 0x0F00) >> 8;

    switch (opcode & 0x00FF) {
    case 0x07:
      V[x] = delay_timer;
      pc += 2;
      break;

    case 0x0A: { // Espera por una tecla presionada y la almacena en el registro
      int tecla = -1;
      for (int k = 0; k < 16; k++) {
        if (key[k]) {
          tecla = k;
          break;
        }
      }
      if (tecla >= 0) {
        V[x] = tecla;
        pc += 2;
      }
      break;
    }

    case 0x15: // Establecer delay timer a la posicion Vx
      delay_timer = V[x];
      pc += 2;
      break;

    case 0x18:
      sound_timer = V[x];
      pc += 2;
      break;

    case 0x1E:
      I = (I + V[x]) & 0xFFF;
      pc += 2;
      break;

    case 0x29:
      I = (V[x] & 0xF) * 5;
      pc += 2;
      break;

    case 0x33:
      memory[I & 0xFFF] = V[x] / 100;
      memory[(I + 1) & 0xFFF] = (V[x] / 10) % 10;
      memory[(I + 2) & 0xFFF] = V[x] % 10;
      pc += 2;
      break;

    case 0x55:
      for (int i = 0; i <= x; i++) {
        memory[(I + i) & 0xFFF] = V[i];
      }
      I = (I + x + 1) & 0xFFF;
      pc += 2;
      break;

    case 0x65:
      for (int i = 0; i <= x; i++) {
        V[i] = memory[(I + i) & 0xFFF];
      }
      I = (I + x + 1) & 0xFFF;
      pc += 2;
      break;

    default:
      pc += 2;
      break;
    }
    break;
  }

  default:
    pc += 2;
    break;
  }
}

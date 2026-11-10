#include "chip8.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
unsigned char gfx[64 * 32];

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

void timers() { // esta función solo se encargará de restar en 1 a los timers si
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
      fopen("tetris.ch8",
            "rb"); // rb para abrir en modo lectura binaria
                   // fseek para mover el puntero al final del archivo vaya

  fseek(file, 0, SEEK_END);
  // ftell para obtenerla posicion actual que equivale al tamaño total en bytes
  // del archivo

  bufferSize = ftell(file);

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

  case 0xA000:
    I = opcode & 0x0FFF;
    pc += 2;
    break;

  case 0xB000: {
    int nnn = opcode & 0x0FFF;

    pc = V[0] + nnn;
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

    pc += 2;
    if (V[x] == nn) {
      pc += 2;
    }
    break;
  }

  case 0x4000: {
    int x = (opcode & 0x0F00) >> 8; // solo obtener x
    int kk = opcode & 0x00FF;       // solo obtener los ultimos dos

    pc += 2;
    if (V[x] != kk) {
      pc += 2;
    }
    break;
  }

  case 0x5000: {
    int x = (opcode & 0x0F00) >> 8;
    int y = (opcode & 0x00F0) >> 4;
    int n = opcode & 0x000F;

    pc += 2;
    if (n == 0 & V[x] == V[y]) {
      pc += 2;
    }

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
    switch (opcode & 0x000F) {
    case 0x0000: {
      int x = (opcode & 0x0F00) >> 8;
      int y = (opcode & 0x00F0) >> 4;
      V[x] = V[y];
      pc += 2;
      break;
    }
    case 0x0001: { // bitwise OR
      int x = (opcode & 0x0F00) >> 8;
      int y = (opcode & 0x00F0) >> 4;
      V[x] = V[x] | V[y];
      pc += 2;

      break;
    }
    case 0x0002: { // bitwise AND
      int x = (opcode & 0x0F00) >> 8;
      int y = (opcode & 0x00F0) >> 4;
      V[x] = V[x] & V[y];
      pc += 2;
      break;
    }
    case 0x0003: {
      pc += 2;
      break;
    }
    case 0x0004: {
      pc += 2;
      break;
    }
    case 0x0005: {
      pc += 2;
      break;
    }
    case 0x0006: {
      pc += 2;
      break;
    }
    case 0x0007: {
      pc += 2;
      break;
    }
    case 0x000E: {
      pc += 2;
      break;
    }
    }
    break;
  }

  case 0x9000: {
    int n = opcode & 0x000F;
    int x = (opcode & 0x0F00) >> 8;
    int y = (opcode & 0x00F0) >> 4;
    pc += 2;
    if (n == 0) {
      if (V[x] != V[y]) {
        pc += 2;
      }
    }

    break;
  }

  case 0xE000: {
    switch (opcode & 0x000F) {
    case 0x000E: {
      int n = (opcode & 0x00F0) >> 4;
      int x = (opcode & 0x0F00) >> 8;
      pc += 2;
      if (n == 9) {
        // Si valor de V[x] coincide con la tecla presionada, se salta a la
        // siguiente instruccio
      }
      break;
    }

    case 0x0001: {
      int n = (opcode & 0x00F0) >> 4;
      int x = (opcode & 0x0F00) >> 8;

      pc += 2;
      if (n == 0xA) {
        // Espera por una tecla presionada y la almacena en el registro
      }

      break;
    }
    }
    break;
  }

  case 0xF000: {
    switch (opcode & 0x000F) {
    case 0x0007: {
      int x = (opcode & 0x0F00) >> 8;
      V[x] = delay_timer;
      pc += 2;
      break;
    }
    case 0x000A: {

      break;
    }
    case 0x000E: {
      int x = (opcode & 0x0F00) >> 8;
      I = I + V[x];
      pc += 2;
      break;
    }
    case 0x0005: {
      switch ((opcode & 0x00F0) >> 4) {

      case 0x0010: { // Establecer delay timer a la posicion Vx
        int x = (opcode & 0x0F00) >> 8;
        delay_timer = V[x];
        pc += 2;
        break;
      }
      case 0x0050: {
        pc += 2;
        break;
      }
      case 0x0060: {
        pc += 2;
        break;
      }
      }
      break;
    }
    case 0x0008: {
      int x = (opcode & 0x0F00) >> 8;
      sound_timer = V[x];
      pc += 2;
      break;
    }

    case 0x0009: {
      pc += 2;
      break;
    }
    case 0x0003: {
      pc += 2;
      break;
    } break;
    }

  case 0x0000: // opcodes que se deduciran con los ultimos 4 bits osea la ultima
               // letra
    switch (opcode & 0x000F) {

    case 0x0000:
      pc = opcode & 0x0FFF;
      break;
    }

  case 0x000E: {
    // LIMPIAR PANTALLA
    pc += 2;
    break;
  }

  break;
  }
  }
}

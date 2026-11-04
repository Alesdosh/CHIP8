#ifndef CHIP8_H
#define CHIP8_H

void initialize(void);
void loadGame(void);
void emulateCycle(void);
void timers(void);

extern unsigned char gfx[64 * 32];
extern unsigned short key[16];

#endif

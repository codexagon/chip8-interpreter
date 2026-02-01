#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

typedef struct chip8 {
	uint8_t ram[4096];
	uint8_t V[16];
	uint16_t I;
	uint16_t pc;
	uint16_t stack[16];
	uint8_t sp;
	uint8_t DT, ST;
	uint8_t screen[32][64];
	uint8_t keypad[16];
} chip8;

void initialize(chip8 *e);
int load_rom(chip8 *e, char *rom_file);
uint16_t fetch_opcode(chip8 *e);
void mainloop(chip8 *e);
void draw_screen(chip8 *e);

#endif

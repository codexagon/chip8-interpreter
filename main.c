#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "chip8.h"
#include "helper.h"
#include "opcodes.h"
#include "window.h"

chip8 emulator;

uint8_t fontset[80] = {
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

void initialize(chip8 *e) {
	if (init_sdl(e) > 0) {
		exit(1);
	}

	e->pc = 0x200;
	e->sp = 0;

	memset(e->ram, 0, sizeof(e->ram));
	memset(e->V, 0, sizeof(e->V));
	memset(e->stack, 0, sizeof(e->stack));
	for (int i = 0; i < 32; i++) {
		memset((e->screen)[i], 0, sizeof((e->screen)[i]));
	}
	memset(e->keypad, 0x0, sizeof(e->keypad));
	memcpy(&((e->ram)[0x50]), fontset, 80);

	srand(time(0));
}

int main(int argc, char *argv[]) {
	initialize(&emulator);

	if (load_rom(&emulator, argv[1]) != 0) {
		printf("Invalid ROM file passed.\n");
		return 1;
	}

	struct timeval t;
	double start = 0, end = 0;

	while (1) {
		start = get_time_ms(&t);
		handle_input(&emulator);
		for (int i = 0; i < 10; i++) {
			mainloop(&emulator);
		}
		draw_screen(&emulator);
		end = get_time_ms(&t);

		// Sleep for remaining time if rendering is finished before 1/60 seconds
		if (end - start < 16.667) {
			SDL_Delay(16.667 - (end - start));
		}

		if (emulator.DT > 0) {
			(emulator.DT)--;
		}
		if (emulator.ST > 0) {
			(emulator.ST)--;
		}
	}

	return 0;
}

uint16_t fetch_opcode(chip8 *e) {
	uint16_t opcode = ((e->ram)[e->pc] << 8) | ((e->ram)[e->pc + 1]);
	e->pc += 2;
	return opcode;
}

void mainloop(chip8 *e) {
	uint16_t opcode = fetch_opcode(e);
	execute_opcodes(e, opcode);
}

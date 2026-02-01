#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "chip8.h"
#include "helper.h"
#include "input.h"

chip8 emulator;

void initialize(chip8 *e) {
	setup_terminal();

	e->pc = 0x200;
	e->sp = 0;

	memset(e->ram, 0, sizeof(e->ram));
	memset(e->V, 0, sizeof(e->V));
	memset(e->stack, 0, sizeof(e->stack));
	for (int i = 0; i < 32; i++) {
		memset((e->screen)[i], 0, sizeof((e->screen)[i]));
	}
	memset(e->keypad, 0x0, sizeof(e->keypad));

	srand(time(0));
}

int main(int argc, char *argv[]) {
	initialize(&emulator);

	if (load_rom(&emulator, argv[1]) != 0) {
		restore_terminal();
		printf("Invalid ROM file passed.\n");
		return 1;
	}

	while (1) {
		handle_input(&emulator);
		for (int i = 0; i < 100; i++) {
			mainloop(&emulator);
		}
		draw_screen(&emulator);
		memset(emulator.keypad, 0x0, sizeof(emulator.keypad));
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

	// variables
	uint16_t addr;
	uint8_t value;
	uint8_t X, Y;
	uint8_t n;
	uint8_t sum, carry;
	uint8_t bit;

	switch (opcode & 0xF000) {
	case 0x0000:
		switch (opcode & 0x000F) {
		case 0x0:
			// Clear the display
			for (int i = 0; i < 32; i++) {
				memset((e->screen)[i], 0, sizeof((e->screen)[i]));
			}
			break;
		case 0xE:
			// Return from a subroutine
			e->pc = (e->stack)[e->sp];
			(e->sp)--;
			break;
		}
		break;
	case 0x1000:
		// Jump to address
		addr = opcode & 0x0FFF;
		e->pc = addr;
		break;
	case 0x2000:
		// Call subroutine at nnn
		addr = opcode & 0x0FFF;
		(e->sp)++;
		(e->stack)[e->sp] = e->pc;
		e->pc = addr;
		break;
	case 0x3000:
		// Skip next instruction if VX = kk
		X = (opcode & 0x0F00) >> 8;
		value = opcode & 0x00FF;
		if ((e->V)[X] == value) {
			e->pc += 2;
		}
		break;
	case 0x4000:
		// Skip next instruction if VX != kk
		X = (opcode & 0x0F00) >> 8;
		value = opcode & 0x00FF;
		if ((e->V)[X] != value) {
			e->pc += 2;
		}
		break;
	case 0x5000:
		// Skip next instruction if VX = VY
		X = (opcode & 0x0F00) >> 8;
		Y = (opcode & 0x00F0) >> 4;
		if ((e->V)[X] == (e->V)[Y]) {
			e->pc += 2;
		}
		break;
	case 0x6000:
		// Set VX = kk
		value = opcode & 0x00FF;
		X = (opcode & 0x0F00) >> 8;
		(e->V)[X] = value;
		break;
	case 0x7000:
		// Set VX = VX + kk
		value = opcode & 0x00FF;
		X = (opcode & 0x0F00) >> 8;
		(e->V)[X] = ((e->V)[X] + value) % 256;
		break;
	case 0x8000:
		switch (opcode & 0x000F) {
		case 0x0:
			// Set VX = VY
			X = (opcode & 0x0F00) >> 8;
			Y = (opcode & 0x00F0) >> 4;
			(e->V)[X] = (e->V)[Y];
			break;
		case 0x1:
			// Set VX = VX OR VY
			X = (opcode & 0x0F00) >> 8;
			Y = (opcode & 0x00F0) >> 4;
			(e->V)[X] |= (e->V)[Y];
			(e->V)[0xF] = 0;
			break;
		case 0x2:
			// Set VX = VX AND VY
			X = (opcode & 0x0F00) >> 8;
			Y = (opcode & 0x00F0) >> 4;
			(e->V)[X] &= (e->V)[Y];
			(e->V)[0xF] = 0;
			break;
		case 0x3:
			// Set VX = VX XOR VY
			X = (opcode & 0x0F00) >> 8;
			Y = (opcode & 0x00F0) >> 4;
			(e->V)[X] ^= (e->V)[Y];
			(e->V)[0xF] = 0;
			break;
		case 0x4:
			// Set VX = VX + VY, VF = carry
			X = (opcode & 0x0F00) >> 8;
			Y = (opcode & 0x00F0) >> 4;
			addr = (e->V)[X] + (e->V)[Y];
			carry = (addr > 255) ? 1 : 0;
			(e->V)[X] = addr & 0xFF;
			(e->V)[0xF] = carry;
			break;
		case 0x5:
			// Set VX = VX - VY, VF = NOT borrow
			X = (opcode & 0x0F00) >> 8;
			Y = (opcode & 0x00F0) >> 4;
			value = (e->V)[X] - (e->V)[Y];
			carry = (e->V)[X] < (e->V)[Y] ? 0 : 1;
			(e->V)[X] = value;
			(e->V)[0xF] = carry;
			break;
		case 0x6:
			// If LSB of VX = 1, VF = 1 else 0, VX = VX / 2
			X = (opcode & 0x0F00) >> 8;
			bit = (e->V)[X] & 0x1;
			(e->V)[X] >>= 1;
			(e->V)[0xF] = bit;
			break;
		case 0x7:
			// Set VX = VY - VX, VF = NOT borrow
			X = (opcode & 0x0F00) >> 8;
			Y = (opcode & 0x00F0) >> 4;
			value = (e->V)[Y] - (e->V)[X];
			carry = (e->V)[Y] < (e->V)[X] ? 0 : 1;
			(e->V)[X] = value;
			(e->V)[0xF] = carry;
			break;
		case 0xE:
			// If MSB of VX = 1, VF = 1 else 0, VX = VX * 2
			X = (opcode & 0x0F00) >> 8;
			bit = ((e->V)[X] & 0x80) >> 7;
			(e->V)[X] <<= 1;
			(e->V)[0xF] = bit;
			break;
		}
		break;
	case 0x9000:
		// Skip next instruction if VX != VY
		X = (opcode & 0x0F00) >> 8;
		Y = (opcode & 0x00F0) >> 4;
		if ((e->V)[X] != (e->V)[Y]) {
			e->pc += 2;
		}
		break;
	case 0xA000:
		// Set I = nnn
		addr = opcode & 0x0FFF;
		(e->I) = addr;
		break;
	case 0xB000:
		// Jump to location NNN + V0
		addr = opcode & 0x0FFF;
		e->pc = addr + (e->V)[0];
		break;
	case 0xC000:
		// Generate a random number, AND it with KK, store in VX
		value = rand();
		value &= (opcode & 0x00FF);
		X = (opcode & 0x0F00) >> 8;
		(e->V)[X] = value;
		break;
	case 0xD000:
		// Draw sprite on screen
		X = (opcode & 0x0F00) >> 8;
		Y = (opcode & 0x00F0) >> 4;
		n = opcode & 0x000F;
		(e->V)[0xF] = 0;

		for (int i = 0; i < n; i++) {
			for (int j = 0; j < 8; j++) {
				if ((e->screen)[((e->V)[Y] + i) % 32][((e->V)[X] + j) % 64] == 1 &&
				    (e->ram[(e->I) + i] & (1 << (7 - j))) >> (7 - j) == 1) {
					(e->V)[0xF] = 1;
				}
				(e->screen)[((e->V)[Y] + i) % 32][((e->V)[X] + j) % 64] ^= (e->ram[(e->I) + i] & (1 << (7 - j))) >> (7 - j);
			}
		}
		break;
	case 0xE000:
		switch (opcode & 0x00FF) {
		case 0x9E:
			// Skip next instruction if key with value VX is pressed
			X = (opcode & 0x0F00) >> 8;
			if ((e->keypad)[(e->V)[X]] == 0x1) {
				e->pc += 2;
			}
			break;
		case 0xA1:
			// Skip next instruction if key with value VX is not pressed
			X = (opcode & 0x0F00) >> 8;
			if ((e->keypad)[(e->V)[X]] != 0x1) {
				e->pc += 2;
			}
			break;
		}
		break;
	case 0xF000:
		switch (opcode & 0x00FF) {
		case 0x07:
			// Set VX = DT
			X = (opcode & 0x0F00) >> 8;
			(e->V)[X] = e->DT;
			break;
		case 0x0A:
			// Wait for key press, then store that key's value in VX
			X = (opcode & 0x0F00) >> 8;
			value = 0;
			for (int i = 0; i < 16; i++) {
				if ((e->keypad)[i] == 0x1) {
					(e->V)[X] = i;
					value = 1;
					break;
				}
			}
			if (value == 0) {
				e->pc -= 2;
			}
			break;
		case 0x15:
			// Set DT = VX
			X = (opcode & 0x0F00) >> 8;
			e->DT = (e->V)[X];
			break;
		case 0x18:
			// Set ST = VX
			X = (opcode & 0x0F00) >> 8;
			e->ST = (e->V)[X];
			break;
		case 0x1E:
			// Set I = I + VX
			X = (opcode & 0x0F00) >> 8;
			(e->I) += (e->V)[X];
			break;
		case 0x29:
			break;
		case 0x33:
			// Store BCD representation of VX
			X = (opcode & 0x0F00) >> 8;
			value = (e->V)[X];
			e->ram[(e->I)] = (e->V)[X] / 100;
			e->ram[(e->I) + 1] = ((e->V)[X] / 10) % 10;
			e->ram[(e->I) + 2] = (e->V)[X] % 10;
			break;
		case 0x55:
			// Store V0 through Vx in consecutive locations starting from I
			X = (opcode & 0x0F00) >> 8;
			for (int i = 0; i <= X; i++) {
				e->ram[(e->I) + i] = (e->V)[i];
			}
			break;
		case 0x65:
			// Read V0 through Vx from consecutive locations starting from I
			X = (opcode & 0x0F00) >> 8;
			for (int i = 0; i <= X; i++) {
				(e->V)[i] = e->ram[(e->I) + i];
			}
			break;
		}
		break;
	default:
		printf("Invalid opcode found: %#04X\n", opcode);
		break;
	}
}

void draw_screen(chip8 *e) {
	printf("\033[J\033[H");

	printf("┌");
	for (int c = 0; c < 64; c++) {
		printf("──");
	}
	printf("┐\n");
	for (int r = 0; r < 32; r++) {
		printf("│");
		for (int c = 0; c < 64; c++) {
			if ((e->screen)[r][c] > 0) {
				printf("██");
			} else {
				printf("  ");
			}
		}
		printf("│\n");
	}
	printf("└");
	for (int c = 0; c < 64; c++) {
		printf("──");
	}
	printf("┘\n");
}

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

uint8_t ram[4096];
uint8_t V[16];
uint16_t I;
uint16_t pc;
uint16_t stack[16];
uint8_t sp;
uint8_t DT, ST;
uint8_t screen[32][64];

void initialize();
int load_rom(char *rom_file);
uint16_t fetch_opcode();
void mainloop();
void draw_screen();

void initialize() {
	printf("\033[2J\033[H");
	pc = 0x200;
	sp = 0;

	memset(ram, 0, sizeof(ram));
	memset(V, 0, sizeof(V));
	memset(stack, 0, sizeof(stack));
	for (int i = 0; i < 32; i++) {
		memset(screen[i], 0, sizeof(screen[i]));
	}

	srand(time(0));
}

int load_rom(char *rom_file) {
	FILE *fp = fopen(rom_file, "rb");
	if (fp == NULL) {
		return 1;
	}

	fseek(fp, 0L, SEEK_END);
	size_t size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	for (int i = 0; i < size; i++) {
		ram[pc + i] = (uint8_t)fgetc(fp);
	}

	fclose(fp);

	return 0;
}

int main(int argc, char *argv[]) {
	initialize();

	if (load_rom(argv[1]) != 0) {
		printf("Invalid ROM file passed.\n");
		return 1;
	}

	while (1) {
		mainloop();
	}

	return 0;
}

uint16_t fetch_opcode() {
	uint16_t opcode = (ram[pc] << 8) | (ram[pc + 1]);
	pc += 2;
	return opcode;
}

void mainloop() {
	uint16_t opcode = fetch_opcode();

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
				memset(screen[i], 0, sizeof(screen[i]));
			}
			break;
		case 0xE:
			// Return from a subroutine
			pc = stack[sp];
			sp--;
			break;
		}
		break;
	case 0x1000:
		// Jump to address
		addr = opcode & 0x0FFF;
		pc = addr;
		break;
	case 0x2000:
		// Call subroutine at nnn
		addr = opcode & 0x0FFF;
		sp++;
		stack[sp] = pc;
		pc = addr;
		break;
	case 0x3000:
		// Skip next instruction if VX = kk
		X = (opcode & 0x0F00) >> 8;
		value = opcode & 0x00FF;
		if (V[X] == value) {
			pc += 2;
		}
		break;
	case 0x4000:
		// Skip next instruction if VX != kk
		X = (opcode & 0x0F00) >> 8;
		value = opcode & 0x00FF;
		if (V[X] != value) {
			pc += 2;
		}
		break;
	case 0x5000:
		// Skip next instruction if VX = VY
		X = (opcode & 0x0F00) >> 8;
		Y = (opcode & 0x00F0) >> 4;
		if (V[X] == V[Y]) {
			pc += 2;
		}
		break;
	case 0x6000:
		// Set VX = kk
		value = opcode & 0x00FF;
		X = (opcode & 0x0F00) >> 8;
		V[X] = value;
		break;
	case 0x7000:
		// Set VX = VX + kk
		value = opcode & 0x00FF;
		X = (opcode & 0x0F00) >> 8;
		V[X] = (V[X] + value) % 256;
		break;
	case 0x8000:
		switch (opcode & 0x000F) {
		case 0x0:
			// Set VX = VY
			X = (opcode & 0x0F00) >> 8;
			Y = (opcode & 0x00F0) >> 4;
			V[X] = V[Y];
			break;
		case 0x1:
			// Set VX = VX OR VY
			X = (opcode & 0x0F00) >> 8;
			Y = (opcode & 0x00F0) >> 4;
			V[X] |= V[Y];
			V[0xF] = 0;
			break;
		case 0x2:
			// Set VX = VX AND VY
			X = (opcode & 0x0F00) >> 8;
			Y = (opcode & 0x00F0) >> 4;
			V[X] &= V[Y];
			V[0xF] = 0;
			break;
		case 0x3:
			// Set VX = VX XOR VY
			X = (opcode & 0x0F00) >> 8;
			Y = (opcode & 0x00F0) >> 4;
			V[X] ^= V[Y];
			V[0xF] = 0;
			break;
		case 0x4:
			// Set VX = VX + VY, VF = carry
			X = (opcode & 0x0F00) >> 8;
			Y = (opcode & 0x00F0) >> 4;
			addr = V[X] + V[Y];
			carry = (addr > 255) ? 1 : 0;
			V[X] = addr & 0xFF;
			V[0xF] = carry;
			break;
		case 0x5:
			// Set VX = VX - VY, VF = NOT borrow
			X = (opcode & 0x0F00) >> 8;
			Y = (opcode & 0x00F0) >> 4;
			value = V[X] - V[Y];
			carry = V[X] < V[Y] ? 0 : 1;
			V[X] = value;
			V[0xF] = carry;
			break;
		case 0x6:
			// If LSB of VX = 1, VF = 1 else 0, VX = VX / 2
			X = (opcode & 0x0F00) >> 8;
			bit = V[X] & 0x1;
			V[X] >>= 1;
			V[0xF] = bit;
			break;
		case 0x7:
			// Set VX = VY - VX, VF = NOT borrow
			X = (opcode & 0x0F00) >> 8;
			Y = (opcode & 0x00F0) >> 4;
			value = V[Y] - V[X];
			carry = V[Y] < V[X] ? 0 : 1;
			V[X] = value;
			V[0xF] = carry;
			break;
		case 0xE:
			// If MSB of VX = 1, VF = 1 else 0, VX = VX * 2
			X = (opcode & 0x0F00) >> 8;
			bit = (V[X] & 0x80) >> 7;
			V[X] <<= 1;
			V[0xF] = bit;
			break;
		}
		break;
	case 0x9000:
		// Skip next instruction if VX != VY
		X = (opcode & 0x0F00) >> 8;
		Y = (opcode & 0x00F0) >> 4;
		if (V[X] != V[Y]) {
			pc += 2;
		}
		break;
	case 0xA000:
		// Set I = nnn
		addr = opcode & 0x0FFF;
		I = addr;
		break;
	case 0xB000:
		// Jump to location NNN + V0
		addr = opcode & 0x0FFF;
		pc = addr + V[0];
		break;
	case 0xC000:
		// Generate a random number, AND it with KK, store in VX
		value = rand();
		value &= (opcode & 0x00FF);
		X = (opcode & 0x0F00) >> 8;
		V[X] = value;
		break;
	case 0xD000:
		// Draw sprite on screen
		X = (opcode & 0x0F00) >> 8;
		Y = (opcode & 0x00F0) >> 4;
		n = opcode & 0x000F;
		V[0xF] = 0;

		for (int i = 0; i < n; i++) {
			for (int j = 0; j < 8; j++) {
				if (screen[(V[Y] + i) % 32][(V[X] + j) % 64] == 1 && (ram[I + i] & (1 << (7 - j))) >> (7 - j) == 1) {
					V[0xF] = 1;
				}
				screen[(V[Y] + i) % 32][(V[X] + j) % 64] ^= (ram[I + i] & (1 << (7 - j))) >> (7 - j);
			}
		}
		draw_screen();
		break;
	case 0xE000:
		switch (opcode & 0x00FF) {
		case 0x9E:
			break;
		case 0xA1:
			break;
		}
		break;
	case 0xF000:
		switch (opcode & 0x00FF) {
		case 0x07:
			// Set VX = DT
			X = (opcode & 0x0F00) >> 8;
			V[X] = DT;
			break;
		case 0x0A:
			break;
		case 0x15:
			// Set DT = VX
			X = (opcode & 0x0F00) >> 8;
			DT = V[X];
			break;
		case 0x18:
			// Set ST = VX
			X = (opcode & 0x0F00) >> 8;
			ST = V[X];
			break;
		case 0x1E:
			// Set I = I + VX
			X = (opcode & 0x0F00) >> 8;
			I += V[X];
			break;
		case 0x29:
			break;
		case 0x33:
			// Store BCD representation of VX
			X = (opcode & 0x0F00) >> 8;
			value = V[X];
			ram[I] = V[X] / 100;
			ram[I + 1] = (V[X] / 10) % 10;
			ram[I + 2] = V[X] % 10;
			break;
		case 0x55:
			// Store V0 through Vx in consecutive locations starting from I
			X = (opcode & 0x0F00) >> 8;
			for (int i = 0; i <= X; i++) {
				ram[I + i] = V[i];
			}
			break;
		case 0x65:
			// Read V0 through Vx from consecutive locations starting from I
			X = (opcode & 0x0F00) >> 8;
			for (int i = 0; i <= X; i++) {
				V[i] = ram[I + i];
			}
			break;
		}
		break;
	default:
		printf("Invalid opcode found: %#04X\n", opcode);
		break;
	}
}

void draw_screen() {
	printf("\033[J\033[H");

	printf("┌");
	for (int c = 0; c < 64; c++) {
		printf("──");
	}
	printf("┐\n");
	for (int r = 0; r < 32; r++) {
		printf("│");
		for (int c = 0; c < 64; c++) {
			if (screen[r][c] > 0) {
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

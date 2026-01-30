#include <stdint.h>
#include <stdio.h>
#include <string.h>

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

	switch(opcode & 0xF000) {
		case 0x0000:
			switch(opcode & 0x000F) {
				case 0:
					// Clear the display
					for (int i = 0; i < 32; i++) {
						memset(screen[i], 0, sizeof(screen[i]));
					}
					break;
			}
			break;
		case 0x1000:
			// Jump to address
			addr = opcode & 0x0FFF;
			pc = addr;
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
			V[X] += value;
			break;
		case 0xA000:
			// Set I = nnn
			addr = opcode & 0x0FFF;
			I = addr;
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
		default:
			printf("Invalid opcode found: %#04X\n", opcode);
			break;
	}
}

void draw_screen() {
	printf("\033[J\033[H");
	for (int r = 0; r < 32; r++) {
		for (int c = 0; c < 64; c++) {
			if (screen[r][c] > 0) {
				printf("██");
			} else {
				printf("  ");
			}
		}
		printf("\n");
	}
}

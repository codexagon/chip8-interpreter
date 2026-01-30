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

int main(int argc, char *argv[]) {
	initialize();

	if (load_rom(argv[1]) != 0) {
		printf("Invalid ROM file passed.\n");
		return 1;
	}

	return 0;
}

void initialize() {
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

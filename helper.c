#include "helper.h"

int load_rom(chip8 *e, char *rom_file) {
	FILE *fp = fopen(rom_file, "rb");
	if (fp == NULL) {
		return 1;
	}

	fseek(fp, 0L, SEEK_END);
	size_t size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	for (int i = 0; i < size; i++) {
		(e->ram)[e->pc + i] = (uint8_t)fgetc(fp);
	}

	fclose(fp);

	return 0;
}

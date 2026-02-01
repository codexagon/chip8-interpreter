#ifndef HELPER_H
#define HELPER_H

#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>

#include "chip8.h"

int load_rom(chip8 *e, char *rom_file);
double get_time_ms(struct timeval *t);

#endif

#ifndef INPUT_H
#define INPUT_H

#include "chip8.h"

void setup_terminal();
void restore_terminal();
void handle_input(chip8 *e);

#endif

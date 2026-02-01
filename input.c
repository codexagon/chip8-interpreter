#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "input.h"

struct termios original;

void setup_terminal() {
	// Enable "raw" mode terminal (disable line buffering (canonical mode), disable echoing of input)
	tcgetattr(STDIN_FILENO, &original);
	struct termios raw = original;
	raw.c_lflag &= ~(ICANON | ECHO);

	// Make terminal non-blocking (return read input immediately without blocking)
	int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

	tcsetattr(STDIN_FILENO, TCSANOW, &raw);

	printf("\033[?1049h");   // Enter alternate buffer
	printf("\033[2J\033[H"); // Clear screen & position cursor at (0, 0)
	fflush(stdout);
}

void restore_terminal() {
	printf("\033[?1049l");                       // Exit alternate buffer
	tcsetattr(STDIN_FILENO, TCSANOW, &original); // Restore original terminal settings
}

void handle_input(chip8 *e) {
	int c = getchar();

	// Set element in keypad array to value of the actual keypad character
	if (c != EOF) {
		switch (c) {
		case 0x0A:
			// Enter
			restore_terminal();
			exit(0);
		case 0x31:
			// 1
			(e->keypad)[0x1] = 0x1;
			break;
		case 0x32:
			// 2
			(e->keypad)[0x2] = 0x1;
			break;
		case 0x33:
			// 3
			(e->keypad)[0x3] = 0x1;
			break;
		case 0x34:
			// 4
			(e->keypad)[0xC] = 0x1;
			break;
		case 0x71:
			// Q
			(e->keypad)[0x4] = 0x1;
			break;
		case 0x77:
			// W
			(e->keypad)[0x5] = 0x1;
			break;
		case 0x65:
			// E
			(e->keypad)[0x6] = 0x1;
			break;
		case 0x72:
			// R
			(e->keypad)[0xD] = 0x1;
			break;
		case 0x61:
			// A
			(e->keypad)[0x7] = 0x1;
			break;
		case 0x73:
			// S
			(e->keypad)[0x8] = 0x1;
			break;
		case 0x64:
			// D
			(e->keypad)[0x9] = 0x1;
			break;
		case 0x66:
			// F
			(e->keypad)[0xE] = 0x1;
			break;
		case 0x7A:
			// Z
			(e->keypad)[0xA] = 0x1;
			break;
		case 0x78:
			// X
			(e->keypad)[0x0] = 0x1;
			break;
		case 0x63:
			// C
			(e->keypad)[0xB] = 0x1;
			break;
		case 0x76:
			// V
			(e->keypad)[0xF] = 0x1;
			break;
		}
		for (int i = 0; i < 16; i++) {
			if ((e->keypad)[i] == 1) {
				printf("%X\n", i);
			}
		}
	}
}

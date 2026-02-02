#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "window.h"

#define SCALE 20

int init_sdl(chip8 *e) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL init failed: %s\n", SDL_GetError());
		return 1;
	}
	e->window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 64 * SCALE, 32 * SCALE, SDL_WINDOW_SHOWN);
	if (!(e->window)) {
		printf("Window creation failed: %s\n", SDL_GetError());
		return 2;
	}
	e->renderer = SDL_CreateRenderer(e->window, -1, SDL_RENDERER_ACCELERATED);
	if (!(e->renderer)) {
		printf("Renderer creation failed: %s\n", SDL_GetError());
		return 3;
	}
	return 0;
}

void close_sdl(chip8 *e) {
	SDL_DestroyRenderer(e->renderer);
	SDL_DestroyWindow(e->window);
	SDL_Quit();
}

void handle_input(chip8 *e) {
	while (SDL_PollEvent(&(e->event))) {
		if ((e->event).type == SDL_KEYDOWN) {
			switch ((e->event).key.keysym.sym) {
				case SDLK_1: (e->keypad)[0x1] = 1; break;
				case SDLK_2: (e->keypad)[0x2] = 1; break;
				case SDLK_3: (e->keypad)[0x3] = 1; break;
				case SDLK_4: (e->keypad)[0xC] = 1; break;
				case SDLK_q: (e->keypad)[0x4] = 1; break;
				case SDLK_w: (e->keypad)[0x5] = 1; break;
				case SDLK_e: (e->keypad)[0x6] = 1; break;
				case SDLK_r: (e->keypad)[0xD] = 1; break;
				case SDLK_a: (e->keypad)[0x7] = 1; break;
				case SDLK_s: (e->keypad)[0x8] = 1; break;
				case SDLK_d: (e->keypad)[0x9] = 1; break;
				case SDLK_f: (e->keypad)[0xE] = 1; break;
				case SDLK_z: (e->keypad)[0xA] = 1; break;
				case SDLK_x: (e->keypad)[0x0] = 1; break;
				case SDLK_c: (e->keypad)[0xB] = 1; break;
				case SDLK_v: (e->keypad)[0xF] = 1; break;
				case SDLK_RETURN: {
					close_sdl(e);
					exit(0);
				}
			}
		} else if ((e->event).type == SDL_KEYUP) {
			switch ((e->event).key.keysym.sym) {
				case SDLK_1: (e->keypad)[0x1] = 0; break;
				case SDLK_2: (e->keypad)[0x2] = 0; break;
				case SDLK_3: (e->keypad)[0x3] = 0; break;
				case SDLK_4: (e->keypad)[0xC] = 0; break;
				case SDLK_q: (e->keypad)[0x4] = 0; break;
				case SDLK_w: (e->keypad)[0x5] = 0; break;
				case SDLK_e: (e->keypad)[0x6] = 0; break;
				case SDLK_r: (e->keypad)[0xD] = 0; break;
				case SDLK_a: (e->keypad)[0x7] = 0; break;
				case SDLK_s: (e->keypad)[0x8] = 0; break;
				case SDLK_d: (e->keypad)[0x9] = 0; break;
				case SDLK_f: (e->keypad)[0xE] = 0; break;
				case SDLK_z: (e->keypad)[0xA] = 0; break;
				case SDLK_x: (e->keypad)[0x0] = 0; break;
				case SDLK_c: (e->keypad)[0xB] = 0; break;
				case SDLK_v: (e->keypad)[0xF] = 0; break;
			}
		}
	}
}

void draw_screen(chip8 *e) {
	// Draw black screen
	SDL_SetRenderDrawColor(e->renderer, 0, 0, 0, 255);
	SDL_RenderClear(e->renderer);

	// Draw white squares if a pixel is on
	SDL_SetRenderDrawColor(e->renderer, 255, 255, 255, 255);
	for (int r = 0; r < 32; r++) {
		for (int c = 0; c < 64; c++) {
			if ((e->screen)[r][c]) {
				SDL_Rect rect = {c * SCALE, r * SCALE, SCALE, SCALE};
				SDL_RenderFillRect(e->renderer, &rect);
			}
		}
	}

	SDL_RenderPresent(e->renderer);
}

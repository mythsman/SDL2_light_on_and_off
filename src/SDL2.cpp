#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <stdio.h>
const int windowWidth = 400;
const int windowHeight = 640;
const SDL_Rect rectBulb = { 0, 0, 400, 400 };
const SDL_Rect rectSwitcher = { 0, 340, 400, 300 };
bool isButton(int x, int y) {
	if (x >= 90 && x <= 310 && y >= 445 && y <= 538) {
		if (x < 134) {
			return (134 - x) * (134 - x) + (491 - y) * (491 - y) <= 42 * 42;
		} else if (x > 267) {
			return (268 - x) * (268 - x) + (491 - y) * (491 - y) <= 42 * 42;
		}
		return true;
	}
	return false;
}
int main(int argc, char** argv) {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window *window = SDL_CreateWindow("Bulb", SDL_WINDOWPOS_CENTERED,
	SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
	SDL_Renderer *render = SDL_CreateRenderer(window, -1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_Surface *blackBmpSurface = SDL_LoadBMP("resources/black.bmp");
	SDL_Surface *whiteBmpSurface = SDL_LoadBMP("resources/white.bmp");
	if (blackBmpSurface == nullptr) {
		puts((char *) SDL_GetError());
		return 1;
	}
	if (whiteBmpSurface == nullptr) {
		puts((char *) SDL_GetError());
		return 1;
	}
	SDL_Surface *surface = blackBmpSurface;
	SDL_Texture *texture = SDL_CreateTextureFromSurface(render, surface);
	SDL_RenderClear(render);
	SDL_RenderCopy(render, texture, NULL, &rectBulb);
	SDL_RenderPresent(render);
	SDL_Surface *switcher[30];
	for (int i = 0; i < 30; i++) {
		char src[50] = { 0 };
		sprintf(src, "resources/switcher/%d.bmp", i);
		switcher[i] = SDL_LoadBMP(src);
	}
	if (switcher == nullptr) {
		puts((char *) SDL_GetError());
		return 1;
	}
	texture = SDL_CreateTextureFromSurface(render, switcher[15]);
	SDL_RenderCopy(render, texture, NULL, &rectSwitcher);
	SDL_RenderPresent(render);
	bool quit = false;
	while (quit == false) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				printf("exit\n");
				quit = true;
				break;
			case SDL_MOUSEBUTTONDOWN:
				printf("Click on (%d,%d)\n", event.button.x, event.button.y);
				if (!isButton(event.button.x, event.button.y))
					break;
				if (surface == blackBmpSurface) {
					surface = whiteBmpSurface;
					texture = SDL_CreateTextureFromSurface(render, surface);
					SDL_RenderCopy(render, texture, NULL, &rectBulb);
					SDL_RenderPresent(render);
					for (int i = 15; i < 30; i++) {
						texture = SDL_CreateTextureFromSurface(render,
								switcher[i]);
						SDL_RenderCopy(render, texture, NULL, &rectSwitcher);
						SDL_RenderPresent(render);
					}
					printf("Light on\n");
				} else {
					surface = blackBmpSurface;
					texture = SDL_CreateTextureFromSurface(render, surface);
					SDL_RenderCopy(render, texture, NULL, &rectBulb);
					SDL_RenderPresent(render);
					for (int i = 0; i < 15; i++) {
						texture = SDL_CreateTextureFromSurface(render,
								switcher[i]);
						SDL_RenderCopy(render, texture, NULL, &rectSwitcher);
						SDL_RenderPresent(render);
					}
					printf("Light off\n");
				}
				break;
			default:
				break;
			}
		}
	}
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <stdio.h>
const int windowWidth = 400;
const int windowHeight = 640;
const SDL_Rect rectBulb = { 0, 0, 400, 400 };
const SDL_Rect rectSwitcher = { 0, 340, 400, 300 };
bool isButton(int x, int y) {//判断鼠标与按钮的位置关系
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

SDL_Cursor *getCursor() {//绘制鼠标指针
	const char *image[] = {
	/* width height num_colors chars_per_pixel */
	"    32    32        3            1",
	/* colors */
	"X c #000000", ". c #ffffff", "  c None",
	/* pixels */
	"                                ", "                                ",
			"           XX                   ",
			"          XXXX                  ",
			"         XX..XX                 ",
			"         XX..XX                 ",
			"         XX..XX                 ",
			"         XX..XX                 ",
			"         XX..XXXX               ",
			"         XX..XXXXX  X           ",
			"         XX..XX..XXXXX          ",
			"         XX..XX..XX..XXX        ",
			"         XX..XX..XX..XX.XX      ",
			"    XXXX XX..XX..XX..XX..XX     ",
			"    XXXXXXX..XX..XX..XX..XX     ",
			"    XX..XXX..........XX..XX     ",
			"    XX..XX...............XX     ",
			"     XX..XX..............XX     ",
			"      XX.XX..............XX     ",
			"       X..X..............XX     ",
			"       X.................XX     ",
			"        XX...............XX     ",
			"        XX..............XX      ",
			"         XX.............XX      ",
			"         XX............6.XX      ",
			"         XX.............X       ",
			"           X...........X        ",
			"          XX...........X        ",
			"           XXXXXXXXXXXXX        ",
			"           XXXXXXXXXXXXX        ",
			"                                ",
			"                                ", "12,2" };
	int i, row, col;
	unsigned char data[4 * 32];//根据样式构造数据参数
	unsigned char mask[4 * 32];
	int hot_x, hot_y;
	i = -1;
	for (row = 0; row < 32; ++row) {
		for (col = 0; col < 32; ++col) {
			if (col % 8) {
				data[i] <<= 1;
				mask[i] <<= 1;
			} else {
				++i;
				data[i] = mask[i] = 0;
			}
			switch (image[4 + row][col]) {
			case 'X':
				data[i] |= 0x01;
				mask[i] |= 0x01;
				break;
			case '.':
				mask[i] |= 0x01;
				break;
			case ' ':
				break;
			}
		}
	}
	sscanf(image[4 + row], "%d,%d", &hot_x, &hot_y);
	return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}

int main(int argc, char** argv) {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window *window = SDL_CreateWindow("Bulb", SDL_WINDOWPOS_CENTERED,
	SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
	SDL_Renderer *render = SDL_CreateRenderer(window, -1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);//默认使用硬件加速
	if (render == nullptr) {
		render = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);//如果硬件加速不支持就用软件加速
	}
	if (render == nullptr) {
		puts((char *) SDL_GetError());
		return 1;
	}
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
	for (int i = 0; i < 30; i++) {//预加载图片帧
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
	SDL_Cursor* rawCursor = SDL_GetCursor();
	SDL_Cursor* newCursor = getCursor();
	bool quit = false;
	while (quit == false) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT://帧听退出
				printf("exit\n");
				quit = true;
				break;
			case SDL_MOUSEMOTION://帧听指针位置
				printf("Move on (%d,%d)\n", event.button.x, event.button.y);
				if (isButton(event.button.x, event.button.y)) {//更换指针样式
					if (SDL_GetCursor() == rawCursor) {
						SDL_SetCursor(newCursor);
					}
				} else {
					if (SDL_GetCursor() == newCursor) {
						SDL_SetCursor(rawCursor);
					}
				}
				break;
			case SDL_MOUSEBUTTONDOWN://帧听点击事件
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
						SDL_Delay(30);//图片显示延时
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
						SDL_Delay(30);
					}
					printf("Light off\n");
				}
				break;
			default:
				break;
			}
		}
	}
	SDL_FreeCursor(newCursor);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

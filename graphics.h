#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCREEN_DEPTH 16

extern SDL_Surface* screen;

int init_screen();
SDL_Surface* load_png(const char* filename);
SDL_Rect blit(SDL_Surface* from, SDL_Surface* to, int dx, int dy, SDL_Rect* s);
void clear_surface(SDL_Surface* surface, SDL_Color color);
void fill_rect(SDL_Surface* surface, SDL_Rect rect, SDL_Color color);
int get_button(int id);

#endif

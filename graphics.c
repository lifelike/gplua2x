#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "SDL.h"
#include "SDL_image.h"
#include "unistd.h"
#include "string.h"
#include "graphics.h"

SDL_Surface* screen;

static SDL_Joystick* joystick;

SDL_Surface* load_png(const char* filename) {
    SDL_Surface* result;
    SDL_RWops* area;
    area = SDL_RWFromFile(filename, "r");
    result = IMG_LoadPNG_RW(area);
    SDL_FreeRW(area);
    return result;
}

int get_button(int id) {
   return SDL_JoystickGetButton(joystick, id);
}

int init_screen() {
   SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO | SDL_INIT_TIMER);
   joystick = SDL_JoystickOpen(0);
   screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH,
                              SDL_SWSURFACE);
#ifdef GP2X
   SDL_ShowCursor(0);
#endif
    return 1;
}
void clear_surface(SDL_Surface* surface, SDL_Color color) {
   SDL_Rect rect;
   rect.x = 0;
   rect.y = 0;
   rect.w = screen->w;
   rect.h = screen->h;
   fill_rect(surface, rect, color);
}

void fill_rect(SDL_Surface* surface, SDL_Rect rect, SDL_Color color) {
   SDL_FillRect(surface, &rect,
                SDL_MapRGB(screen->format, color.r, color.g, color.b));
}

SDL_Rect blit(SDL_Surface* from, SDL_Surface* to, int dx, int dy, SDL_Rect* s)
{
    SDL_Rect res;
    SDL_Rect d;
    d.x = dx;
    d.y = dy;
    SDL_BlitSurface(from, s, to, &d);
    res.x = dx;
    res.y = dy;
    if (s) {
       res.w = s->w;
       res.h = s->h;
    } else {
       res.w = from->w;
       res.h = from->h;
    }
    return res;
}



#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "SDL.h"
#include "ui.h"

#define MAX_COLUMN 50
#define COLUMN_INCREMENT 5
#define TEXT_MARGIN 10

int key_to_gp2x_button(int code) {
   switch(code) {
      case SDLK_UP:
      case SDLK_KP8:
      case SDLK_w:
         return GP2X_UP;
      case SDLK_DOWN:
      case SDLK_KP2:
      case SDLK_s:
         return GP2X_DOWN;
      case SDLK_RIGHT:
      case SDLK_KP6:
      case SDLK_d:
         return GP2X_RIGHT;
      case SDLK_LEFT:
      case SDLK_KP4:
      case SDLK_a:
         return GP2X_LEFT;
      case SDLK_KP5:
      case SDLK_x:
         return GP2X_STICKCLICK;
      case SDLK_RETURN:
      case SDLK_l:
         return GP2X_B;
      case SDLK_SPACE:
         return GP2X_START;
      case SDLK_ESCAPE:
         return GP2X_X;
      case SDLK_BACKSPACE:
         return GP2X_SELECT;
      case SDLK_q:
         return GP2X_QUIT;
      case SDLK_j:
         return GP2X_A;
      case SDLK_i:
         return GP2X_Y;
      case SDLK_k:
         return GP2X_X;
      case SDLK_1:
         return GP2X_L;
      case SDLK_0:
         return GP2X_R;
      default:
         return GP2X_NONE;
   }
}

int wait_for_any_key() {
    while(1) {
        SDL_Event event;
        SDL_WaitEvent(&event);
        switch(event.type) {
        case SDL_QUIT:
            return GP2X_QUIT;
        case SDL_KEYDOWN:
           return key_to_gp2x_button(event.key.keysym.sym);
        case SDL_MOUSEBUTTONDOWN:
            return GP2X_START;
        case SDL_JOYBUTTONDOWN:
            return event.jbutton.button;
        }
    }
}

int is_ok(int key) {
    return key == GP2X_START
        || key == GP2X_B
        || key == GP2X_Y
        || key == GP2X_STICKCLICK;
}

int display_image(const char* filename, int wait) {
    SDL_Surface* image = load_png(filename);
    blit(image, screen, 0, 0, NULL);
    SDL_FreeSurface(image);
    SDL_Flip(screen);
    if (wait) {
       return wait_for_any_key();
    } else {
       return GP2X_NONE;
    }
}

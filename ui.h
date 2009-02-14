#ifndef UI_H
#define UI_H

#include "graphics.h"

#define GP2X_UP 0
#define GP2X_DOWN 4
#define GP2X_LEFT 2
#define GP2X_RIGHT 6
#define GP2X_A 12
#define GP2X_B 13
#define GP2X_X 14
#define GP2X_Y 15
#define GP2X_L 10
#define GP2X_R 11
#define GP2X_VOLUP 16
#define GP2X_VOLDOWN 17
#define GP2X_SELECT 9
#define GP2X_START 8
#define GP2X_STICKCLICK 18
#define GP2X_UPLEFT 1
#define GP2X_DOWNLEFT 3
#define GP2X_UPRIGHT 7
#define GP2X_DOWNRIGHT 5
#define GP2X_QUIT -2
#define GP2X_NONE -1

#define GP2X_MAX_BUTTON_ID  18

int key_to_gp2x_button(int code);
int wait_for_any_key();
int is_ok(int key);
int display_image(const char* filename, int wait);

#endif

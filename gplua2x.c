#include "gplua2x.h"

#include "SDL/SDL.h"
#include <stdlib.h>
#include <unistd.h>
#include "graphics.h"
#include "ui.h"

#ifdef GP2X
#include <lua5.1/lauxlib.h>
#include <lua5.1/lualib.h>
#elif FREEBSD
#include <lua51/lauxlib.h>
#include <lua51/lualib.h>
#elif MACOSX
#include <lauxlib.h>
#include <lualib.h>
#else
#include <lua5.1/lauxlib.h>
#include <lua5.1/lualib.h>
#endif

typedef struct lua2x_Rect {
   SDL_Rect sdl_rect;
} lua2x_Rect;

static lua2x_Rect* lua2x_rect_check (lua_State* L, int n) {
   void* data = luaL_checkudata(L, n, "gp2x.rect");
   luaL_argcheck(L, data != NULL, n, "gp2x.rect expected");
   return data;
}

typedef struct lua2x_Surface {
   SDL_Surface* sdl_surface;
} lua2x_Surface;

static const struct luaL_reg lua2x_rect_methods [] = {
   {NULL, NULL}
};

static lua2x_Surface* lua2x_surface_check (lua_State* L, int n) {
   void* data = luaL_checkudata(L, n, "gp2x.surface");
   luaL_argcheck(L, data != NULL, n, "gp2x.surface expected");
   return data;
}

static int lua2x_surface_clear(lua_State* L) {
   lua2x_Surface* surface = lua2x_surface_check(L, 1);
   SDL_Color color;
   color.r = lua_tointeger(L, 2);
   color.g = lua_tointeger(L, 3);
   color.b = lua_tointeger(L, 4);
   clear_surface(surface->sdl_surface, color);
   return 0;
}

static int lua2x_surface_fill_rect(lua_State* L) {
   lua2x_Surface* surface = lua2x_surface_check(L, 1);
   lua2x_Rect* rect = lua2x_rect_check(L, 2);
   SDL_Color color;
   color.r = lua_tointeger(L, 3);
   color.g = lua_tointeger(L, 4);
   color.b = lua_tointeger(L, 5);
   fill_rect(surface->sdl_surface, rect->sdl_rect, color);
   return 0;
}

static int lua2x_surface_blit(lua_State* L) {
   int a = 1;
   SDL_Rect* s = NULL;
   lua2x_Rect* res;
   lua2x_Surface* to_surface = lua2x_surface_check(L, a++);
   lua2x_Surface* from_surface = lua2x_surface_check(L, a++);
   int dx = lua_tointeger(L, a++);
   int dy = lua_tointeger(L, a++);
   if (lua_gettop(L) == 5) {
      s = &(lua2x_rect_check(L, a++)->sdl_rect);
   }
   res = lua_newuserdata(L, sizeof(lua2x_Rect));
   res->sdl_rect = blit(from_surface->sdl_surface, to_surface->sdl_surface,
                        dx, dy, s);
   luaL_getmetatable(L, "gp2x.rect");
   lua_setmetatable(L, -2);
   return 1;
}

static int lua2x_surface_free(lua_State* L) {
   lua2x_Surface* surface = lua2x_surface_check(L, 1);
   fprintf(stderr, "freeing surface %p\n", surface);
   if (surface->sdl_surface) {
      SDL_FreeSurface(surface->sdl_surface);
   }
   surface->sdl_surface = NULL;
   return 0;
}

static const struct luaL_reg lua2x_surface_methods [] = {
   {"clear", lua2x_surface_clear},
   {"blit", lua2x_surface_blit},
   {"fillrect", lua2x_surface_fill_rect},
   {"__gc", lua2x_surface_free},
   {NULL, NULL}
};

static int lua2x_rect_new(lua_State* L) {
   lua2x_Rect* rect = lua_newuserdata(L, sizeof(lua2x_Rect));
   rect->sdl_rect.x = lua_tointeger(L, 1);
   rect->sdl_rect.y = lua_tointeger(L, 2);
   rect->sdl_rect.w = lua_tointeger(L, 3);
   rect->sdl_rect.h = lua_tointeger(L, 4);
   luaL_getmetatable(L, "gp2x.rect");
   lua_setmetatable(L, -2);
   return 1;
}

static int lua2x_get_screen(lua_State* L) {
   lua2x_Surface* lua2x_screen = lua_newuserdata(L, sizeof(lua2x_Surface));
   lua2x_screen->sdl_surface = screen;
   luaL_getmetatable(L, "gp2x.surface");
   lua_setmetatable(L, -2);
   return 1;
}

static int lua2x_load_png(lua_State* L) {
   const char* filename = luaL_checkstring(L, 1);
   SDL_Surface* image = load_png(filename);
   if (!image) {
      luaL_error(L, "lua2x failed to load image %s: %s",
                 filename, lua_tostring(L, -1));
   }
   lua2x_Surface* lua2x_screen = lua_newuserdata(L, sizeof(lua2x_Surface));
   lua2x_screen->sdl_surface = image;
   luaL_getmetatable(L, "gp2x.surface");
   lua_setmetatable(L, -2);
   return 1;
}

static int lua2x_display_image(lua_State* L) {
   const char* filename = luaL_checkstring(L, 1);
   lua_pushinteger(L, display_image(filename, 1));
   return 1;
}

static int lua2x_flip(lua_State* L) {
   SDL_Flip(screen);
   return 0;
}

static int lua2x_wait_for_any_key(lua_State* L) {
   lua_pushinteger(L, wait_for_any_key());
   return 1;
}

static Uint32 delay_timer(Uint32 interval, void* data) {
   SDL_Event event;
   event.type = SDL_USEREVENT;
   event.user.code = 1;
   event.user.data1 = NULL;
   event.user.data2 = NULL;
   SDL_PushEvent(&event);
   return interval;
}

static int button_state[GP2X_MAX_BUTTON_ID + 1];

static int lua2x_get_button(lua_State* L) {
   int button = lua_tointeger(L, 1);
   int result = button_state[button];
   lua_pushboolean(L, result);
   return 1;
}

// args are interval(ms)
// ,then callbacks for button(id),buttonup(id),mouseclicks(x,y),tick(nr)
// if a tick callback returns something loop will exit
static int lua2x_main_loop(lua_State* L) {
   int i;
   int tick = 1;
   SDL_Event event;
   int interval = lua_tointeger(L, 1);
   const char* button_callback_name = lua_tostring(L, 2);
   const char* buttonup_callback_name = lua_tostring(L, 3);
   const char* mouse_callback_name = lua_tostring(L, 4);
   const char* tick_callback_name = lua_tostring(L, 5);
   for (i = 0; i < GP2X_MAX_BUTTON_ID + 1; i++) {
      button_state[i] = 0;
   }
   SDL_AddTimer(interval, delay_timer, NULL);
   while (1) {
      int result;
      int button;
      SDL_WaitEvent(&event);
      switch (event.type) {
         case SDL_QUIT:
            return 0;
         case SDL_KEYDOWN:
            button = key_to_gp2x_button(event.key.keysym.sym);
            if (button >= 0) {
               button_state[button] = 1;
               lua_getglobal(L, button_callback_name);
               lua_pushinteger(L, button);
               lua_call(L, 1, 0);
            }
            break;
         case SDL_KEYUP:
            button = key_to_gp2x_button(event.key.keysym.sym);
            if (button >= 0) {
               button_state[button] = 0;
               lua_getglobal(L, buttonup_callback_name);
               lua_pushinteger(L, button);
               lua_call(L, 1, 0);
            }
            break;
         case SDL_JOYBUTTONDOWN:
            button = event.jbutton.button;
            button_state[button] = 1;
            lua_getglobal(L, button_callback_name);
            lua_pushinteger(L, button);
            lua_call(L, 1, 0);
            break;
         case SDL_JOYBUTTONUP:
            button = event.jbutton.button;
            button_state[button] = 0;
            lua_getglobal(L, buttonup_callback_name);
            lua_pushinteger(L, button);
            lua_call(L, 1, 0);
            break;
         case SDL_MOUSEBUTTONDOWN:
            lua_getglobal(L, mouse_callback_name);
            lua_pushinteger(L, event.button.x);
            lua_pushinteger(L, event.button.y);
            lua_call(L, 2, 0);
            break;
         case SDL_USEREVENT:
            lua_getglobal(L, tick_callback_name);
            lua_pushinteger(L, tick++);
            lua_call(L, 1, 1);
            result = lua_toboolean(L, -1);
            if (result) {
               return 0;
            }
            break;
      }
   }
}

static const struct luaL_reg lua2x_functions [] = {
   {"getscreen", lua2x_get_screen},
   {"loadpng", lua2x_load_png},
   {"flip", lua2x_flip},
   {"waitkey", lua2x_wait_for_any_key},
   {"rect", lua2x_rect_new},
   {"displayimage", lua2x_display_image},
   {"mainloop", lua2x_main_loop},
   {"getbutton", lua2x_get_button},
   {NULL, NULL}
};

static void lua2x_add_class(lua_State* L,
                            char* name,
                            const struct luaL_reg* methods)
{
   luaL_newmetatable(L, name);
   lua_pushstring(L, "__index");
   lua_pushvalue(L, -2);
   lua_settable(L, -3);
   luaL_openlib(L, NULL, methods, 0);
}

static void lua2x_add_constant(lua_State* L,
                               char* name,
                               int value)
{
   lua_getglobal(L, "gp2x");
   lua_pushstring(L, name);
   lua_pushinteger(L, value);
   lua_settable(L, -3);
}

int lua2x_run_file(char* filename,
                   lua_CFunction before,
                   lua_CFunction after)
{
   lua_State *L = lua_open();

   init_screen();

   L = lua_open();
   luaL_openlibs(L);
   lua2x_add_class(L, "gp2x.rect", lua2x_rect_methods);
   lua2x_add_class(L, "gp2x.surface", lua2x_surface_methods);
   luaL_openlib(L, "gp2x", lua2x_functions, 0);
   lua2x_add_constant(L, "UP", GP2X_UP);
   lua2x_add_constant(L, "DOWN", GP2X_DOWN);
   lua2x_add_constant(L, "LEFT", GP2X_LEFT);
   lua2x_add_constant(L, "RIGHT", GP2X_RIGHT);
   lua2x_add_constant(L, "A", GP2X_A);
   lua2x_add_constant(L, "B", GP2X_B);
   lua2x_add_constant(L, "X", GP2X_X);
   lua2x_add_constant(L, "Y", GP2X_Y);
   lua2x_add_constant(L, "L", GP2X_L);
   lua2x_add_constant(L, "R", GP2X_R);
   lua2x_add_constant(L, "VOLUP", GP2X_VOLUP);
   lua2x_add_constant(L, "VOLDOWN", GP2X_VOLDOWN);
   lua2x_add_constant(L, "SELECT", GP2X_SELECT);
   lua2x_add_constant(L, "START", GP2X_START);
   lua2x_add_constant(L, "STICKCLICK", GP2X_STICKCLICK);
   lua2x_add_constant(L, "UPLEFT", GP2X_UPLEFT);
   lua2x_add_constant(L, "DOWNLEFT", GP2X_DOWNLEFT);
   lua2x_add_constant(L, "UPRIGHT", GP2X_UPRIGHT);
   lua2x_add_constant(L, "DOWNRIGHT", GP2X_DOWNRIGHT);
   lua2x_add_constant(L, "QUIT", GP2X_QUIT );
   lua2x_add_constant(L, "NONE", GP2X_NONE );

   if (before) {
      // call before
   }

   if (luaL_loadfile(L, filename) || lua_pcall(L, 0, 0, 0))
      luaL_error(L, "liblua2x cannot run file: %s",
                lua_tostring(L, -1));

   if (after) {
      // call after
   }

   SDL_Quit();
#ifdef GP2X
    chdir("/usr/gp2x");
    execl("/usr/gp2x/gp2xmenu", "/usr/gp2x/gp2xmenu", NULL);
#endif
    return 0;

}


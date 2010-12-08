#include <stdio.h>
#include "SDL/SDL.h"
#include <stdlib.h>
#include <unistd.h>
#include "graphics.h"
#include "ui.h"

#ifdef GP2X
#include <lua5.1/lua.h>
#include <lua5.1/lauxlib.h>
#include <lua5.1/lualib.h>
#elif MACOSX
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#elif FREEBSD
#include <lua51/lua.h>
#include <lua51/lauxlib.h>
#include <lua51/lualib.h>
#else
#include <lua5.1/lua.h>
#include <lua5.1/lauxlib.h>
#include <lua5.1/lualib.h>
#endif

static int cfun(lua_State* L) {
   int i;
   FILE* f;
   lua_pushstring(L, "f");
   lua_gettable(L, lua_upvalueindex(1));
   f = (FILE*) lua_touserdata(L, -1);
   lua_pop(L, 1);
   for (i = 1; i <= lua_gettop(L); i++) {
      fprintf(f, "... argument %d: isnumber %d", i, lua_isnumber(L, i));
      fprintf(f, ", istable %d", lua_istable(L, i));
      fprintf(f, ", isfunction %d", lua_isfunction(L, i));
      fprintf(f, ", isstring %d", lua_isstring(L, i));
      if (lua_isnumber(L, i)) {
         fprintf(f, ", value %d", (int)lua_tointeger(L, i));
      } else if (lua_isstring(L, i)) {
         fprintf(f, ", value \"%s\"", lua_tostring(L, i));
      }
      fprintf(f, "\n");
   }
   lua_pushinteger(L, 21);
   return 1;
}

int main(int argc, char *argv[]) {
   FILE* f;
   SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO);
   SDL_JoystickOpen(0);
   init_screen();

   SDL_Color color;
   color.r = 0xcf;
   color.g = 0xff;
   color.b = 0xcf;
   clear_surface(screen, color);

   f = fopen("lua.txt", "w");
   lua_State *L = lua_open();
   fprintf(f, "Stack size is now: %d\n", lua_gettop(L));
   luaL_openlibs(L);
   fprintf(f, "Stack size is now: %d\n", lua_gettop(L));

   if (luaL_loadfile(L, "test.lua") || lua_pcall(L, 0, 0, 0))
      luaL_error(L, "cannot run configuration file: %s",
                lua_tostring(L, -1));
   lua_getglobal(L, "x");
   lua_getglobal(L, "y");

   lua_getglobal(L, "t");
   lua_pushstring(L, "foo");
   lua_gettable(L, -2);
   fprintf(f, "Got values x=%d, y=%d, and t[\"foo\"]=%s from test.lua.\n",
          (int)lua_tointeger(L, -4),
          (int)lua_tointeger(L, -3),
          lua_tostring(L, -1));
   fprintf(f, "Stack size is now: %d\n", lua_gettop(L));
   lua_pop(L, 4);
   fprintf(f, "Stack size after pop 4: %d\n", lua_gettop(L));

   lua_getglobal(L, "testf");
   lua_pushinteger(L, 7);
   lua_pushinteger(L, 6);
   lua_call(L, 2, 1);
   fprintf(f, "testf(7, 6) = %d\n", (int)lua_tointeger(L, -1));
   fprintf(f, "Stack size is now: %d\n", lua_gettop(L));
   lua_pop(L, 1);

   lua_getglobal(L, "testcf");
   lua_newtable(L);
   lua_pushlightuserdata(L, f);
   lua_setfield(L, -2, "f");
   lua_pushcclosure(L, cfun, 1);
   lua_call(L, 1, 1);
   fprintf(f, "Got %d back from call to testcf.\n",
           (int)lua_tointeger(L, -1));
   lua_pop(L, 1);

   fprintf(f, "Stack size is now: %d\n", lua_gettop(L));

   fclose(f);

   SDL_Flip(screen);
   wait_for_any_key();
   SDL_Quit();
#ifdef GP2X
    chdir("/usr/gp2x");
    execl("/usr/gp2x/gp2xmenu", "/usr/gp2x/gp2xmenu", NULL);
#endif
    return 0;

}



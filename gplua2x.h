#ifndef __LUA2X_H__
#define __LUA2X_H__

#ifdef GP2X
#include <lua5.1/lua.h>
#elif FREEBSD
#include <lua51/lua.h>
#elif MACOSX
#include <lua.h>
#else
#include <lua5.1/lua.h>
#endif

int lua2x_run_file(char* filename,
                   lua_CFunction before,
                   lua_CFunction after);

#endif

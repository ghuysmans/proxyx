#include "../config.h"
#include <unistd.h>
#include <string.h>
#include <error.h>
#include <stdio.h>
#ifdef HAVE_LUA
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#endif //HAVE_LUA

int say(lua_State *L) {
	luaL_checkstring(L, 1);
	printf("%s\n", lua_tostring(L, 1));
	return 0;
}

int main(int argc, char *argv[]) {
	int o, verbose=0, e=0;
	while ((o=getopt(argc, argv, "p:v")) != -1) {
		switch (o) {
			case 'v':
				verbose++;
				//break;
		}
	}
	if (argv[optind]) {
#ifdef HAVE_LUA
		lua_State *L = luaL_newstate();
		luaL_openlibs(L);
		lua_newtable(L);
		lua_pushnumber(L, 0);
		lua_pushnumber(L, 42);
		lua_rawset(L, -3);
		lua_pushnumber(L, 1);
		lua_pushnumber(L, 3141592);
		lua_rawset(L, -3);
		lua_setglobal(L, "t");
		//
		lua_newtable(L);
			lua_pushstring(L, "headers");
			lua_newtable(L);
					lua_pushstring(L, "Host");
					lua_pushstring(L, "mars.edu");
				lua_rawset(L, -3);
					lua_pushstring(L, "Content-Type");
					lua_pushstring(L, "text/html");
				lua_rawset(L, -3);
			lua_rawset(L, -3);
				lua_pushstring(L, "say");
				lua_pushcfunction(L, say);
			lua_rawset(L, -3);
		lua_setglobal(L, "http");
		if (e = luaL_loadfile(L, argv[1]) ||
				lua_pcall(L, 0, LUA_MULTRET, 0))
			fprintf(stderr, "%s\n", lua_tostring(L, -1));
		else {
			printf("returned %f\n", lua_tonumber(L, -1));
			lua_pop(L, 1);
		}
		lua_close(L);
#else
		e = 1;
		fprintf(stderr, "No Lua support!\n");
#endif //HAVE_LUA
	}
	else
		printf("Usage: %s script\n", argv[0]);
	return e;
}

#include <unistd.h>
#include <string.h>
#include <error.h>
#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

int main(int argc, char *argv[]) {
	int o, verbose=0;
	while ((o=getopt(argc, argv, "p:v")) != -1) {
		switch (o) {
			case 'v':
				verbose++;
				//break;
		}
	}
	if (argv[optind]) {
		lua_State *L = luaL_newstate();
		int e;
		luaL_openlibs(L);
		lua_newtable(L);
		lua_pushnumber(L, 0);
		lua_pushnumber(L, 42);
		lua_rawset(L, -3);
		lua_pushnumber(L, 1);
		lua_pushnumber(L, 3141592);
		lua_rawset(L, -3);
		lua_setglobal(L, "t");
		if (e = luaL_loadfile(L, argv[1]) ||
				lua_pcall(L, 0, LUA_MULTRET, 0))
			fprintf(stderr, "%s\n", lua_tostring(L, -1));
		else {
			printf("returned %f\n", lua_tonumber(L, -1));
			lua_pop(L, 1);
		}
		lua_close(L);
		return e;
	}
	else {
		printf("Usage: %s script\n", argv[0]);
		return 0;
	}
}

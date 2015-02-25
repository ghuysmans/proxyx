CFLAGS?=-Wall
PLATFORM?=linux
LUA_DIR?=lua-5.3.0/src
LUA_GCC=-I${LUA_DIR} -L${LUA_DIR} -llua -lm

ifndef RELEASE
	CC+=-g
endif
ifndef VERBOSE
	#Inspired from the Linux Kernel Makefile
	Q=@
endif

all: reso luatest

reso: reso.c
	${Q}${CC} ${CFLAGS} -o reso reso.c
luatest: lua
	${Q}${CC} ${CFLAGS} -o luatest luatest.c ${LUA_GCC}

clean:
	rm reso luatest
	${Q}$(MAKE) -C ${LUA_DIR} clean

lua:
	${Q}$(MAKE) -C ${LUA_DIR} ${PLATFORM}
.PHONY: lua

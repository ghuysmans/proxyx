cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules")

project(proxyx)
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)

#Lua optional support
find_package(Lua52)
if(LUA52_FOUND)
	include_directories(${LUA_INCLUDE_DIR})
	link_libraries(${LUA_LIBRARIES})
endif(LUA52_FOUND)

#config header
get_directory_property(COMPILE_DEFINITIONS COMPILE_DEFINITIONS)
configure_file("${PROJECT_SOURCE_DIR}/config.h.in"
	"${PROJECT_BINARY_DIR}/config.h")

add_executable(luatest luatest.c)
add_executable(reso reso.c)

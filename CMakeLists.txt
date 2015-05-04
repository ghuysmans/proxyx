cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/modules")

project(proxyx)
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

#Lua optional support
find_package(Lua52)
if(LUA52_FOUND)
	include_directories(${LUA_INCLUDE_DIR})
	link_libraries(${LUA_LIBRARIES})
endif(LUA52_FOUND)

#OpenSSL
find_package(OpenSSL)
if(OPENSSL_FOUND)
	include_directories(${OPENSSL_INCLUDE_DIR})
	link_libraries(${OPENSSL_LIBRARIES})
else()
	message(FATAL_ERROR "OpenSSL is required")
endif(OPENSSL_FOUND)

#config header
get_directory_property(COMPILE_DEFINITIONS COMPILE_DEFINITIONS)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/config.h")

#Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
	#adapted from https://www.tty1.net/blog/2014/cmake-doxygen_en.html
	set(doxyfile_in "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in")
	set(doxyfile "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
	configure_file("${doxyfile_in}" "${doxyfile}")
	add_custom_target(doc
		COMMAND "${DOXYGEN_EXECUTABLE}" "${doxyfile}"
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
		COMMENT "Generating API documentation with Doxygen"
		VERBATIM)
endif(DOXYGEN_FOUND)

#unit tests
include(tests/tests.cmake)

add_executable(luatest poc/luatest.c)
add_executable(proxyx src/proxyx.c src/http_headers.c src/http.c src/buffer.c src/socket.c src/cache.c)

#inspired from http://www.cmake.org/pipermail/cmake/2009-July/030788.html

macro(expect test cmd args)
	add_test(${test}
		${CMAKE_COMMAND}
		-Dcmd=${cmd}
		-Dargs:string=${args}
		-Dresult=tests/${test}.res
		-Dexpected=tests/${test}.exp
		-P tests/do_test.cmake)
endmacro(expect)

if (UNIX)
	enable_testing()
	#CuTest
	file(GLOB UnitTests tests/U*.c)
	execute_process(
		COMMAND "tests/make-tests.sh" ${UnitTests}
		OUTPUT_FILE "tests/UAll.c")
	add_executable(UAll ${UnitTests} tests/CuTest.c src/http_headers.c)
	add_test(UAll UAll)
	#Other tests
	add_executable(buffer tests/buffer.c src/buffer.c src/socket.c)
	expect(buffer/a tests/buffer.sh a)
	expect(buffer/b tests/buffer.sh b)
	expect(buffer/c tests/buffer.sh c)
	add_executable(fetch tests/fetch.c src/buffer.c src/http_headers.c src/http.c)
	expect(fetch/a ./fetch tests/fetch/a)
	expect(fetch/b ./fetch tests/fetch/b)
	expect(fetch/c ./fetch tests/fetch/c)
	expect(fetch/d ./fetch tests/fetch/d)
endif (UNIX)

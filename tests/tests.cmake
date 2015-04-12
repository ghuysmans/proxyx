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
	expect(dummy_dir ls "TODO")
	#unit tests
	file(GLOB UnitTests tests/U*.c)
	execute_process(
		COMMAND "tests/make-tests.sh" ${UnitTests}
		OUTPUT_FILE "tests/UAll.c")
	add_executable(UAll ${UnitTests} tests/CuTest.c src/http_headers.c)
	add_test(UAll UAll)
endif (UNIX)

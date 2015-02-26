#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "CuTest.h"

void TestPotato2(CuTest* tc)
{
	char* s = "coucou";
	CuAssertStrEquals(tc, "coucou", s);
}
void TestPotato(CuTest* tc)
{
	char* s = "coucoux";
	CuAssertStrEquals(tc, "coucou", s);
}

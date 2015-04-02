#include <stdio.h>
#include "CuTest.h"

void TestPotato(CuTest* tc)
{
	char* s = "coucou";
	CuAssertStrEquals(tc, "coucou", s);
}

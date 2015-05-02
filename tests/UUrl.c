#include <stdlib.h>
#include <string.h>
#include "CuTest.h"

void AssertPartsEquals(CuTest *tc, char *url, char *host, char *service) {
	char *h, *s;
	char *u=strdup(url);
	char *m=malloc(strlen(url)+20), *p=m+strlen(url);
	get_host_parts(u, &h, &s);
	strcpy(m, url);
	strcpy(p, " host");
	CuAssertStrEquals_Msg(tc, m, host, h);
	strcpy(p, " service");
	CuAssertStrEquals_Msg(tc, m, service, s);
	free(m);
	free(u);
}

void TestDnsUrl(CuTest *tc) {
	AssertPartsEquals(tc, "a.com:port", "a.com", "port");
	AssertPartsEquals(tc, "a.com", "a.com", "http");
}

void TestUrl6(CuTest *tc) {
	AssertPartsEquals(tc, "[1:2]:port", "[1:2]", "port");
	AssertPartsEquals(tc, "[1:2]", "[1:2]", "http");
}

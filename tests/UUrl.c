#include <stdlib.h>
#include <string.h>
#include "CuTest.h"

void AssertPartsEquals(CuTest *tc, char *url, char *host, char *service, char *resource) {
	char *h, *s, *r;
	char *u=strdup(url);
	char *m=malloc(strlen(url)+20), *p=m+strlen(url);
	int e=get_host_parts(u, &h, &s, &r);
	strcpy(m, url);
	strcpy(p, " return");
	CuAssertIntEquals_Msg(tc, m, 0, e);
	if (!e) {
		strcpy(p, " host");
		CuAssertStrEquals_Msg(tc, m, host, h);
		strcpy(p, " service");
		CuAssertStrEquals_Msg(tc, m, service, s);
		strcpy(p, " resource");
		CuAssertStrEquals_Msg(tc, m, resource, r);
	}
	free(m);
	free(u);
}

void TestDnsUrl(CuTest *tc) {
	AssertPartsEquals(tc, "http://a.com/t/p:a", "a.com", "http", "t/p:a");
	AssertPartsEquals(tc, "http://a.com:port/t/p:a", "a.com", "port", "t/p:a");
	AssertPartsEquals(tc, "http://a.com:port", "a.com", "port", "");
	AssertPartsEquals(tc, "http://a.com", "a.com", "http", "");
	AssertPartsEquals(tc, "http://a.com/", "a.com", "http", "");
}

void TestUrl6(CuTest *tc) {
	AssertPartsEquals(tc, "http://[1:2]/t/p:a", "[1:2]", "http", "t/p:a");
	AssertPartsEquals(tc, "http://[1:2]:port/t/p:a", "[1:2]", "port", "t/p:a");
	AssertPartsEquals(tc, "http://[1:2]:port", "[1:2]", "port", "");
	AssertPartsEquals(tc, "http://[1:2]", "[1:2]", "http", "");
	AssertPartsEquals(tc, "http://[1:2]/", "[1:2]", "http", "");
}

#include <stdlib.h>
#include <string.h>
#include "CuTest.h"
#include "http_headers.h"

void AssertHeadersContain(CuTest *tc, HTTP_HEADER *h, char *name, char *value) {
	while (h) {
		if (strcmp(h->field_name, name))
			h = h->next;
		else {
			CuAssertStrEquals(tc, h->field_value, value);
			return;
		}
	}
	CuFail(tc, name);
}

void TestHeaders(CuTest *tc) {
	char *s = strdup("a: b\r\nc: 42: nope\r\n");
	HTTP_HEADER *headers = parse_http_headers(s, NULL);
	AssertHeadersContain(tc, headers, "a", "b");
	AssertHeadersContain(tc, headers, "c", "42: nope");
	free_http_headers(headers);
	free(s);
}

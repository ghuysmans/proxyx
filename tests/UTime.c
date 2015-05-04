#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CuTest.h"
#include "http_headers.h"
void Test1123(CuTest *tc){
	struct tm tm;
	char* testrfc1123="Sun, 07 Jan 2001 12:33:45 GMT";
	char* converted;
	from_http_date(testrfc1123, &tm);
	converted = to_http_date(&tm);
	CuAssertStrEquals_Msg(tc, "RFC1123=>RFC1123", testrfc1123, converted);
}

void Test850(CuTest *tc){
	struct tm tm;
	char* testrfc850="Sunday, 07-Jan-01 12:33:45 GMT";
	char* testrfc1123="Sun, 07 Jan 2001 12:33:45 GMT";
	char* converted;
	from_http_date(testrfc850, &tm);
	converted = to_http_date(&tm);
	CuAssertStrEquals_Msg(tc, "RFC850=>RFC1123", testrfc1123, converted);
}

void TestAsctime(CuTest *tc){
	struct tm tm;
	char* testrfc1123="Sun, 07 Jan 2001 12:33:45 GMT";
	char* testasctime="Sun Jan 07 12:33:45 2001";
	char* converted;
	from_http_date(testasctime, &tm);
	converted = to_http_date(&tm);
	CuAssertStrEquals_Msg(tc, "Asctime=>RFC1123", testrfc1123, converted);
}

void TestUnknown(CuTest *tc){
	struct tm tm;
	char* testunknown="This is definitely not a valid date";
	int res = from_http_date(testunknown, &tm);
	CuAssertIntEquals_Msg(tc, "Unknown=>TM", 1, res);
}

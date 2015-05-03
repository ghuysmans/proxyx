#include <time.h>
#include <stdio.h>

struct tm tm;

main(){
	//char* date="6 Jan 2001 12:33:45";

	char* testrfc850="Sunday, 07-Jan-01 12:33:45 GMT";
	char* testrfc1123="Sun, 07 Jan 2001 12:33:45 GMT";
	char* testasctime="Sun Jan 07 12:33:45 2001";
	char converted[64];

	//FORMATS
	char* rfc850="%A, %d-%b-%y %H:%M:%S GMT";
	char* rfc1123="%a, %d %b %Y %H:%M:%S GMT";
	char* asctime="%a %b %d %H:%M:%S %Y";

	if (strptime(testasctime, rfc850, &tm) == NULL)
	    printf("Date reading Error");

	printf("Original Date : %s\r\n", testrfc850);
	printf("Year: %d; Month: %d; Day: %d;\n",
		tm.tm_year, tm.tm_mon, tm.tm_mday);
	printf("Hour: %d; Minute: %d; Second: %d\n",
		tm.tm_hour, tm.tm_min, tm.tm_sec);
	printf("Week day: %d; Year day: %d\n\n", tm.tm_wday, tm.tm_yday);

	if (strftime(converted, sizeof(converted), asctime, &tm) == NULL)
	    printf("Conversion Error");

	printf("Assumed Date : %s\r\n", testasctime);
	printf("Converted Date : %s\r\n", converted);
}

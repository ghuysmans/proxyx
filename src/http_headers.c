/**
 * @file
 * RFC 2616 headers encoding/decoding library
 */
#include "http_headers.h"

/* 4.2
 * [...] each header field consists of a name followed by a colon (":") and
 * the field value. Field names are case-insensitive. The field value
 * MAY be preceded by any amount of LWS, though a single SP is preferred.
 * Header fields can be extended over multiple lines by preceding
 * each extra line with at least one SP or HT. Applications ought to
 * follow "common form", where one is known or indicated,
 * when generating HTTP constructs, since there might exist
 * some implementations that fail to accept anything. */

/**
 * Linked list node representing an HTTP header
 */
typedef struct _HTTP_HEADER {
	char *field_name; ///<never NULL
	char *field_value; ///<NULL if empty FIXME?
	struct _HTTP_HEADER *next; ///<NULL indicates this is the last node
} HTTP_HEADER;

/**
 * Parses HTTP headers until CRLF CRLF.
 * @return NULL if raw doesn't point to a field name
 */
HTTP_HEADER *parse_http_headers(const char *raw /**<raw headers*/);

/**
 * Frees memory allocated for a given structure.
 */
void free_http_headers(const HTTP_HEADER *h /**<first node*/);

/**
 * Finds the value associated to a field name.
 * @returns NULL if the field doesn't exist
 */
char *find_http_header(const HTTP_HEADER *h /**<first node*/,
		const char *name /**<field name*/);

/**
 * Adds a header field to the given structure.
 * @return NULL if memory couldn't be allocated
 */
HTTP_HEADER *add_http_header(const HTTP_HEADER *h /**<first node*/,
		const char *name /**<field name*/,
		const char *value /**<field value*/);

/* 3.3
 * All HTTP date/time stamps MUST be represented in Greenwich Mean Time (GMT),
 * without exception. For the purposes of HTTP, GMT is exactly equal to UTC
 * (Coordinated Universal Time). This is indicated in the first two formats
 * by the inclusion of "GMT" as the three-letter abbreviation for time zone,
 * and MUST be assumed when reading the asctime format. HTTP-date is
 * case sensitive and MUST NOT include additional LWS beyond that specifically
 * included as SP in the grammar.
 * HTTP-date    = rfc1123-date | rfc850-date | asctime-date
 * rfc1123-date = wkday "," SP date1 SP time SP "GMT"
 * rfc850-date  = weekday "," SP date2 SP time SP "GMT"
 * asctime-date = wkday SP date3 SP time SP 4DIGIT
 * date1        = 2DIGIT SP month SP 4DIGIT //dmy, e.g. 02 Jun 1982
 * date2        = 2DIGIT "-" month "-" 2DIGIT //d-m-y, e.g. 02-Jun-82
 * date3        = month SP ( 2DIGIT | ( SP 1DIGIT )) //md, e.g. Jun  2
 * time         = 2DIGIT ":" 2DIGIT ":" 2DIGIT //00:00:00 - 23:59:59
 * wkday        = "Mon" | "Tue" | "Wed" | "Thu" | "Fri" | "Sat" | "Sun"
 * weekday      = "Monday" | "Tuesday" | "Wednesday" | "Thursday" | 
 * 					"Friday" | "Saturday" | "Sunday"
 * month        = "Jan" | "Feb" | "Mar" | "Apr"
					| "May" | "Jun" | "Jul" | "Aug"
					| "Sep" | "Oct" | "Nov" | "Dec"
 */

/**
 * Reads a date complying with either RFC 1123, RFC 850 or asctime's format.
 * @return 0 if successfull
 */
int from_http_date(const char *raw /**<raw date string*/,
		struct tm *d /**<decoded date*/);

/**
 * Converts a date to the preferred HTTP date format (RFC 1123).
 * @return NULL if memory couldn't be allocated
 */
char *to_http_date(const struct tm *d /**<target*/);

//use gmtime(), mktime()

/* FIXME find doc */

/**
 * Extracts the host and service from the HTTP Host header field.
 * @return 0 if successfull
 * FIXME consistency with "success"
 */
int get_host_parts(const char *raw /**<HTTP Host header field*/,
		char **host /**<Host name copy, to be freed*/,
		char **service /**<Service (in the raw buffer)*/);

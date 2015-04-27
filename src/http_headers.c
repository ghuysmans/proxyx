/**
 * @file
 * RFC 2616 headers encoding/decoding library
 */
#include "http_headers.h"
#include <string.h>
#include <stdlib.h>

/* 4.2
 * [...] each header field consists of a name followed by a colon (":") and
 * the field value. Field names are case-insensitive. The field value
 * MAY be preceded by any amount of LWS, though a single SP is preferred.
 * Header fields can be extended over multiple lines by preceding
 * each extra line with at least one SP or HT. Applications ought to
 * follow "common form", where one is known or indicated,
 * when generating HTTP constructs, since there might exist
 * some implementations that fail to accept anything. */

#ifndef _HTTP_HEADER_STRUCT
#define _HTTP_HEADER_STRUCT
/**
 * Linked list node representing an HTTP header
 */
typedef struct _HTTP_HEADER {
	char *field_name; ///<never NULL
	char *field_value; ///<may point to an empty string
	struct _HTTP_HEADER *next; ///<NULL indicates this is the last node
} HTTP_HEADER;
#endif //_HTTP_HEADER_STRUCT


/**
 * Parses one HTTP header per line until NUL.
 * Each line must end with \r\n,
 * these characters musn't be repeated at the end.
 * @return NULL if raw doesn't point to a field name
 * @note NUL characters will replace ':' and '\r'.
 */
HTTP_HEADER *parse_http_headers(char *raw /**<raw headers*/, HTTP_HEADER *nx) {
	if (*raw) {
		HTTP_HEADER *h = malloc(sizeof(HTTP_HEADER));
		char *end;
		if (!h) {
			free_http_headers(nx);
			return NULL;
		}
		h->field_name = raw;
		h->field_value = strchr(raw, ':');
		if (!h->field_value) {
			free_http_headers(nx);
			free(h);
			return NULL;
		}
		*(h->field_value++) = 0;
		while (isblank(*h->field_value))
			h->field_value++;
		end = strstr(h->field_value, "\r\n");
		*end = 0;
		//TODO handle LWS in order to be RFC-compliant
		h->next = nx;
		return parse_http_headers(end+2, h);
	}
	else
		return nx;
}

/**
 * Frees memory allocated for a given structure.
 */
void free_http_headers(const HTTP_HEADER *h /**<first node*/) {
	while (h) {
		HTTP_HEADER *tmp = h->next;
		free((void*)h);
		h = tmp;
	}
}

/**
 * Finds the value associated to a field name.
 * @returns NULL if the field doesn't exist
 */
char *find_http_header(const HTTP_HEADER *h /**<first node*/,
		const char *name /**<field name*/) {
	while (h) {
		if (strcmp(h->field_name, name))
			h = h->next;
		else
			return h->field_value;
	}
	return NULL; //not found
}

/**
 * Adds a header field to the given list (inserting at the head).
 * @return NULL if memory couldn't be allocated, otherwise the new first node.
 */
HTTP_HEADER *add_http_header(const HTTP_HEADER *h /**<first node*/,
		const char *name /**<field name*/,
		const char *value /**<field value*/) {
	HTTP_HEADER* nw = malloc(sizeof(HTTP_HEADER));
	if (nw) {
		nw->field_name = (char*)name;
		nw->field_value = (char*)value;
		nw->next = (HTTP_HEADER*)h;
	}
	return nw;
}

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
 * @return 0 if successful
 * @note no data is copied
 */
int get_host_parts(const char *field_value /**<HTTP Host header field*/,
		char **host /**<Host name*/,
		char **service /**<Service*/,
		char **resource /**<Resource identifier without leading slash*/) {
	char *colon;
	*host = (char*)field_value;
	if (**host == '[') {
		colon = strstr(*host, "]:");
		if (colon)
			colon++;
	}
	else
		colon = strchr(*host, ':');
	*resource = strchr(*host, '/');
	if (colon && (colon<*resource || !*resource)) {
		*colon = 0; //terminate *host
		*service = colon+1;
	}
	else
		*service = "http";
	if (*resource) {
		**resource = 0; //terminate *host or *service
		(*resource)++;
	}
	else
		*resource = "";
	return 0;
}

#include <openssl/md5.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	unsigned char hash[MD5_DIGEST_LENGTH];
	if (argv[1]) {
		int i;
		MD5(argv[1], strlen(argv[1]), hash);
		for (i=0; i<MD5_DIGEST_LENGTH; i++)
			printf("%02x", hash[i]);
		printf("\n");
	}
	else
		printf("usage: %s message\n", argv[0]);
	return 0;
}

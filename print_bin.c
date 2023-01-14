#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "print_bin.h"
#include "bitmap.h"
#include "base.h"

int printUlongBinary(unsigned long *val, int len)
{
	unsigned long pos;

	if(val == NULL || len < 0) {
		printf("error null point or len error!!! len:%d\n", len);
		return -1;
	}

	printf("pos: ");
	for (pos = 0; pos < len; pos++) {
		if (pos > 32)
			break;
		printf("%-2ld ", pos);
	}
	printf("\n");

	printf("bit: ");
	for (pos = 0; pos < len; pos++) {
		if (pos > 32)
			break;
		printf("%-2ld ", val[pos]);
	}
	printf("\n");

	if (len > 32) {
		printf("pos: ");
		for (pos = 33; pos < len; pos++) {
			printf("%-2ld ", pos);
		}
		printf("\n");

		printf("bit: ");
		for (pos = 33; pos < len; pos++) {
			printf("%-2ld ", val[pos]);
		}
		printf("\n");
	}


	return 0;
}

int printIntBinary(int *val, int len)
{
	int pos;

	if(val == NULL || len < 0) {
		printf("error null point or len error!!! len:%d\n", len);
		return -1;
	}

	printf("pos: ");
	for (pos = 0; pos < len; pos++) {
		if (pos > 32)
			break;
		printf("%-2d ", pos);
	}
	printf("\n");

	printf("bit: ");
	for (pos = 0; pos < len; pos++) {
		if (pos > 32)
			break;
		printf("%-2d ", val[pos]);
	}
	printf("\n");

	if (len > 32) {
		printf("pos: ");
		for (pos = 33; pos < len; pos++) {
			printf("%-2d ", pos);
		}
		printf("\n");

		printf("bit: ");
		for (pos = 33; pos < len; pos++) {
			printf("%-2d ", val[pos]);
		}
		printf("\n");
	}

	return 0;
}

int covert_hex_to_binary(unsigned long *bitmap, int *val, unsigned int len)
{
	unsigned long pos;

	for_each_clear_bit(pos, bitmap, len)
		val[pos] = 0;
	for_each_set_bit(pos, bitmap, len)
		val[pos] = 1;

	return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

/* bitmap header */
#include "bitmap.h"
#include "base.h"
#include "print_bin.h"

struct convert {
	long param1;
	long param2;
	int param1_len;
	int param2_len;
	bool need_covert;
	bool debug;
};

struct convert convert;

#define GETVALE(pval, src, type) \
({                             \
	int __ret = 0;          \
	(type *)pval = (type *)src; \
	__ret;              \
})


int start_convert(void *src, void *dst, int len)
{
	int *val = (int *)dst;
	
	struct convert *from = (struct convert *)src;

	if (convert.debug == true) {
		printf("conver before---->\n");
		print_int_binary(val, len);
	}

	printf("\nStart to convert.\n");
	covert_hex_to_binary((unsigned long*)(&(from->param1)), val, len);

	if (convert.debug == true) {
		printf("convert after---->\n");
		print_int_binary(val, len);
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int i, ret;
	int *buf1, buf1_len;

	convert.debug = false;
    switch (argc) {
	case 1:
		printf("Please use: ./convert_hex 0x4 32 \n");
		exit(1);
	case 3:
		convert.param1 = strtoul(argv[1], 0, 0);
		convert.param1_len = strtoul(argv[2], 0, 0);
		convert.debug = true;
		printf("param1=%#lx param1_len=%d\n", convert.param1, convert.param1_len );
		break;
	case 4:
		convert.param1 = strtoul(argv[1], 0, 0);
		convert.param1_len = strtoul(argv[2], 0, 0);
		convert.debug = (bool)strtoul(argv[2], 0, 0);
		printf("param1=%#lx param1_len=%d debug=%d\n", convert.param1, convert.param1_len, convert.debug);
		break;
	default:
		printf("ERROR input must be one or three params!!! \n");
		exit(1);
    }

	buf1_len = convert.param1_len;
	buf1 = (int *)malloc(sizeof(int)*buf1_len);
	if (!buf1) {
		perror(NULL);
		exit(1);
	}
	memset(buf1, 0, buf1_len);

	start_convert(&convert.param1, buf1, buf1_len);

	if(buf1)
		free(buf1);

	return 0;
}
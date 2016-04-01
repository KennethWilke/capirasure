#include "libcapirasure.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define TEST_DATA "1234567812345"

void print_data(char *prefix, void *data, size_t size)
{
	int i = 0;
	char *ptr = data;

	printf("%s", prefix);

	while (i < size)
	{
		if (i && !(i % 16))
		{
			printf("\n");
		}
		printf("%02hhx", ptr[i]);
		i++;
	}

	printf("\n");
}


int main()
{
	capi_handle *afu = capi_init();
	struct timespec sleep_time = {0, 1000};
	int us = 0;
	if (!afu)
	{
		return 1;
	}

	char *data = TEST_DATA;
	erasure_wed *wed = erasure_encode_request(data, sizeof(TEST_DATA) - 1);

	print_data("[s0]\n", wed->source0, wed->buffer_size);
	print_data("[s1]\n", wed->source1, wed->buffer_size);
	print_data("[s2]\n", wed->source2, wed->buffer_size);
	print_data("[s3]\n", wed->source3, wed->buffer_size);
	print_data("[s4]\n", wed->source4, wed->buffer_size);
	print_data("[s5]\n", wed->source5, wed->buffer_size);
	print_data("[s6]\n", wed->source6, wed->buffer_size);
	print_data("[s7]\n", wed->source7, wed->buffer_size);
	capi_attach(afu, wed);

	cxl_mmio_write64(afu, 0, (__u64)wed);
	while (!wed->status)
	{
		us += 1;
		nanosleep(&sleep_time, NULL);
	}

	printf("Encoded in %dμs\n", us);
	print_data("[r0]\n", wed->result0, wed->buffer_size);
	print_data("[r1]\n", wed->result1, wed->buffer_size);
	print_data("[r2]\n", wed->result2, wed->buffer_size);
	print_data("[r3]\n", wed->result3, wed->buffer_size);


	capi_free(afu);
	erasure_wed_free(wed);

	return 0;
}

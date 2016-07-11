#include "erasure.h"


int erasure_init(void)
{
	afu = capi_init();
	if (!afu)
	{
		fprintf(stderr, "Failed to open CAPI device at " DEVICE_PATH "\n");
		return 0;
	}

	return 1;
}


void erasure_cleanup(void)
{
	capi_free(afu);
}


int erasure_process_request(erasure_wed *wed)
{
	struct timespec sleep_time = {1, 0};

	capi_attach(afu, wed);
	cxl_mmio_write64(afu, 0, (__u64)wed);

	while (!wed->status)
	{
		printf("waiting...\n");
		nanosleep(&sleep_time, NULL);
	}

	return wed->status;
}


erasure_wed* erasure_wed_new(void)
{
	erasure_wed *new = aligned_alloc(WED_ALIGNMENT, sizeof(*new));
	if (!new)
	{
		fprintf(stderr, "Failed to allocate erasure WED structure\n");
		return NULL;
	}

	// Zero out states and padding
	new->status = 0;
	new->job_counter = 0;
	new->return_size = 0;
	new->pad1 = 0;
	new->pad2 = 0;
	new->pad3 = 0;

	// Use default block size
	new->block_size = BLOCK_SIZE;
	new->buffer_size = 0;

	// Set param to encode Reed-Solomon 8:4
	new->param_s0 = 0;
	new->param_s1 = RS_8_4;

	new->source0 = NULL;
	new->source1 = NULL;
	new->source2 = NULL;
	new->source3 = NULL;
	new->source4 = NULL;
	new->source5 = NULL;
	new->source6 = NULL;
	new->source7 = NULL;
	new->result0 = NULL;
	new->result1 = NULL;
	new->result2 = NULL;
	new->result3 = NULL;
	return new;
}


int erasure_allocate_buffers(erasure_wed *wed)
{
	uint8_t **struct_member;
	for (struct_member = &(wed->source0);
	     struct_member <= &(wed->result3);
	     struct_member++)
	{
		*struct_member = aligned_alloc(DATA_ALIGNMENT, wed->buffer_size);
		if (!*struct_member)
		{
			fprintf(stderr, "[ERROR] Failed to allocate erasure buffers\n");
			return 0;
		}
	}
	return 1;
}


void erasure_free_buffers(erasure_wed *wed)
{
	uint8_t **struct_member;
	for (struct_member = &(wed->source0);
	     struct_member <= &(wed->result3);
	     struct_member++)
	{
		if (*struct_member)
		{
			free(*struct_member);
			*struct_member = NULL;
		}
	}
}


erasure_wed* erasure_encode_request(char *data, ssize_t len)
{
	uint8_t **struct_member;
	ssize_t overall_size, copy_offset = 0;
	erasure_wed *new = erasure_wed_new();
	if (!new)
	{
		return NULL;
	}

	if (!len || !data)
	{
		return NULL;
	}

	fprintf(stderr, "[INFO] data at %p is %zd bytes\n", data, len);

	new->buffer_size = len / 128;
	if (len % 128)
	{
		new->buffer_size++;
	}

	overall_size = new->buffer_size * 128;

	fprintf(stderr, "[INFO] buffer size: %d\n", new->buffer_size);
	fprintf(stderr, "[INFO] overall size: %zd\n", overall_size);
	erasure_allocate_buffers(new);

	struct_member = &(new->source0);

	while (copy_offset < overall_size)
	{
		fprintf(stderr, "ptr: %p\n", struct_member);
		fprintf(stderr, "[INFO] %zd more to write\n", len - copy_offset);
		fprintf(stderr, "[INFO] copy_offset: %zd\n", copy_offset);
		if (len - copy_offset >= new->buffer_size)
		{
			fprintf(stderr, "%zd >= %d\n", len - copy_offset, new->buffer_size);
			fprintf(stderr, "[INFO] data fits in this buffer\n");
			memcpy(*struct_member, data + copy_offset, new->buffer_size);
			copy_offset += new->buffer_size;
			struct_member++;
		}
		else if (copy_offset >= len)
		{
			bzero(*struct_member, new->buffer_size);
			copy_offset += new->buffer_size;
			struct_member++;
		}
		else
		{
			ssize_t partial = len - copy_offset;
			bzero(*struct_member, new->buffer_size);
			memcpy(*struct_member, data + copy_offset, partial);
			copy_offset += new->buffer_size;
			struct_member++;
		}
	}

	return new;
}

void erasure_wed_free(erasure_wed *wed)
{
	erasure_free_buffers(wed);
	free(wed);
}


void erasure_wed_print(erasure_wed *wed)
{
	if (!wed)
	{
		printf("[wed: (null)]\n");
	}
	else
	{
		printf("[wed: %p]\n", wed);
		printf(" status: %hu\n", wed->status);
		printf(" job_counter: %hu\n", wed->job_counter);
		printf(" return_size: %u\n", wed->return_size);
		printf(" block_size: %u\n", wed->block_size);
		printf(" buffer_size: %u\n", wed->buffer_size);
		printf(" param_s0: %hx\n", wed->param_s0);
		printf(" param_s1: %hx\n", wed->param_s1);
		printf(" pad1: %u\n", wed->pad1);
		printf(" pad2: %u\n", wed->pad2);
		printf(" pad3: %u\n", wed->pad3);
		printf(" source0: %p\n", wed->source0);
		printf(" source1: %p\n", wed->source1);
		printf(" source2: %p\n", wed->source2);
		printf(" source3: %p\n", wed->source3);
		printf(" source4: %p\n", wed->source4);
		printf(" source5: %p\n", wed->source5);
		printf(" source6: %p\n", wed->source6);
		printf(" source7: %p\n", wed->source7);
		printf(" result0: %p\n", wed->result0);
		printf(" result1: %p\n", wed->result1);
		printf(" result2: %p\n", wed->result2);
		printf(" result3: %p\n", wed->result3);
		printf("-----------------------------------\n");
	}
}

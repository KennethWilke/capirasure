#ifndef __ERASURE_H__
#define __ERASURE_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "capi.h"

#define BLOCK_SIZE 8192
#define WED_ALIGNMENT 128
#define DATA_ALIGNMENT 8

#define RS_8_4 0x0703

typedef struct
{
	uint16_t volatile status;
	uint16_t volatile job_counter;
	uint32_t volatile return_size;
	uint32_t block_size;
	uint32_t buffer_size;
	uint16_t param_s0;
	uint16_t param_s1;
	uint32_t pad1;
	uint32_t pad2;
	uint32_t pad3;
	uint8_t *source0;
	uint8_t *source1;
	uint8_t *source2;
	uint8_t *source3;
	uint8_t *source4;
	uint8_t *source5;
	uint8_t *source6;
	uint8_t *source7;
	uint8_t *result0;
	uint8_t *result1;
	uint8_t *result2;
	uint8_t *result3;
} erasure_wed;

capi_handle *afu;


int erasure_init(void);
erasure_wed* erasure_wed_new(void);
int erasure_allocte_buffers(erasure_wed *wed);
void erasure_free_buffers(erasure_wed *wed);
erasure_wed* erasure_encode_request(char *data, ssize_t len);
int erasure_process_request(erasure_wed *wed);
void erasure_wed_free(erasure_wed *wed);
void erasure_cleanup(void);

#endif

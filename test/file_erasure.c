#include "libcapirasure.h"
#include <stdio.h>
#include <unistd.h>

typedef struct
{
	void *data;
	size_t length;
} buffer;

int parse_args(int argc, char *argv[]);
buffer * buffer_from_file(char *filename);
void buffer_free(buffer *target);

char *target_file = NULL;
int decode_request = 0;


int main(int argc, char *argv[])
{
	buffer *filedata;
	erasure_wed *wed;

	if (!parse_args(argc, argv))
	{
		fprintf(stderr,
		        "Usage: %s [-d] filename\n"
		        " -d | Decode file\n",
		        argv[0]);
		return 1;
	}

	if (!erasure_init())
	{
		fprintf(stderr, "Failed to initialize erasure coding subsystem\n");
		return 1;
	}

	if (decode_request)
	{
		printf("Decode functionality not yet implemented\n");
		return 0;
	}
	else
	{
		filedata = buffer_from_file(target_file);
		if(!filedata)
		{
			fprintf(stderr, "Failed to get data from target file\n");
			return 1;
		}

		wed = erasure_encode_request(filedata->data, filedata->length);

		erasure_wed_print(erasure_wed *wed)

		erasure_process_request(wed);

		erasure_wed_free(wed);
		buffer_free(filedata);
	}

	erasure_cleanup();

	return 0;
}


int parse_args(int argc, char *argv[])
{
	int option;

	opterr = 0;

	while ((option = getopt(argc, argv, "d")) != -1)
	{
		switch (option)
		{
			case 'd':
				decode_request = 1;
				break;
			default:
				fprintf(stderr, "Undefined flag given\n");
				return 0;
		}
	}

	// Verify we have one more argument, which should be the target filename
	if (optind + 1 > argc)
	{
		fprintf(stderr, "No target file specified\n");
		return 0;
	}
	else if (optind + 1 < argc)
	{
		fprintf(stderr, "More than one target file specified\n");
		return 0;
	}

	target_file = argv[optind];

	return 1;
}


buffer * buffer_from_file(char *filename)
{
	FILE *filepointer;
	buffer *new;

	filepointer = fopen(filename, "rb");
	if(!filepointer)
	{
		fprintf(stderr, "Failed to open file %s: %m\n", filename);
		return NULL;
	}

	// Allocate buffer object
	new = malloc(sizeof(*new));
	if(!new)
	{
		fprintf(stderr, "Failed to allocate buffer structure: %m");
		goto BUFFER_FAILURE;
	}

	// Determine size
	fseek(filepointer, 0, SEEK_END);
	new->length = ftell(filepointer);
	rewind(filepointer);

	// Allocate actual buffer
	new->data = malloc(new->length);
	if(!new->data)
	{
		fprintf(stderr, "Failed to allocate buffer for file data: %m");
		goto DATABUFFER_FAILURE;
	}
	fread(new->data, sizeof(char), new->length, filepointer);

	// Close file
	fclose(filepointer);

	return new;

	// Failure cleanups
	DATABUFFER_FAILURE: free(new);
	BUFFER_FAILURE: fclose(filepointer);
	return NULL;
}


void buffer_free(buffer *target)
{
	if(target->data)
	{
		free(target->data);
	}

	free(target);
}

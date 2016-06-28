#include "libcapirasure.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define TEST_DATA "Flannel kogi shabby chic, tousled VHS umami pabst blue bottle street art gochujang. Vice actually kitsch stumptown, pabst sriracha kogi master cleanse. Cliche raw denim bitters, try-hard four dollar toast lo-fi bicycle rights. Ramps ennui bitters letterpress. Sustainable crucifix drinking vinegar, PBR&B austin slow-carb meditation celiac semiotics lo-fi asymmetrical typewriter knausgaard post-ironic ennui. Vice hoodie church-key, portland bespoke microdosing art party beard tilde roof party pitchfork keffiyeh green juice. Polaroid roof party put a bird on it, four dollar toast pinterest VHS waistcoat normcore marfa godard austin kinfolk asymmetrical microdosing.\n\nChurch-key meditation knausgaard intelligentsia, organic sartorial celiac pinterest taxidermy. Photo booth hashtag shabby chic, vegan tattooed aesthetic ugh. Letterpress cred hashtag twee, occupy wayfarers locavore semiotics hammock. Cray asymmetrical everyday carry, yuccie quinoa XOXO venmo farm-to-table hoodie put a bird on it taxidermy portland irony. Biodiesel hella lo-fi actually, narwhal kickstarter gentrify readymade normcore. Swag paleo readymade flannel migas shabby chic. Sustainable +1 ugh bespoke, twee knausgaard selvage vice plaid gentrify organic offal.\n\n90's sartorial PBR&B, yuccie squid migas bespoke deep v quinoa chillwave flexitarian williamsburg letterpress keytar. Echo park chartreuse YOLO 3 wolf moon. Paleo neutra seitan typewriter taxidermy tacos. Fap everyday carry gochujang, cold-pressed pickled banh mi iPhone kinfolk twee godard plaid freegan schlitz chillwave deep v. Artisan flannel pug, readymade ennui ramps church-key humblebrag banh mi. Biodiesel next level banh mi lo-fi bushwick, keffiyeh readymade chambray. Gentrify you probably haven't heard of them fanny pack gastropub bicycle rights, yuccie 90's gochujang before they sold out sartorial 8-bit slow-carb pug meggings seitan.\n\nChicharrones 8-bit XOXO farm-to-table sartorial. Seitan semiotics beard chicharrones. Four loko tilde kombucha, cornhole hella sriracha heirloom twee keffiyeh humblebrag fingerstache meggings quinoa. Pop-up lumbersexual tofu, occupy vinyl venmo normcore 3 wolf moon trust fund. Kale chips polaroid pinterest, selvage tofu portland swag taxidermy narwhal fanny pack microdosing. Fingerstache seitan biodiesel ennui thundercats. Organic irony marfa waistcoat church-key roof party."

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

	erasure_wed_print(wed);

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

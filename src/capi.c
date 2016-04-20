#include "capi.h"


capi_handle * capi_init(void)
{
	capi_handle *new;
	new = cxl_afu_open_dev(DEVICE_PATH);
	if (!new)
	{
		fprintf(stderr, "Failed to open device at " DEVICE_PATH "\n");
		return NULL;
	}

	return new;
}


int capi_attach(capi_handle *afu, void *wed)
{
	int ret = cxl_afu_attach(afu, (__u64)wed);
	cxl_mmio_map(afu, CXL_MMIO_BIG_ENDIAN);
	return ret;
}


void capi_free(capi_handle *afu)
{
	cxl_afu_free(afu);
}

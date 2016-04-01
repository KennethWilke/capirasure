#ifndef __CAPI_H__
#define __CAPI_H__

#include "libcxl.h"

#define DEVICE_PATH "/dev/cxl/afu0.0d"

typedef struct cxl_afu_h capi_handle;

capi_handle * capi_init(void);
int capi_attach(capi_handle *afu, void *wed);
void capi_free(capi_handle *afu);

#endif

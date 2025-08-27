#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vhal_client vhal_client_t;

/** Connects to VHAL. Returns NULL on failure. */
vhal_client_t* vhal_connect(void);

/** Disconnects and frees client handle. Safe to pass NULL. */
void vhal_disconnect(vhal_client_t* client);

/** Returns 0 on success, negative on failure. out_value must be non-NULL. */
int vhal_get_int32 (vhal_client_t* c, int32_t propId, int32_t areaId, int32_t* out_value);
int vhal_get_bool  (vhal_client_t* c, int32_t propId, int32_t areaId, int*     out_value /*0/1*/);

int vhal_set_int32 (vhal_client_t* c, int32_t propId, int32_t areaId, int32_t value);
int vhal_set_bool  (vhal_client_t* c, int32_t propId, int32_t areaId, int     value /*0/1*/);

#ifdef __cplusplus
}
#endif


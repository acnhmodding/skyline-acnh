#pragma once

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

void sl_api_publish(const char* name, uint32_t version, const void* vtable);
const void* sl_api_get(const char* name, uint32_t version);

#ifdef __cplusplus
}
#endif

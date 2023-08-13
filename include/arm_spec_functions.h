#pragma once

#ifdef __arm__ //ARM architecture
#include <cstdint>

void* arm_wide64_memcpy(void* dst, const void* src, size_t n);
#endif
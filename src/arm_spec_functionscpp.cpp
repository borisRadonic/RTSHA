#ifdef __arm__ //ARM architecture

#include "arm_spec_functions.h"

void* arm_wide64_memcpy(void* dst, const void* src, size_t n)
{
	// Cast the source and destination to byte pointers for byte-wise copying.
	uint8_t* d = (uint8_t*)dst;
	const uint8_t* s = (const uint8_t*)src;

	// For large copies, use 64-bit wide transfers to exploit the Cortex-M7's wide memory interface.
	if (n >= 8U)
	{
		// Cast pointers to 64-bit for word-wise copying.
		uint64_t* dw = (uint64_t*)d;
		const uint64_t* sw = (const uint64_t*)s;

		// Copy 8 bytes at a time.
		while (n >= 8)
		{
			*dw++ = *sw++;
			n -= 8U;
		}

		// Update byte pointers after word-wise copying.
		d = (uint8_t*)dw;
		s = (uint8_t*)sw;
	}

	// Copy any remaining bytes.
	while (n--)
	{
		*d++ = *s++;
	}
	return dst;
}
#endif

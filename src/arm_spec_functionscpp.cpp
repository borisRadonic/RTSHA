/******************************************************************************
The MIT License(MIT)

Real Time Safety Heap Allocator (RTSHA)
https://github.com/borisRadonic/RTSHA

Copyright(c) 2023 Boris Radonic

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

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

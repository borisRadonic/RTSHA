# Real Time Safety Heap Allocator (RTSHA) Demo on Digilent ZYBOZ7 ZYNQ-7020 Development Board



> [!WARNING] 
> This project is currently a work in progress. The release of the initial version is tentatively scheduled for December. Please consider this before using the code.


## Overview

The provided code showcases how to manage and utilize a custom heap storage system within an application based on the Cortex-A9, specifically on the Xilinx Zynq®-7000 SoC platform.
The code demonstrates how the standard heap functions (malloc, free, etc.) from the standard library and FreeRTOS have been overridden with RTSHA functions. It provides insights into memory allocation, heap configurations, and details about overloading new and delete operators.
Moreover, this implementation measures and reports the performance of its heap operations (in terms of CPU cycles), providing insights into the efficiency of the memory management operations.

The Cortex-A9 (I-Cache) and data (D-Cache) caches are enabled. 

## Details

## Memory Setup

The linker script is responsible for defining the layout of the program in memory. One of the critical settings in the linker file pertains to the memory segments or regions, which define the starting location and length of each memory block used by the application.

**Original Setting**

	ps7_ddr_0 : ORIGIN = 0x100000, LENGTH = 0x3FF00000

This line indicates that there's a DDR RAM segment named ps7_ddr_0 starting at the memory address 0x100000. The LENGTH value of 0x3FF00000 suggests the size of this memory block, which was quite substantial.

**Updated Settings**

To accommodate more precise memory management requirements, especially for multi-core processing and specialized heap handling, the original setting was split into four separate segments:

1. Application Memory for CPU0:
	
	ps7_ddr_0  0x100000 0x10000000

This setting reserves memory specifically for the application running on CPU0. It starts from the address 0x100000 and spans 0x10000000 bytes.

2. RTSHA Heap Memory:

	ps7_ddr_1  0x10100000 0x10000000

The RTSHA (Real-Time Safe Heap Allocator) requires a dedicated segment for the heap. This segment starts at 0x10100000 and spans 256MB (or 0x10000000 bytes).

3. Reserved Memory for a Potential Second CPU Core:

	ps7_ddr_2  0x10200000 0x10000000
	
Though not utilized in the current configuration, this memory segment is reserved for potential use with a second CPU core. Starting at 0x10200000, it also spans 256MB.

4. Another Reserved Block for the Second CPU Core:

	ps7_ddr_3  0x10300000 0x0FF00000
	
This is an additional reserved memory block for the second CPU core, starting at address 0x10300000. The size is slightly less than the previous blocks, spanning 0x0FF00000 bytes.	
	
	
The heap used by this demo, specifically on CPU0, begins at the address 0x10200000. With a length of 0x10000000 bytes, it spans 256MB. It's worth noting that this dedicated heap space is integral for systems that demand predictable and efficient memory allocation, especially in real-time scenarios.

Implications

By breaking down the memory into specific segments, the system can achieve:

**Isolation:** Preventing one part of the system (like the RTSHA heap) from inadvertently affecting another.
**Optimization:** Ensuring that each CPU has its dedicated memory, reducing contention.
**Flexibility:** Reserved segments mean that future adaptations (like integrating a second CPU core) become easier.

Such granularity and deliberate segmentation are crucial in embedded and real-time systems where efficient memory management can make a significant difference in performance and reliability.


## Overloaded New and Delete

The operators new and delete have been overloaded, redirecting memory allocation and deallocation to the RTSHA heap functions instead of the standard malloc and free.
For detailed implementation, refer to the newnew.cpp file.

## Overriding Default Memory Management

Standard heap functions are overridden using RTSHA functions. This is further explained in newmalloc.cpp. 
It's crucial to ensure safe memory management practices to prevent memory leaks and undefined behavior.

## Heap Initialization

The function rtsha_create_heap initializes the heap, specifying its start address and size.
Pages with specific sizes and purposes can be added to this heap using the rtsha_add_page function. 

The example demonstrates adding pages of types RTSHA_PAGE_TYPE_32, RTSHA_PAGE_TYPE_64, and RTSHA_PAGE_TYPE_POWER_TWO.

## Usage

1. Ensure that you have the required hardware platform and setup.
2. Deploy the code.
3. Monitor the log outputs to see the memory management in action and the performance metrics.

Memory can be allocated and deallocated using rtsha_malloc and rtsha_free, respectively.
Additional functions like rtsha_memset and rtsha_memcpy allow for common safety memory operations.
In the example, the time taken for allocations and deallocations is measured in CPU cycles using the XTime_GetTime function.


## Important Reminders

Ensure memory safety and proper management to prevent memory leaks and undefined behavior.
Handle allocations and deallocations appropriately. Failure to do so can lead to system instability.

## Example Outputs

The code sends formatted strings to a UART terminal, reporting the performance (in terms of cycles) of various heap operations. It will report either the performance metrics or an error message if the operation fails.

Example Output:

	Power2 Page rtsha_malloc: 1436 cycles, rtsha_free: 775 cycles MaxM=3563 MaxF=1470
	Small Fix Page rtsha_malloc: 487 cycles, rtsha_free: 350 cycles


## Measured Performance


Based on the results obtained from the system's profiling, here are the performance metrics in terms of CPU cycles for the memory operations:

**CPU Frequency: 333Mhz**

Measured Performance for Release Version:

*Power2 Page:*

rtsha_malloc: 963 to 3931 Cycles

rtsha_free: 726 to 1470 Cycles

This represents the time taken for memory allocation and deallocation in a flexible page system that can handle a range of block sizes in power-of-two increments,
up to a specified maximum. These metrics are crucial for understanding the efficiency of the RTSHA heap system. 
Depending on the application's requirements, developers can utilize these metrics to optimize memory operations further and make informed decisions regarding
which type of page to use for different allocation needs.

*Small Fix Page:*

rtsha_malloc: 450 to 560 Cycles

rtsha_free: 330 to 400 Cycles

This represents the time taken for memory allocation and deallocation for smaller fixed-size pages, specifically designed for handling memory chunks less than 512 bytes.

## Potential Improvements

**Error handling:** While the code checks for memory allocation errors, robust applications might require more comprehensive error handling and recovery mechanisms.

**Code Modularization:** Depending on the project's size, consider splitting the memory management, performance measurement, and reporting functionalities into separate modules or functions for better code organization.

**Performance:** Try investigate other compiler flags that might be specific to your toolchain and beneficial to your application.
# Real Time Safety Heap Allocator (RTSHA) Demo on STM32H7 Discovery kit



> [!WARNING] 
> This project is currently a work in progress. The release of the initial version is tentatively scheduled for December. Please consider this before using the code.


## Overview


The provided code is an example of managing and utilizing a custom heap storage system in a Cortex-M7 based application. This system allows for memory allocation in various-sized blocks,
with functions provided to allocate, deallocate, and manipulate memory.

Moreover, this implementation measures and reports the performance of its heap operations (in terms of CPU cycles), providing insights into the efficiency of the memory management operations.

The Cortex-M7 (I-Cache) and data (D-Cache) caches are enabled. 

## Details

## Memory Setup

    A segment of 288KB of RAM (starting at address 0x30000000) is dedicated for the RTSHA heap storage.

## Overloaded New and Delete

    The operators new and delete have been overloaded, redirecting memory allocation and deallocation to the RTSHA heap functions instead of the standard malloc and free.
    For detailed implementation, refer to the newnew.cpp file.

## Heap Initialization

    The function rtsha_create_heap initializes the heap, specifying its start address and size.
    Pages with specific sizes and purposes can be added to this heap using the rtsha_add_page function. The example demonstrates adding pages of types RTSHA_PAGE_TYPE_32, RTSHA_PAGE_TYPE_64, and RTSHA_PAGE_TYPE_POWER_TWO.

## Heap Usage

    Memory can be allocated and deallocated using rtsha_malloc and rtsha_free, respectively.
    Additional functions like rtsha_memset and rtsha_memcpy allow for common safety memory operations.
    In the example, the time taken for allocations and deallocations is measured in CPU cycles using the DWT_GetCycles function.

## Additional Operations

    The code provides an example of using the overloaded new operator with a standard C++ list of doubles.
    A UART-based report is generated at regular intervals, showcasing the time taken for memory operations in cycles.

## Important Reminders

    Ensure memory safety and proper management to prevent memory leaks and undefined behavior.
    Handle allocations and deallocations appropriately. Failure to do so can lead to system instability.

## Example Outputs

    The code sends formatted strings to a UART terminal, reporting the performance (in terms of cycles) of various heap operations. It will report either the performance metrics or an error message if the operation fails.

## Dependencies

    The code depends on the existence of several external functions and configurations, such as DWT_GetCycles, HAL_UART_Transmit, rtsha_malloc, and others. Ensure that these are properly implemented in your project.
    Ensure that the UART (in this example, UART3) is correctly configured and initialized for the reporting to work.

## Measured Performance

Based on the results obtained from the system's profiling, here are the performance metrics in terms of CPU cycles for the memory operations:

*Small Fix Page:*

    rtsha_malloc: 2226 cycles
    rtsha_free: 1752 cycles

This represents the time taken for memory allocation and deallocation for smaller fixed-size pages, specifically designed for handling memory chunks less than 32 bytes.

*Power2 Page:*

    rtsha_malloc: 5810 cycles
    rtsha_free: 3545 cycles

This represents the time taken for memory allocation and deallocation in a flexible page system that can handle a range of block sizes in power-of-two increments,
up to a specified maximum. These metrics are crucial for understanding the efficiency of the RTSHA heap system. 
Depending on the application's requirements, developers can utilize these metrics to optimize memory operations further and make informed decisions regarding
which type of page to use for different allocation needs.

## Potential Improvements

    Error handling: While the code checks for memory allocation errors, robust applications might require more comprehensive error handling and recovery mechanisms.
    Code Modularization: Depending on the project's size, consider splitting the memory management, performance measurement, and reporting functionalities into separate modules or functions for better code organization.
	Performance: Try to use optimization flags, like -O2 or -O3 when building your project and investigate other compiler flags that might be specific to your toolchain and beneficial to your application.
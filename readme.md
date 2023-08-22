# Real Time Safety Heap Allocator (RTSHA)

> [!WARNING] 
> This project is currently a work in progress. The release of the initial version is tentatively scheduled for December. Please consider this before using the code.

Good programming practices for real time emmbedded applications includes the rule that all values must allocated on the stack if possible. There are some situations where is this not possible like when the size of the value is unknown or the vectors are growing in size over time.
In those situations memory from the heap must be dinamically usig heap allocator functions like **malloc**() and **free**().

There are various implementations of heap allocation algorithms used on different platforms. Some notable examples include:
  - Dlmalloc: Doug Lea's Memory Allocator.
  - Phkmalloc: Originally developed by Poul-Henning Kamp for FreeBSD in 1995-1996, later adapted by several operating systems.
  - ptmalloc: A memory-efficient implementation derived from Dlmalloc, commonly used on Unix systems.
  - jemalloc: A SMP-scalable allocator primarily used in FreeBSD's libc.
  - Google Chrome's PartitionAlloc: An optimized memory allocator focused on space efficiency, allocation latency, and security.
  - glibc heap allocator: Derived from ptmalloc and commonly used in the GNU C Library.
  ...

While each of these memory management implementations has its advantages, they are not specifically designed for use in hard real-time environments, where speed, determinism, elimination of fragmentation, and memory safety are the primary goals.

**Real Time Safety Heap Allocator** can be sused on bare metal platforms or together with small RT OS for several reasons:

  1. **Memory Management**: Heap algorithms are responsible for managing dynamic memory allocation and deallocation in a system.
     On platforms, where the operating system is absent or minimal, managing memory becomes crucial.
     RTSHA algorithm ensures efficient allocation and deallocation of memory resources, minimizes fragmentation issues, or undefined behavior.

  2. **Deterministic Behavior**: Platforms often require real-time or safety-critical applications where predictability and determinism are vital. RTSHA algorithms provide guarantees on the worst-case execution time for memory allocation and deallocation operations. This predictability ensures that the system can meet its timing constraints consistently.

  3. **Certification Requirements:** Safety-critical systems often require compliance with industry standards and regulations, such as DO-178C for avionics, ISO 26262 for automotive, or IEC 61508 for industrial control systems.
    These standards emphasize the need for certified software components, including memory management. High code quality, good documentation, and standards used during the development of HR-SHA, such as MISRA, etc., 
    can meet the certification requirements, accelerate and streamline the certification process and demonstrate the reliability and robustness of their systems.
  
  4. **Resource Optimization**: Bare metal platforms typically have limited resources, including memory. RTSHA optimizes memory utilization by minimizing fragmentation and efficiently managing memory allocation requests.
    This optimization is crucial for maximizing the available resources and ensuring the system operates within its limitations.
      
 Overall, using the RTSHA on bare metal platforms enhances memory management, promotes determinism, ensures memory safety, meets some important safety certification requirements, and optimizes resource utilization.
 These factors are essential for developing reliable, predictable, and secure systems in safety-critical environments.


## Table of Contents

1. [About RTSHA](#about-rtsha)
2. [Project Status](#project-status)
3. [Configuration](#configuration)
4. [Building](#building)
5. [Documentation](#documentation)
6. [Examples](#examples)


## About RTSHA

### The main requirements for RTSHA

**Predictable Execution Time**: The worst-case execution time for the 'malloc, free' and 'new delete C++' functions must be deterministic and independent of application data.

**Memory Pool Preservation**: The algorithm must strive to minimize the likelihood of exhausting the memory pool. This can be achieved by reducing fragmentation and minimizing memory waste.

**Fragmentation Management**: The algorithms should effectively manage and reduce external fragmentation, which can limit the amount of available free memory.

**Defined Behavior**: The allocator must aim to eliminate any undefined behavior to ensure consistency and reliability in its operations.

**Functional Safety**; The allocator must adhere to the principles of functional safety. It should consistently perform its intended function during normal and abnormal conditions. Its design must consider and mitigate possible failure modes, errors, and faults.
* When we talk about 'functional safety'in RTSHA, we are not referring to 'security'. "Functional safety" refers to the aspect of a system's design that ensures it operates correctly in response to its inputs and failures, minimizing risk of physical harm,  while "security" refers to the measures taken to protect a system from unauthorized access, disruption, or damage. *


**Error Detection and Handling**: The allocator should have mechanisms to detect and handle memory allocation errors or failures. This can include robust error reporting, and fallback or recovery strategies in case of allocation failures.

**Support for Different Algorithms**: The allocator should be flexible enough to support different memory allocation algorithms, allowing it to be adapted to the specific needs of different applications.

**Configurability**: The allocator should be configurable to suit the requirements of specific platforms and applications. This includes adjusting parameters like the size of the memory pool, the size of allocation blocks, and the allocation strategy.

**Efficiency**: The allocator should be efficient, in terms of both time and space. It should aim for minimal overhead and quick allocation and deallocation times.

**Readability and Maintainability**: The code for the allocator should be clear, well-documented, and easy to maintain. This includes adhering to good coding practices, such as using meaningful variable names and including comments that explain the code.

**Compatibility**: The allocator should be compatible with the system it is designed for and work well with other components of the system. 


**RTSHA Algorithms**

There are several different algorithms that can be used for heap allocation supported by RTSHA:

### Small Fix Memory Pages

This algorithm is an approach to memory management that is often used in specific situations where objects of a certain size are frequently allocated and deallocated. By using of uses 'Fixed chunk size' algorithm greatly simplies the memory allocation process and reduce fragmentation.

The memory is divided into pages of chunks(blocks) of a fixed size (32, 64, 128, 256 and 512 bytes).
When an allocation request comes in, it can simply be given one of these blocks. This means that the allocator doesn't have to search through the heap to find a block of the right size, which can improve performance.
The free blocks memory is used as 'free list' storage. The list is implemented using a standard linked list.
However, by enabling the precompiler option USE_STL_LIST, the STL version of the forward list can also be utilized. There isn't a significant performance difference between the two implementations.

Deallocations are also straightforward, as the block is added back to the list of available chunks. There's no need to merge adjacent free blocks, as there is with some other allocation strategies, which can also improve performance.

However, fixed chunk size allocation is not a good fit for all scenarios. It works best when the majority of allocations are of the same size, or a small number of different sizes. If allocations requests are of widely varying sizes, then this approach can lead to a lot of wasted memory, as small allocations take up an entire chunk, and large allocations require multiple chunks.

Small Fix Memory Page is also used internaly by "Power Two Memory Page" and "Big Memory Page" algorithms.

**Power Two Memory Pages**

This algorithm is a more intricate system that exclusively allows blocks of sizes that are powers of two. This design makes merging free blocks back together easier and significantly reduces fragmentation.
The core of this algorithm is based on an array of free lists. Each list corresponds to one group of power-of-two sizes. For instance, there's a dedicated list for 64-byte free blocks, another for 128-byte blocks, and so on. This structured approach ensures that blocks of a specific size are readily available, optimizing memory management and access.
This method ensures efficient block allocation and deallocation operations, making the most of the power-of-two size constraint.
Utilizing the combination of power-of-two block sizes with an array of free lists and a binary search mechanism, this algorithm strikes a balance between memory efficiency and operational speed.
This is a fairly efficient method of allocating memory, particularly useful for systems where memory fragmentation is an important concern. The algorithm divides memory into partitions to try to minimize fragmentation and the 'Best Fit' algorithm searches the page to find the smallest block that is large enough to satisfy the allocation. 
Furthermore, this system is resistant to breakdowns due to its algorithmic approach to allocating and deallocating memory. The coalescing operation helps ensure that large contiguous blocks of memory can be reformed after they are freed, reducing the likelihood of fragmentation over time.
Coalescing relies on having free blocks of the same size available, which is not always the case, and so this system does not completely eliminate fragmentation but rather aims to minimize it.


**Big Memory Pages**

Note: This algorithm is primarily designed for test purposes, especially for systems with constrained memory. When compared to the "Small Fixed Memory Pages" and "Power Two Memory Pages" algorithms, this approach may exhibit relatively slower (inperformant) behaviors.
The "Big Memory Pages" algorithm employs the "Best Fit" strategy, which is complemented by a "Red-Black" balanced tree. The Red-Black tree ensures worst-case guarantees for insertion, deletion, and search times, making it predictable in performance. A key distinction between this system and the "Power Two Memory Page" is in how they handle memory blocks. Unlike the latter, "Big Memory Pages" does not restrict memory to be partitioned exclusively into power-of-two sizes. Instead, variable block sizes are allowed, providing more flexibility. Additionally, once memory blocks greater than 512 bytes are released, they are promptly merged or coalesced, optimizing the memory usage.
Despite its features, it's essential to understand the specific use-cases and limitations of this algorithm and to choose the most suitable one based on the system's requirements and constraints.

The use of 'Small Fixed Memory Pages' in combination with 'Power Two Memory Pages' is recommended for all real time systems.

**Modern C++ and STL**

Writing a correct and efficient memory allocator is a non-trivial task.
STL provides many algorithms for sorting, searching, manipulating and processing data. These algorithms can be useful for managing metadata about memory blocks, such as free and used blocks. 
Using existing algorithms and data structures from the C++ Standard Template Library (STL) has several advantages over developing your own, such as:

- Efficiency: The STL is designed by experts who have fine-tuned the algorithms and data structures for performance. They generally offer excellent time and space complexity, and have been optimized over many years of use and improvement.
- Reliability: STL components have been thoroughly tested and are widely used. This means they are reliable and less likely to contain bugs compared to new, untested code.
- Readability and Maintainability: Using standard algorithms and data structures makes code more readable and easier to understand for other developers familiar with C++.
- Productivity: It's usually faster to use an existing STL component than to write a new one from scratch. 
- Compatibility: STL components are designed to work together seamlessly, and using them can help ensure compatibility with other C++ code and libraries.


## Project Status

This project is currently a work in progress. The release of the initial version is tentatively scheduled for December. Please consider this before using the code.


## Configuration


## Building

Building

Windows

	Open ide/vs2022/RTSHALibrary.sln in Visual Studio 2022 and build.



## Documentation

[View RTSH Documentation in PDF](https://github.com/borisRadonic/RTSHA/blob/master/docs/RTSHA%20Documentation.pdf)


## Examples

[STM32H750 Demo](https://github.com/borisRadonic/RTSHA/tree/master/Examples/STM32H750/DemoRTSH)
[ZYNQ-7020 Demo](https://github.com/borisRadonic/RTSHA/tree/master/Examples/Xilinx/ZYBOZ7)



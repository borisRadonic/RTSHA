# # Real Time Safety Heap Allocator (RTSHA)

Good programming practices for real time emmbedded applications includes the rule that all values must allocated on the stack if possible. There are some situations where is this not possible like when the size of the value is unknown or the vectors are growing in size over time. In those situations memory from the heap must be dinamically usig heap allocator functions like **malloc**() and **free**().

There are various implementations of heap allocation algorithms used on different platforms. Some notable examples include:
  - Dlmalloc: Doug Lea's Memory Allocator.
  - Phkmalloc: Originally developed by Poul-Henning Kamp for FreeBSD in 1995-1996, later adapted by several operating systems.
   - ptmalloc: A memory-efficient implementation derived from Dlmalloc, commonly used on Unix systems.
   - jemalloc: A SMP-scalable allocator primarily used in FreeBSD's libc.
   - Google Chrome's PartitionAlloc: An optimized memory allocator focused on space efficiency, allocation latency, and security.
   - glibc heap allocator: Derived from ptmalloc and commonly used in the GNU C Library.

While each of these memory management implementations has its advantages, they are not specifically designed for use in hard real-time environments, where speed, determinism, elimination of fragmentation, and memory safety are the primary goals.

**Real Time Safety Heap Allocator** can be sused on bare metal platforms or together with small RT OS for several reasons:

  1. **Memory Management**: Heap algorithms are responsible for managing dynamic memory allocation and deallocation in a system.
     On platforms, where the operating system is absent or minimal, managing memory becomes crucial.
     HRT-SHA algorithm ensures efficient allocation and deallocation of memory resources, minimizes fragmentation issues, or undefined behavior.

  2. **Deterministic Behavior**: Platforms often require real-time or safety-critical applications where predictability and determinism are vital. RTSHA algorithms provide guarantees on the worst-case execution time for memory allocation and deallocation operations. This predictability ensures that the system can meet its timing constraints consistently.

  3. **Certification Requirements:** Safety-critical systems often require compliance with industry standards and regulations, such as DO-178C for avionics, ISO 26262 for automotive, or IEC 61508 for industrial control systems.
    These standards emphasize the need for certified software components, including memory management. High code quality, good documentation, and standards used during the development of HR-SHA, such as MISRA, etc., 
    can meet the certification requirements, accelerate and streamline the certification process and demonstrate the reliability and robustness of their systems.
  
  4. **Resource Optimization**: Bare metal platforms typically have limited resources, including memory. HRT-SHA optimizes memory utilization by minimizing fragmentation and efficiently managing memory allocation requests.
    This optimization is crucial for maximizing the available resources and ensuring the system operates within its limitations.
      
 Overall, using the RTSHA on bare metal platforms enhances memory management, promotes determinism, ensures memory safety, meets some important safety certification requirements, and optimizes resource utilization.
 These factors are essential for developing reliable, predictable, and secure systems in safety-critical environments.


## Table of Contents

1. [About RTSHA](#about-hrt-sha)
2. [Project Status](#project-status)
3. [Configuration](#configuration)
4. [Building](#building)
5. [Documentation](#documentation)



## About RTSHA

Real Time Safety Heap Allocator:


- prevents fragmentation of physical memory when memory is allocated and de-allocated dynamically
- helps to achive deterministic performance and timing constraints
- possibility to configure for specific platforms and applications
- enables configuration that uses fixed memory chunk sizes 16, 32, 64, 128, 512 ... bytes and big standard variable chunk sizes


The main requirements that need to be fulfilled are:

- The worst-case execution time of the malloc and free functions has to be known and not dependable of application data.
	The algorithm enables minimizing the likelihood of depleting the memory pool by reducing fragmentation and minimizing wasted memory.
- Efficient allocation and deallocation of memory resources, minimizes fragmentation issues, or undefined behavior.

The algorithm:

- promptly merges or coalesced the memory blocks after their release. This is a standard part of memory management in which two adjacent free blocks of memory are merged.
- expects user memory devided in pages defined by user. The page contains the memory blocks of the same size ( for example 16, 32, 64, 128, 512 bytes and the pages for 'big blocks' )
  The smallest allocatable memory block is 16 bytes and the biggest allocatable block is user defined.
- Almost-Best-Fit strategy is employed to minimize fragmentation of big blocks (blocks greater than 512 bytes)
- reuses recently released memory blocks over those that were released further in the past


## Project Status

* Work in progress...


## Configuration


##Building


Building
Windows

Open ide/vs2022/RTSHALibrary.sln in Visual Studio 2022 and build.



## Documentation
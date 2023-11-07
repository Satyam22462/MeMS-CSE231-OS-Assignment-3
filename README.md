# MeMS:Memory Management System [CSE231 OS Assignment-3]


## [Documentation](https://docs.google.com/document/d/e/2PACX-1vTiMFYWuqKHlGG5cWZfVgHx3T-H_HYAjIc_yJ_kX5171FPiZCmyyJB0BJUuVplX3pY4ycw8bFf4gaxG/pub)





## Overview

MeMS is a custom memory management system implemented in C for efficient memory allocation and management. It is designed to minimize memory fragmentation and dynamically allocate memory in multiples of the system's PAGE_SIZE using mmap and munmap system calls.


## Key Features
**Memory Allocation:** Allocates memory in multiples of PAGE_SIZE.

**Free List:** Manages free memory using a free list data structure.

**Memory Reuse:** Reuses available memory segments from the free list.

**Dynamic Memory Growth:** Requests more memory from the OS when needed.

**Memory Mapping:** Maps MeMS virtual addresses to MeMS physical addresses.

Printing statistics about memory usage, including the number of pages used, unused memory, main chain length, and sub-chain length.


## Code Structure
Defines data structures (MemorySegment and MainNode) to represent memory segments and main memory nodes.

Provides functions for initialization, memory allocation, statistics printing, and memory cleanup.

Global variables are used to maintain memory-related data.


## Data Structures
#### MemorySegment (MS)
Represents a memory segment.
Contains information such as size, type (0 for HOLE, 1 for PROCESS), the previous and next segments,starting and ending virtual addresses, and pointer to addresses to mems physical memory.

#### MainNode (MN)
Represents a node in the main chain.
Contains the main memory block, a sub-chain of memory segments, and references to the previous and next nodes in the main chain

## Functions
**mems_init():** Initializes the MeMS system, including the head of the free list and the starting MeMS virtual address. It also sets other global variables.

**mems_finish():** Called at the end of the MeMS system, it unmaps allocated memory using the munmap system call.

**mems_malloc(size_t size):** Allocates memory of the specified size by reusing a segment from the free list if available. If no suitable segment is found, it uses the mmap system call to request more memory from the OS and updates the free list accordingly.

**mems_print_stats():** Prints statistics about the MeMS system, including the number of pages used, unused memory, main chain length, and details about each node in the main chain and each segment in the sub-chain.

                        For every Subchain in the free list print the data as follows
                      MAIN[starting_mems_vitual_address:ending_mems_vitual_address] -> <HOLE or PROCESS>[starting_mems_vitual_address:ending_mems_vitual_address] <-> ..... <-> NULL
                        After printing the whole freelist print the following stats
                      Page used: <Total Pages used by the user>
                      Space unused: <Total space mapped but not used by user>
                      Main Chain Length: <Length of the main chain>
                      Sub-chain Length array: <Array denoting the length of the subchains>
                      
** mems_get(void *v_ptr): ** Returns the MeMS physical address mapped to the given MeMS virtual address.


** mems_free(void *v_ptr): ** Frees the memory pointed to by the MeMS virtual address, marking the corresponding sub-chain node as HOLE and combining adjacent HOLE nodes to reduce fragmentation.


## How to Use these functions in your program
Include the necessary headers in your C program:

#include"mems.h"


Initialize the MeMS system using mems_init().

Allocate and free memory using mems_malloc() and mems_free().

Print memory statistics using mems_print_stats().

To clean up the MeMS system, call mems_finish() at the end of your program.


## How to run c program
After implementing functions in mems.h follow the below steps to run example.c file

$ make

$ ./example

## Example Output

PAGE_SIZE= 4096 Bytes

Starting MeMS Virtual Address= 1000

![Example_output](https://github.com/Satyam22462/MeMS-CSE231-OS-Assignment-3/assets/119166910/8b781327-0f4d-4423-beef-49621e3e43a1)


### [GitHub Repository](https://github.com/Satyam22462/MeMS-CSE231-OS-Assignment-3.git)




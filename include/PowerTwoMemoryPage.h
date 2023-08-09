#pragma once
#include <stdint.h>
#include "internal.h"
#include "MemoryPage.h"

namespace rtsha
{
	using namespace std;

	/*! \class PowerTwoMemoryPage
	* \brief This class provides various memory handling functions that manipulate MemoryBlock's on 'Power two memory page'
	*
	This is a complex system, which only allows blocks of sizes that are powers of two. This makes merging free blocks back together easier
	and reduces fragmentation. A specialised binary search tree data structures (red-black tree) for fast storage and retrieval of ordered
	information are stored at the end of the page using fixed size Small Fix Memory Page.
	
	This is a fairly efficient method of allocating memory, particularly useful for systems where memory fragmentation is an important concern.
	The algorithm divides memory into partitions to try to minimize fragmentation and the 'Best Fit' algorithm searches the page to find the
	smallest block that is large enough to satisfy the allocation.

	Furthermore, this system is resistant to breakdowns due to its algorithmic approach to allocating and deallocating memory.
	The coalescing operation helps ensure that large contiguous blocks of memory can be reformed after they are freed, reducing 
	the likelihood of fragmentation over time.

	Coalescing relies on having free blocks of the same size available, which is not always the case, and so this system does not
	completely eliminate fragmentation but rather aims to minimize it.
	*/
	class PowerTwoMemoryPage : public MemoryPage
	{
	public:

		/// @brief Default constructor is deleted to prevent default instantiation.
		PowerTwoMemoryPage() = delete;

		/**
		* @brief Constructor that initializes the PowerTwoMemoryPage with a given page.
		* @param page The rtsha_page structure to initialize the PowerTwoMemoryPage with.
		*/
		PowerTwoMemoryPage(rtsha_page* page) : MemoryPage(page)
		{
		}

		/// @brief Virtual destructor for the PowerTwoMemoryPage.
		virtual ~PowerTwoMemoryPage()
		{
		}

		/*! \fn allocate_block(size_t size)
		* \brief Allocates a memory block of power 2 size.
		*
		* \param size The size of the memory block, in bytes.
		*
		* \return On success, a pointer to the memory block allocated by the function.
		*/
		virtual void* allocate_block(size_t size) final;

		/*! \fn free_block(MemoryBlock& block)
		* \brief This function deallocates memory block.
		*
		* A block of previously allocated memory.
		*
		* \param block Previously allocated memory block.
		*/
		virtual void free_block(MemoryBlock& block) final;

		/*! \fn createInitialFreeBlocks()
		* \brief This function creates initial free blocks on empty memory page.
		*
		*/
		void createInitialFreeBlocks();

	protected:

		/**
		* @brief Splits a memory block into two blocks based on the specified size.
		*
		* This function is used for power-of-two allocators where blocks can be
		* efficiently split in half multiple times. After splitting, the original block
		* is resized to 'end_size', and the remainder of the same size becomes a new block.
		*
		* @param block The block to be split.
		 * @param end_size The desired size of the block after splitting.
		*/
		void splitBlockPowerTwo(MemoryBlock& block, size_t end_size);

		/**
		* @brief Merges the specified block with its left neighbor.
		*
		* This function is typically used when freeing a block of memory to see if
		* it can be coalesced with its left neighbor to create a larger block.
		*
		* @param block The block to be merged with its left neighbor.
		*/
		void mergeLeft(MemoryBlock& block);

		/**
		* @brief Merges the specified block with its right neighbor.
		*
		* This function is typically used when freeing a block of memory to see if
		* it can be coalesced with its right neighbor to create a larger block.
		*
		* @param block The block to be merged with its right neighbor.
		*/
		void mergeRight(MemoryBlock& block);
		
	};
}

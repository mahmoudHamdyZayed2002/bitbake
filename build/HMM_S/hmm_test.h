#ifndef HMM_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

  /* 
 * Structure representing metadata associated with each memory block.
 * Contains a pointer to the next block, the size of the block, and a flag
 * indicating whether the block is free or busy.
 */
struct meta_data {
    struct meta_data *next_block; // Pointer to the next block
    size_t size;                  // Size of the block
    char flag;                    // Flag indicating freeBlock or busyBlock
};

/* Macro defining the value used to indicate a free memory block. */
#define freeBlock 1

/* Macro defining the value used to indicate a busy (allocated) memory block. */
#define busyBlock 0

/* Macro defining the size of the struct meta_data, used for metadata calculation. */
#define META_SIZE sizeof(struct meta_data) /*16 bytes*/

/* Macro defining the size of a page in memory [4 kb], typically used for heap management. */
#define PAGE_SIZE 4096

/* 
 * Macro defining the size of the memory increment when expanding the heap.
 * It's often set to a multiple of the page size to improve memory allocation performance.
 */
#define PBR_STEP PAGE_SIZE * 33


/*
 * Initializes the memory management system.
 * 
 * This function sets up the initial state of the memory management system by:
 * 1. Allocating memory for the initial free list using the sbrk system call.
 * 2. Determining the current program break and storing it in ProgBreak.
 * 3. Initializing the number of blocks to 1.
 * 4. Setting the flag of the first block in the free list to indicate it's free.
 * 5. Setting the next block pointer of the first block to NULL, indicating no further blocks yet.
 * 6. Setting the size of the first block in the free list based on the allocated memory size.
 * 7. Updating the tail pointer to point to the end of the allocated memory block.
 * 
 * Parameters: None
 * 
 * Returns: None
 */
void init_fun(void);
/*
 * Finds a suitable free block for memory allocation or extends the heap if necessary.
 * 
 * This function traverses the linked list of free memory blocks to find a block 
 * that can accommodate the requested size. If such a block is found, it returns a 
 * pointer to that block. If not, it extends the heap by requesting additional memory 
 * using the sbrk system call and then initializes metadata for the newly allocated block.
 * 
 * Parameters:
 *  - size: The size of memory requested for allocation.
 * 
 * Returns:
 *  - Pointer to the suitable free block if found.
 *  - NULL if no suitable block is found or memory extended fails.
 */
struct meta_data *find_free_block( size_t size);
/*
 * Allocates a block of memory of the required size.
 * 
 * This function attempts to find a suitable free block of memory using the find_free_block
 * function. If a suitable block is found, it marks the block as busy and checks if the 
 * block can be split to accommodate the requested size. If splitting is possible, it 
 * creates a new free block. Finally, it returns a pointer to the allocated block.
 * 
 * Parameters:
 *  - size: The size of memory requested for allocation.
 * 
 * Returns:
 *  - Pointer to the allocated block if successful.
 *  - NULL if no suitable block is found or memory allocation fails.
 */
struct meta_data *needed_block(size_t size);
/*
 * Allocates a block of memory of the required size using a custom memory management system.
 * 
 * This function first checks if the free list is NULL. If so, it initializes the memory management
 * system using the init_fun function. Then, it checks if the requested block size is negative.
 * If the size is valid, it adjusts the size to ensure proper alignment. Next, it calls the needed_block
 * function to find a suitable free block for allocation. If a block is successfully allocated,
 * it updates the program break and returns a pointer to the allocated memory block.
 * 
 * Parameters:
 *  - block_size: The size of memory requested for allocation.
 * 
 * Returns:
 *  - Pointer to the allocated block if successful.
 *  - NULL if memory allocation fails or the requested block size is negative.
 */
void *my_malloc(size_t block_size);
/*
 * Allocates and initializes a block of memory with zeros.
 * 
 * This function allocates a block of memory large enough to fit 'num' elements,
 * each of size 'size' bytes. It checks for invalid input parameters and returns
 * NULL if 'num' or 'size' is zero, or if their product would result in an overflow.
 * Otherwise, it allocates the memory using my_malloc, initializes the memory block
 * with zeros using memset, and returns a pointer to the allocated memory.
 * 
 * Parameters:
 *  - num: Number of elements.
 *  - size: Size of each element in bytes.
 * 
 * Returns:
 *  - Pointer to the allocated and initialized memory block if successful.
 *  - NULL if memory allocation fails or the input parameters are invalid.
 */
void *my_calloc(size_t num , size_t size);
/*
 * Reallocates memory to change the size of a previously allocated block.
 * 
 * This function reallocates the memory block pointed to by 'ptr' to a new size specified
 * by 'new_size'. If 'ptr' is NULL, it behaves like malloc and allocates a new memory block.
 * If 'new_size' is zero, it deallocates the memory block pointed to by 'ptr' and behaves
 * like free. Otherwise, it attempts to resize the existing memory block. If resizing is
 * successful and the next block is free and large enough to accommodate the difference
 * in sizes, the function splits the next block and creates a new free block after resizing
 * the current block. If the next block is not free or is not large enough, the function
 * allocates a new memory block, copies the data from the old block, and frees the old block.
 * 
 * Parameters:
 *  - ptr: Pointer to the previously allocated memory block.
 *  - new_size: The new size of the memory block.
 * 
 * Returns:
 *  - Pointer to the reallocated memory block if successful.
 *  - NULL if memory allocation fails or 'new_size' is zero.
 */
void *my_realloc(void *ptr , size_t new_size);
/*
 * Deallocates a previously allocated memory block.
 * 
 * This function deallocates the memory block pointed to by 'ptr'. If 'ptr' is NULL,
 * it does nothing. Otherwise, it traverses the free list to find the metadata corresponding
 * to the memory block pointed to by 'ptr'. If found, it marks the block as free and attempts
 * to merge adjacent free blocks to coalesce contiguous free memory blocks into larger ones.
 * 
 * Parameters:
 *  - ptr: Pointer to the previously allocated memory block.
 * 
 * Returns:
 *  - None
 */
void my_free (void *ptr);
/*
 * Merges adjacent free blocks to coalesce contiguous free memory blocks into larger ones.
 * 
 * This function traverses the free list to find adjacent free memory blocks. If it finds
 * two consecutive free blocks, it merges them into a single larger free block. It also
 * handles the case where the program break needs to be adjusted if the last block in the
 * heap becomes smaller than the page size.
 * 
 * Parameters:
 *  - None
 * 
 * Returns:
 *  - None
 */
void merge_(void);
/*
 * Splits a free memory block into two blocks.
 * 
 * This function splits the memory block pointed to by 'fit_block' into two blocks,
 * where the first block has the specified 'size' and the second block contains the
 * remaining free space after the split. If the remaining free space is large enough
 * to hold metadata, it creates a new free block; otherwise, it does not split the block.
 * 
 * Parameters:
 *  - fit_block: Pointer to the memory block to split.
 *  - size: The size of the first block after splitting.
 * 
 * Returns:
 *  - None
 */
void split(struct meta_data *fit_block , size_t size);

void display_free_list(struct meta_data *ptr);
//void freg_freed(unsigned int freg);



#endif 
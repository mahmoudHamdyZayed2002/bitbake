
#include "hmm_test.h"
#if 1
void * malloc(size_t size)
{
    return my_malloc(size);
}

void free(void *ptr)
{
    my_free(ptr);
}

void *calloc(size_t nmemb, size_t size)
{
	return my_calloc(nmemb , size);
}

void *realloc(void *ptr, size_t size)
{
	return my_realloc(ptr , size);
}
#endif
void *ProgBreak; //to record the current adress of the program break
struct meta_data *free_list = NULL;  // this pointer points to the start of the free list
unsigned char num_block=0;    //i used this global variable to record number of allocated blocks

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
void init_fun(void) {
    // Allocate memory for the initial free list
    free_list = sbrk(PBR_STEP);
    // Set ProgBreak to the current program break
    ProgBreak = sbrk(0);
    // Initialize the number of blocks
    num_block = 1;
    // Set the flag of the first block to indicate it's free
    free_list->flag = 1;
    // Set the next block pointer of the first block to NULL
    free_list->next_block = NULL;
    // Set the size of the first block in the free list
    free_list->size = PBR_STEP - META_SIZE;
}

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
struct meta_data *find_free_block(size_t size) {
    // Start the search from the beginning of the free list
    struct meta_data *current = free_list;
    // Variable to track if a suitable block is found
    char check = 0;

    if (current == NULL){
        return NULL;
    }
    
    // Loop until a suitable block is found or the end of the free list is reached
    while (1) {
        // Check if the current block is large enough and free
        if (((current->size > size) || (current->size == size)) && (current->flag)) {
            // Suitable block found
            check = 1;
            break;
        } 
        else {
            // Move to the next block in the free list
            if (!(current->next_block)) {
                // End of the free list reached without finding a suitable block
                check = 0;
                break;
            } else {
                current = current->next_block;
            }
        }
    }
    // If no suitable block is found, extend the heap
    if (!check) {
        while (1) {
            // Extend the heap by requesting additional memory till it suits the needed size
            current->next_block = sbrk(PBR_STEP);
            num_block++;
            // Check if the previous block was free
            if ((current->flag) == freeBlock) {
                // Increase the size of the previous block with the added heap memory
                current->size += PBR_STEP;
                current->next_block = NULL;

                // Check if the enlarged block can accommodate the requested size
                if ((size < current->size) || (size == current->size)) {
                    return current;
                }
                //continue in moving program break to add more free space
                else{
                    continue;
                }
            } 
            else if ((current->flag) == busyBlock) {
                // Initialize the metadata of the newly allocated block
                current->next_block->size = PBR_STEP - META_SIZE;

                // Check if the newly allocated block can accommodate the requested size
                if ((size < current->next_block->size) || (size == current->next_block->size)) {
                    return current->next_block;
                }
                //continue in moving program break to add more free space
                else{
                    continue;
                }
            }
        }
    } 
    //i found a suitable block in my free list
    else if (check) {
        // Return the pointer to the suitable block if found
        return current;
    }
    // Return NULL if no suitable block is found
    return NULL;
}

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
struct meta_data *needed_block(size_t size) {
    // Find a suitable free block for allocation
    struct meta_data *block = find_free_block(size);
    // Check if a suitable block is found
    if (block) {
        // Mark the block as busy
        block->flag = busyBlock;
        // Check if the block can be split to accommodate the requested size
        if (block->size > size) {
            // Split the block to create a new free block of the required size
            split(block, size);
        } else {
            // No need to split the block
        }
        // Return the allocated block
        return block;
    }
    // Return NULL if no suitable block is found
    return NULL;
}
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
void *my_malloc(size_t block_size) {
    void *return_ptr;
    
    // Check if the free list is NULL
    if (NULL == free_list) {
        // Initialize the memory management system
        init_fun(); 
    }
    else {
        /**/
    }
    
    // Check if the requested block size is negative
    if (block_size < 0) {
        return NULL;
    } 
    else {
        // Adjust block size to ensure proper alignment (assuming 8-byte alignment)
        block_size = ((block_size + 7) / 8) * 8;
        
        // Find a suitable free block for allocation
        struct meta_data *current = needed_block(block_size);
        
        // Check if a block is successfully allocated
        if (NULL == current) {
            return (void *)current;
        } 
        else {
            // Update the program break
            //ProgBreak = sbrk(0);
            
            // Move the pointer to the next memory location after metadata
            current += 1;
            
            // Set the return pointer to the allocated memory block
            return_ptr = (void *)(current);
        }
    }
    
    return return_ptr;
}


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
void *my_calloc(size_t num, size_t size) {
    // Check if either 'num' or 'size' is zero
    if ((num == 0) || (size == 0)) {
        return NULL;
    }
    // Check for potential overflow when calculating the total memory size
    else if (num * size < 0) {
        return NULL;
    }
    else {
        // Allocate memory for 'num' elements of size 'size' using my_malloc
        void *ret_ptr = my_malloc(size * num);
        
        // Check if memory allocation was successful
        if (ret_ptr != NULL) {
            // Initialize the memory block with zeros using memset
            memset(ret_ptr, 0, size * num);
        }
        
        // Return the pointer to the allocated and initialized memory block
        return ret_ptr;
    }
}

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
void *my_realloc(void *ptr, size_t new_size) {
    // Check if 'ptr' is NULL
    if (NULL == ptr) {
        // Behave like malloc and allocate a new memory block
        return malloc(new_size);
    }
    // Check if 'new_size' is zero
    else if (0 == new_size) {
        // Deallocate the memory block pointed to by 'ptr' and behave like free
        my_free(ptr);
        return NULL;
    }
    else {
        // Initialize variables
        void *ptr_old = ptr;
        void *ptr_new = NULL;
        struct meta_data *current = ((struct meta_data *)ptr) - 1;
        size_t old_size = current->size;
        signed int diff_size = new_size - old_size;
        
        // Check if the new size is the same as the old size or smaller
        if (old_size == new_size || 0 > diff_size) {
            // No need to reallocate, return the old pointer
            return ptr_old;
        }
        
        // Check if the next block exists
        if (current->next_block) {
            // Check if the next block is free
            if (current->next_block->flag) {
                // Check if the size of the next block is sufficient to accommodate the difference in sizes
                if ((current->next_block->size) > diff_size) {
                    // Calculate the size of the remaining free block after resizing
                    size_t size_ = current->next_block->size - diff_size;
                    
                    // Create a new free block after resizing the current block
                    struct meta_data *ptr_next = (void *)(((void *)current->next_block) + diff_size);
                    ptr_next->next_block = current->next_block->next_block;
                    ptr_next->flag = freeBlock;
                    ptr_next->size = size_;
                    current->next_block = ptr_next;
                    current->size = new_size;
                    ptr_new = ((void *)(current + 1));
                    
                    // Initialize the newly allocated memory with zeros
                    memset((ptr_new + old_size), 0, diff_size);
                } 
                else {
                    // Allocate a new memory block since the next block is not large enough
                    ptr_new = my_malloc(new_size);
                    if (ptr_new != NULL) {
                        // Copy data from the old block to the new block
                        ptr_new = memcpy(ptr_new, ptr_old, current->size);
                        memset((ptr_new + old_size), 0, diff_size);
                        // Free the old memory block
                        my_free(ptr_old);
                    }
                }
            } 
            else {
                // Allocate a new memory block since the next block is not free
                ptr_new = my_malloc(new_size);
                if (ptr_new != NULL) {
                    // Copy data from the old block to the new block
                    ptr_new = memcpy(ptr_new, ptr_old, current->size);
                    memset((ptr_new + old_size), 0, diff_size);
                    // Free the old memory block
                    my_free(ptr_old);
                }
            }
        } 
        else {
            // Allocate a new memory block since there's no next block
            ptr_new = my_malloc(new_size);
            if (ptr_new != NULL) {
                // Copy data from the old block to the new block
                ptr_new = memcpy(ptr_new, ptr_old, current->size);
                
                // Free the old memory block
                my_free(ptr_old);
            }
        }
        
        // Return the pointer to the reallocated memory block
        return ptr_new;
    }
}

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
void my_free(void *ptr) {
    // Check if 'ptr' is NULL
    if (ptr == NULL) {
        // Do nothing if 'ptr' is NULL
    } 
    else {
        // Initialize variables
        struct meta_data *current_node = (struct meta_data*)(ptr) - 1;
         if (current_node->flag == busyBlock) {
            current_node->flag = freeBlock; // Flag = 1 indicates that this block is free
            // Attempt to merge adjacent free blocks
             merge_();
        } 
        else {
            // Print a message if the memory block was previously freed
            printf("Previously freed memory block\n");
        }

        
    }
}
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
void merge_(void) {
    // Initialize variables
    struct meta_data *current_node = free_list;
    struct meta_data *prev_node = NULL;
    
    // Traverse the free list to find adjacent free memory blocks
    while (current_node) {
        // Check if the current node and its next node exist
        if (current_node->next_block) {
            // Check if both current and next nodes are free
            if ((freeBlock == current_node->flag) && (freeBlock == current_node->next_block->flag)) {
                // Merge the current and next nodes into a single larger free block
                current_node->size += current_node->next_block->size + META_SIZE;
                current_node->next_block = current_node->next_block->next_block;
            }
        }
        
        // Move to the next node
        prev_node = current_node;
        current_node = current_node->next_block;
        
        // Check if the end of the list is reached
        if (current_node == NULL) {
            break;
        }
        
        // Check if both previous and current nodes are free
        if (prev_node) {
            if ((freeBlock == prev_node->flag) && (freeBlock == current_node->flag)) {
                // Merge the previous and current nodes into a single larger free block
                prev_node->size += current_node->size + META_SIZE;
                prev_node->next_block = current_node->next_block; 
                // Move to the next node
                current_node = prev_node->next_block;
            }
        }
    }
    
    // Adjust the program break if necessary
    while ((PBR_STEP < (prev_node->size)) && (prev_node->flag)) {
        // Decrease the program break to release excess memory
        sbrk(-PBR_STEP);
        num_block -= 1;
        // Update the size of the last block in the heap
        prev_node->size -= PBR_STEP;
    }
}

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
void split(struct meta_data *fit_block, size_t size) {
    // Calculate the pointer to the new block after the split
    struct meta_data *new_block = (struct meta_data *)((void *)fit_block + size + META_SIZE);
    
    // Check if the remaining space after splitting is large enough to hold metadata
    if ((fit_block->size - size) > META_SIZE) {
        // Create a new free block with the remaining space
        new_block->size = (fit_block->size) - size - META_SIZE;
        new_block->flag = freeBlock;
        new_block->next_block = fit_block->next_block;
        
        // Update the fit_block metadata to represent the first block after splitting
        fit_block->next_block = new_block;
        fit_block->size = size;
        fit_block->flag = busyBlock;
    } else {
        // If the remaining space is not large enough to hold metadata, do not split
        fit_block->size = size;
    }
}

void display_free_list(struct meta_data *ptr){
    char i=0;
    printf("current\t\t\t   size\t\t flag\t\t next\t\t numberBlocks\t\t progbreak \t\t \n");
    printf("\n");
    while(ptr){
        printf("[%i]=%10p , %10ld , %10i ,    %10p ,   %10i,              %10p\n"
                ,i,ptr+1,ptr->size,ptr->flag,(ptr->next_block)+1,num_block,ProgBreak);
        
        ptr=ptr->next_block;
        i++;
    };
    printf("------------------------------------\n");
}


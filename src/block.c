/**
 * @file block.c
 * @author Hrvoje Z
 * @brief Block allocator source file
 * @version 0.1
 * @date 2025-01-20
 * 
 * @copyright Copyright (c) 2025
 * 
 */

/* Includes */
#include "block_defs.h"
#include "block.h"

/* Macros and Constants */


#ifdef ALLOC_BLOCK_SIZE
#define BLOCK_SIZE (ALLOC_BLOCK_SIZE)
#else
#define BLOCK_SIZE (32U) // Default value
#endif

#ifdef ALLOC_NUM_BLOCKS
#define BLOCK_NUMS (ALLOC_NUM_BLOCKS)
#else
#define BLOCK_NUMS (10U) // Default value
#endif

#define BLOCK_USED   (1U)
#define BLOCK_UNUSED (0U)

/* Custom Macros */

/**
 * @brief Set buffer elements of defined size to filler value
 */
#define BLOCK_MEMSET(buffer, size, filler) \
    for (size_t index = 0; index < (size); index++) { \
        ((uint8_t *)(buffer))[index] = (filler); \
        }

/**
 * @brief Check if pointer is aligned to block size
 */
#define IS_PTR_ALIGNED(ptr, pool) \
    (((uint8_t *)ptr - pool) % BLOCK_SIZE == 0U)

/**
 * @brief Convert pointer to pool index
 */
#define BLOCK_PTR_2_INDEX(ptr, pool) \
    (((uint8_t *)ptr - pool) / BLOCK_SIZE)

/* Type definitions */

/**
 * @brief Block structure (linked list) containing pointer and data structure
 * Extra care with alignment is required for handling linked lists,
 * compiler might add extra padding so further investigation is required
 * 
 */
typedef struct freeBlock {
    struct freeBlock *next;     // Pointer to next block
    uint8_t data[BLOCK_SIZE];   // Data inside block
} freeBlock_t;

/* Static variables */
static uint8_t  staticPool[BLOCK_SIZE * BLOCK_NUMS];    /* Static memory pool */
static freeBlock_t * freeList;                          /* Free Block List */
static ATOMIC uint8_t blockMux;                         /* Block mutex */

/* Static function prototypes */

/* Public functions */

/**
 * @brief Initialize static pool, used block list and mutex
 * 
 */
void block_init(void)
{
    /* Compile time asserts (sanity checks) */
    COMPILE_TIME_ASSERT((BLOCK_SIZE % 4U) == 0U); /* 4 byte alignment for correct structure alignment */
    COMPILE_TIME_ASSERT((BLOCK_SIZE * BLOCK_NUMS) % (4 + 16) == 0U); // Correct alignment of structure and size needed
    COMPILE_TIME_ASSERT((BLOCK_SIZE > 0U));
    COMPILE_TIME_ASSERT((BLOCK_NUMS > 0U));
    
    /* Initialize static pool to default value */
    BLOCK_MEMSET(staticPool, sizeof(staticPool), 0U);

    /* Initialize linked list */
    freeList = (freeBlock_t *)staticPool;
    for(size_t index = 0; index < BLOCK_NUMS - 1; ++index)
    {
        freeBlock_t * curBlock = (freeBlock_t *)(staticPool + index * BLOCK_SIZE);
        BLOCK_MEMSET(curBlock->data, BLOCK_SIZE, 0U);
        curBlock->next = (freeBlock_t *)(staticPool + (index + 1) * BLOCK_SIZE);
    }
    /* End of head should point to NULL */
    freeBlock_t * lastBlock = (freeBlock_t *)(staticPool + (BLOCK_NUMS - 1) * BLOCK_SIZE);
    lastBlock->next = NULL;

    /* Initialize mutex/locks */
    blockMux = 0U;
}

/**
 * @brief Allocate single block
 * 
 * @return void* Pointer to allocated block
 */
void * block_alloc(void)
{
    freeBlock_t * pAddr = NULL;
    /* Lock allocator */
    MUX_LOCK(&blockMux);

    /* Linked list search - already full */
    if (NULL != freeList)
    {
        pAddr = freeList;
        freeList = freeList->next;
    }
    else
    {
        /* No memory available */
    }

    /* Unlock block allocator */
    MUX_UNLOCK(&blockMux);

    return (void *)pAddr;
}

/**
 * @brief Free single block
 * 
 * @param pBlock Pointer to block
 */
void block_free(void * pBlock)
{
    /* Lock block allocator */
    MUX_LOCK(&blockMux);
    /* NULL Check and pointer alignment verification */
    if((NULL != pBlock) && (IS_PTR_ALIGNED(pBlock, staticPool)))
    {
        freeBlock_t * pAddrFree = (freeBlock_t *)pBlock;
        /* Erase data in the block */
        BLOCK_MEMSET(pAddrFree->data, BLOCK_SIZE, 0U);
        pAddrFree->next = freeList;
        freeList = pAddrFree;
    }   
    else
    {
        /* Do nothing */
    }
    /* Unlock block allocator */
    MUX_UNLOCK(&blockMux);
}

/* Static functions */

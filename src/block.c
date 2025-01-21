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
    for (size_t index = 0U; index < (size); index++) { \
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

/* Static variables */
static uint8_t staticPool[BLOCK_SIZE * BLOCK_NUMS]; /* Static memory pool */
static uint8_t blockUsed[BLOCK_NUMS];               /* Block used flag per block */
static size_t blockNumUsed     = 0U;                /* Number of blocks used */
static ATOMIC uint8_t blockMux = 0U;                /* Block mutex */

/* Static function prototypes */

/* Public functions */

/**
 * @brief Initialize static pool, used block list and mutex
 * 
 */
void block_init(void)
{
    /* Compile time asserts (sanity checks) */
    COMPILE_TIME_ASSERT((BLOCK_SIZE % 4U) == 0U); /* 4 byte alignment */
    COMPILE_TIME_ASSERT((BLOCK_SIZE > 0U));
    COMPILE_TIME_ASSERT((BLOCK_NUMS > 0U));
    /* Initialize blocks to default value */
    BLOCK_MEMSET(staticPool, sizeof(staticPool), 0U);
    BLOCK_MEMSET(blockUsed, sizeof(blockUsed), BLOCK_UNUSED);
    blockNumUsed = 0U;
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
    uint8_t * pAddr = NULL;
    /* Lock allocator */
    MUX_LOCK(&blockMux);
    /* Check if there are free blocks - parse through data only if memory available */
    if ((size_t)BLOCK_NUMS > blockNumUsed) /* Data race issue possible if this variable is not protected with mutex*/
    {
        /* Static pool available */

        /* Linear search - becomes inneficient as the pool grows */
        /* Find first free block */
        for(size_t index = 0U; index < (size_t)BLOCK_NUMS; index++)
        {
            if (BLOCK_UNUSED == blockUsed[index])
            {
                /* Block found */
                blockUsed[index] = BLOCK_USED;
                pAddr = (uint8_t *)&staticPool[index * BLOCK_SIZE];
                blockNumUsed++;
                break;
            }
            else
            {
                /* No free blocks */
            }
        }
    }
    else
    {
        /* No memory available */
    }

    /* Unlock block allocator */
    MUX_UNLOCK(&blockMux);

    return pAddr;
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
        /* Verify double free before proceeding */
        size_t index = BLOCK_PTR_2_INDEX(pBlock, staticPool);
        if (BLOCK_USED == blockUsed[index])
        {
            /* Free block */
            BLOCK_MEMSET(pBlock, BLOCK_SIZE, 0U);
            blockUsed[index] = BLOCK_UNUSED;
            blockNumUsed--; // Underflow not possible
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }
    /* Unlock block allocator */
    MUX_UNLOCK(&blockMux);
}

/* Static functions */

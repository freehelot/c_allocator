#include "unity/unity.h"
#include "unity/unity_internals.h"

/* Standard library includes */
#include <stdio.h>
#include <stdint.h>

/* Filest under test includes */
#include "block.h"


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


typedef struct freeBlock {
    struct freeBlock *next;     // Pointer to next block
    uint8_t data[BLOCK_SIZE];   // Data inside block
} freeBlock_t;


void setUp(void)
{
    /* Only block initialization is needed as all the buffers are cleared on init */
    block_init();
}

void tearDown(void)
{
    /* Do nothing */
}

/* Test if allocation is working */
void test_alloc(void)
{
    // Given
    
    // When
    uint8_t * pBlock = (uint8_t *)block_alloc();
    // Then
    // Expected to not be NULL
    TEST_ASSERT_NOT_EQUAL(NULL, (block_alloc()));
}

/* Test when memory is full */
void test_nomem_alloc(void)
{
    // Given
    size_t num_of_blocks = BLOCK_NUMS; // Defined in CMake
    // Allocation should succeed until end of block size
    for (size_t index = 0U; index < num_of_blocks; index ++)
    {
        TEST_ASSERT_NOT_EQUAL(NULL, block_alloc());
    }
    // When
    uint8_t * pBlock = block_alloc(); // Expected NULL
    // Allocation should fail -> return NULL
    TEST_ASSERT_EQUAL(NULL, pBlock);
}

/* Test deallocation */
void test_dealloc(void)
{
    // Given
    freeBlock_t * pBlock = (freeBlock_t *)block_alloc(); // Casting to correct alignment
    TEST_ASSERT_NOT_EQUAL(NULL, pBlock); // Expected non-null
    pBlock->data[0U]= 0xFFU; // Allocate data
    TEST_ASSERT_EQUAL(0xFFU, pBlock->data[0U]); // Verify data is indeed written
    // When
    block_free((void *)pBlock); // Free Block
    // Then
    TEST_ASSERT_EQUAL(0U, pBlock->data[0U]); // Expected erased block
}

/* Test dealocation and allocation between blocks */
void test_inbetween_alloc(void)
{
    // Given
    freeBlock_t *pBlock = NULL;
    freeBlock_t *pTestBlock = NULL; // Inbetween block
    size_t num_of_blocks = BLOCK_NUMS; // Defined in block.c
    size_t blockSize = BLOCK_SIZE;
    for(size_t index = 0; index <  num_of_blocks; index++)
    {
        pBlock = (freeBlock_t *)block_alloc();
        if((num_of_blocks / 2U) == index)
        {
            pTestBlock = pBlock;
        }
        TEST_ASSERT_NOT_EQUAL(NULL, pBlock);
    }
    // All blocks are allocated, expect null on consecutive call
    TEST_ASSERT_EQUAL(NULL, block_alloc());
    // When
    TEST_ASSERT_NOT_EQUAL(NULL, pTestBlock);
    TEST_ASSERT_NOT_EQUAL(pBlock, pTestBlock);
    pTestBlock->data[0U] = 0xBA; // Allocate some value to buffer
    // Verify data is indeed written
    TEST_ASSERT_EQUAL(0xBA, pTestBlock->data[0U]);
    block_free(pTestBlock);
    // Then
    TEST_ASSERT_EQUAL(0U, pTestBlock->data[0U]);
    // Allocating new block should pass
    TEST_ASSERT_NOT_EQUAL(NULL, block_alloc());

}

/* Non-aligned deallocation */
void test_dealloc_nonaligned(void)
{
    // Given
    freeBlock_t * pBlock = (freeBlock_t *)block_alloc();
    TEST_ASSERT_NOT_EQUAL(NULL, pBlock);
    pBlock->data[0U] = 0xFFU; // Alocate value to first byte of block
    TEST_ASSERT_EQUAL(0xFFU, pBlock->data[0U]); // Verify data is indeed written    
    // When
    block_free(pBlock + 1U); // Incorrect alignment given to free
    // Then
    TEST_ASSERT_EQUAL(0xFFU, pBlock->data[0U]); // Data should still persist 
}

int main(void)
{
    UNITY_BEGIN();
    /* Test list to run */
    RUN_TEST(test_alloc);
    RUN_TEST(test_nomem_alloc);
    RUN_TEST(test_dealloc);
    RUN_TEST(test_inbetween_alloc);
    RUN_TEST(test_dealloc_nonaligned);
    return UNITY_END();
}
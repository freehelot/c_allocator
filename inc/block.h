/**
 * @file block.h
 * @author Hrvoje Z
 * @brief Block allocator header file
 * @version 0.1
 * @date 2025-01-20
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef BLOCK_H
#define BLOCK_H

/* Includes */

/* Macros and Constants */

/* Public function prototypes */

void block_init(void);

void * block_alloc(void);

void block_free(void * pBlock);

#endif // BLOCK_H

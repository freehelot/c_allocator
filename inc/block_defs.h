/**
 * @file block_defs.h
 * @author Hrvoje Z 
 * @brief Project specific definitions header file
 * @version 0.1
 * @date 2025-01-20
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef BLOCK_DEFS_H
#define BLOCK_DEFS_H

#ifndef EMBEDDED_TARGET
/* If not running on embedded target, use standard library */
#include <stdio.h>
#include <stdatomic.h>
#include <stdint.h>

/* Compile time assert*/
#define COMPILE_TIME_ASSERT(condition) _Static_assert(condition, "Compile-time assertion failed")
/* "Mutex" atomic operations */
#define MUX_LOCK(mux)   while(atomic_exchange(mux, 1) == 1){}
#define MUX_UNLOCK(mux) atomic_store(mux, 0)
#define ATOMIC _Atomic               
#else 
/* Define definitions for target e.g. mutex lock, compile time asserts*/
#define COMPILE_TIME_ASSERT(condition) () \ // To be defined depending on target/compiler
#define MUX_LOCK(mux) () \ // To be defined depending on target e.g. RTOS/other
#define MUX_UNLOCK(mux) () \ // To be defined depending on target e.g. RTOS/other
#define ATOMIC

typedef unsigned char uint8_t; // Support for uint8_t if not defined

#endif


#endif // BLOCK_DEFS_H

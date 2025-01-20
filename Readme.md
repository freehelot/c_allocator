# Static allocator project

## Task
Develop a block memory allocator in C.

Allocator should allocate and release one block with fixed size from static pool.

The block size and number of blocks are fixed at runtime, but configurable during project build.

The module should be adapted to work on Embedded platforms of different bit sizes in multitasking environment.

The module must have a set of unit-tests.

## Requirements
Built on gcc version 11.4.0 (Ubuntu 11.4.0-1ubuntu1~22.04) 

Unit Test library provided in ```test/unity``` as source file includes, more information available on https://www.throwtheswitch.org/unity

## Usage
### Build as library
```
mkdir build
cd build
cmake ..
make
```
### Build for Embedded Target
```
mkdir build
cd build
cmake -DEMBEDDED_TARGET=ON ..
make
```
### Build Unit Tests
```
mkdir build
cd build
cmake -DBUILD_UNIT_TESTS_ON
make
```

### Run Unit Tests
While being positioned in ```build``` folder, run ```ctest --verbose```.

### Static allocator configuration
#### Sizes
Sizes are configurable in CMakeLists.txt in root folder with ```-DALLOC_BLOCK_SIZE=32 -DALLOC_NUM_BLOCKS=10```

#### Embedded target specific
```block_defs.h``` file has been prepared for inclusion of allocator implementation as a library. There, it is possible to define mutex and compile time assert macros for specific targets or compilers.

```size_t``` has been used since width of uint/int is unclear and for maximum compatibility with embedded targets.

If ```uint8_t``` is not defined for embedded target, typedef is availabled where it is defined as ```unsigned char``` to avoid potential issues.

Macros for mutex lock/unlock have been empty as it is really target and OS specific and can be easily configured using the mentioned header file.
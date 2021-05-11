#pragma once

// 64-bit integer
typedef unsigned long long uint64_t;
typedef long long int64_t;

// 32-bit integer
typedef unsigned int uint32_t;
typedef int int32_t;

// 16-bit integer
typedef unsigned short uint16_t;
typedef short int16_t;

// 8-bit integer
typedef unsigned char uint8_t;
typedef char int8_t;
typedef unsigned char byte;
typedef unsigned int dword;
typedef unsigned int uint;
typedef unsigned short word;
// size
typedef unsigned long int size_t;

// integer point
typedef struct
{
    int32_t X;
    int32_t Y;
} __attribute__((packed)) point_t;

// integer rectangle
typedef struct
{
    int32_t X;
    int32_t Y;
    int32_t Width;
    int32_t Height;
} __attribute__((packed)) bounds_t;
typedef uint64_t                 uintptr_t;
// null
#define NULL 0

// bitwise
#define bitutil_low_16(address) (uint16_t)((address) & 0xFFFF)
#define bitutil_high_16(address) (uint16_t)(((address) & 0xFFFF0000) >> 16)

// unused
#define UNUSED(x) (void)(x)

static inline bool bounds_contains(bounds_t* bounds, int32_t x, int32_t y) { return (x >= bounds->X && y >= bounds->Y && x < bounds->X + bounds->Width && y < bounds->Y + bounds->Height); }
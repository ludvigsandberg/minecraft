#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#define TRUE  1
#define FALSE 0

typedef unsigned char bool;

typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef signed long s64;
typedef unsigned long u64;
typedef float f32;
typedef double f64;

typedef char static_assert_bool[(sizeof(bool) == 1) ? 1 : -1];
typedef char static_assert_u8[(sizeof(u8) == 1) ? 1 : -1];
typedef char static_assert_u16[(sizeof(u16) == 2) ? 1 : -1];
typedef char static_assert_u32[(sizeof(u32) == 4) ? 1 : -1];
typedef char static_assert_u64[(sizeof(u64) == 8) ? 1 : -1];
typedef char static_assert_f32[(sizeof(f32) == 4) ? 1 : -1];
typedef char static_assert_f32[(sizeof(f64) == 8) ? 1 : -1];

#endif
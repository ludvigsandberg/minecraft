/*****************************************************************************
 * File:        pp.h
 * Author:      ludvigsandberg
 * Date:        2026-06-20
 * Description: Preprocessor utilities.
 *****************************************************************************/

#ifndef COMMON_PP_H
#define COMMON_PP_H

#define CONCAT_IMPL(A, B)    A##B
#define CONCAT(A, B)         CONCAT_IMPL(A, B)
#define CONCAT_3(A, B, C)    CONCAT(CONCAT(A, B), C)
#define CONCAT_4(A, B, C, D) CONCAT(CONCAT_3(A, B, C), D)

#define MIN(A, B) (((A) < (B)) ? (A) : (B))
#define MAX(A, B) (((A) > (B)) ? (A) : (B))

#endif
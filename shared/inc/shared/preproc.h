#ifndef SHARED_PREPROC_H
#define SHARED_PREPROC_H

#define CONCAT(A, B)      CONCAT_IMPL(A, B)
#define CONCAT_IMPL(A, B) A##B

#define MIN(A, B) (((A) < (B)) ? (A) : (B))
#define MAX(A, B) (((A) > (B)) ? (A) : (B))

#endif
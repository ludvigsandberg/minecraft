#ifndef MACROS_H
#define MACROS_H

#define TRUE  1
#define FALSE 0

#define INDEX_2D(X, Y, LEN)    ((Y) * (LEN) + (X))
#define INDEX_3D(X, Y, Z, LEN) ((Z) * (LEN) * (LEN) + (Y) * (LEN) + (X))

#define MIN(A, B) (((A) < (B)) ? (A) : (B))
#define MAX(A, B) (((A) > (B)) ? (A) : (B))

#endif
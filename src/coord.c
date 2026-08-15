#include "coord.h"

struct coord coord_add(const struct coord *a, const struct coord *b) {
    struct coord coord;
    coord.x = a->x + b->x;
    coord.y = a->y + b->y;
    coord.z = a->z + b->z;
    return coord;
}

struct coord coord_sub(const struct coord *a, const struct coord *b) {
    struct coord coord;
    coord.x = a->x - b->x;
    coord.y = a->y - b->y;
    coord.z = a->z - b->z;
    return coord;
}
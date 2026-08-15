#ifndef COORD_H
#define COORD_H

struct coord {
    int x;
    int y;
    int z;
};

struct coord coord_add(const struct coord *a, const struct coord *b);
struct coord coord_sub(const struct coord *a, const struct coord *b);

#endif

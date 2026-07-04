/*****************************************************************************
 * File:        que_$NAME$.c
 * Author:      ludvigsandberg
 * Date:        2026-07-04
 * Description: DO NOT EDIT. Generated from common/template/que.c.
 *****************************************************************************/

#include "$SELF_INCLUDE$"

#include <stdlib.h>
#include <string.h>

void que_$NAME$_new(que_$NAME$_t *q) {
    q->data = NULL;
    q->head = 0;
    q->size = 0;
    q->cap  = 0;
}

void que_$NAME$_free(que_$NAME$_t *q) {
    free(q->data);
}

$TYPE$ *que_$NAME$_front(const que_$NAME$_t *q) {
    return q->data + q->head;
}

size_t que_$NAME$_size(const que_$NAME$_t *q) {
    return q->size;
}

void que_$NAME$_push(que_$NAME$_t *q, $TYPE$ val) {
    size_t nc;
    size_t front_count;
    void *nd;

    if (q->size == q->cap) {
        nc = q->cap ? q->cap * 2 : 4;
        nd = malloc(nc * sizeof(*q->data));
        if (!nd)
            abort();
        if (q->size > 0) {
            front_count = q->cap - q->head;
            if (front_count > q->size)
                front_count = q->size;
            memcpy(nd, q->data + q->head, front_count * sizeof(*q->data));
            if (q->size > front_count)
                memcpy((char *)nd + front_count * sizeof(*q->data), q->data,
                       (q->size - front_count) * sizeof(*q->data));
        }
        free(q->data);
        q->data = nd;
        q->head = 0;
        q->cap  = nc;
    }
    q->data[(q->head + q->size) % q->cap] = val;
    q->size++;
}

void que_$NAME$_pop(que_$NAME$_t *q) {
    q->head = (q->head + 1) % q->cap;
    q->size--;
}

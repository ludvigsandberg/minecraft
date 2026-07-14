/*****************************************************************************
 * File:        que_$NAME$.h
 * Author:      ludvigsandberg
 * Date:        2026-07-04
 * Description: DO NOT EDIT. Generated from common/template/que.h.
 *****************************************************************************/

#ifndef $PROJECT$_QUE_$UPPER_NAME$_H
#define $PROJECT$_QUE_$UPPER_NAME$_H

#include <stddef.h>

$TYPE_INCLUDE$

typedef struct que_$NAME$_s {
    $TYPE$ *data;
    size_t head;
    size_t size;
    size_t cap;
} que_$NAME$_t;

void que_$NAME$_new(que_$NAME$_t *q);
void que_$NAME$_free(que_$NAME$_t *q);

$TYPE$ *que_$NAME$_front(const que_$NAME$_t *q);
size_t que_$NAME$_size(const que_$NAME$_t *q);

void que_$NAME$_push(que_$NAME$_t *q, $TYPE$ val);
void que_$NAME$_pop(que_$NAME$_t *q);

#endif

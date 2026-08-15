#ifndef CLIENT_H
#define CLIENT_H

#include <pthread.h>

#include "array.h"
#include "queue.h"

/* Thread pool task. */
struct task {
    void (*function)(void *context);
    void *context;
    struct task *next;
};

struct client {
    /* Thread pool. */

    struct array threads;
    pthread_t *thread_elems;

    pthread_mutex_t mutex;
    pthread_cond_t cond;

    struct queue tasks;
    struct task *task_elems;
};

void client_init(struct client *client);
void client_update(struct client *client);

#endif

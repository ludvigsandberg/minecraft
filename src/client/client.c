#include "client/client.h"

#include <unistd.h>

#include "macros.h"

static void worker(void *context) {
    struct client *client;

    assert(context);

    client = (struct client *)context;

    while (TRUE) {
        struct task task;

        pthread_mutex_lock(&client->mutex);

        while (client->tasks.size == 0) {
            pthread_cond_wait(&client->cond, &client->mutex);
        }

        QUEUE_POP(&client->tasks, &task);

        pthread_mutex_unlock(&client->mutex);

        assert(task.function);
        task.function(task.context);
    }
}

void client_init(struct client *client) {
    long cores;
    int workers;
    int i;

    /* Setup thread pool. */

    cores   = sysconf(_SC_NPROCESSORS_ONLN);
    cores   = MAX(cores, 2);
    workers = (int)MIN(cores - 1, 8);

    printf("Thread pool contains %d worker thread(s).\r\n", workers);

    pthread_mutex_init(&client->mutex, NULL);
    pthread_cond_init(&client->cond, NULL);

    for (i = 0; i < workers; i++) {
        pthread_create(&client->thread_elems[i], NULL, worker, client);
    }
}
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <errno.h>

#include "macros.h"
#include "array.h"
#include "vector.h"

#define PORT 2000

#pragma pack(push, 1)
typedef struct client_update_pkt_s {
    int sender_socket;
    vec3_t position;
    vec3_t velocity;
    float head_yaw;
    float head_pitch;
} client_update_pkt_t;
#pragma pack(pop)

typedef struct client_s {
    int socket;

    unsigned char *in;
    size_t in_count;
    size_t in_cap;

    unsigned char *out;
    size_t out_count;
    size_t out_cap;

    char ip[INET_ADDRSTRLEN];
} client_t;

static int set_nonblocking(int socket) {
    int flags = fcntl(socket, F_GETFL, 0);

    if (flags == -1) {
        return FALSE;
    }

    flags |= O_NONBLOCK;

    if (fcntl(socket, F_SETFL, flags) == -1) {
        return FALSE;
    }

    return TRUE;
}

int main(int argc, char **argv) {
    int listener;

    client_t *clients   = NULL;
    size_t client_count = 0;
    size_t client_cap   = 0;

    int yes = 1;
    struct sockaddr_in addr;

    struct pollfd *polled_sockets = NULL;
    size_t polled_sockets_count   = 0;
    size_t polled_sockets_cap     = 0;

    struct pollfd fd;

    size_t i;

    (void)argc;
    (void)argv;

    listener = socket(AF_INET, SOCK_STREAM, 0);

    if (!listener) {
        printf("Failed to create listener socket.\r\n");
        exit(EXIT_FAILURE);
    }

    if (!set_nonblocking(listener)) {
        printf("Failed to set listener socket to non-blocking.\r\n");
        exit(EXIT_FAILURE);
    }

    /* Allow socket to be reusable. Avoids address-in-use error. */
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        printf("Failed to bind listener socket.\r\n");
        exit(EXIT_FAILURE);
    }

    if (listen(listener, 999) == -1) {
        printf("Failed to listen.\r\n");
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %d\r\n", PORT);

    while (TRUE) {
        fd.fd     = listener;
        fd.events = POLLIN;

        ARR_APPEND(polled_sockets, polled_sockets_count, polled_sockets_cap,
                   &fd);

        for (i = 0; i < client_count; i++) {
            fd.fd = clients[i].socket;

            ARR_APPEND(polled_sockets, polled_sockets_count,
                       polled_sockets_cap, &fd);
        }

        /* While there are no connected clients, wait indefinitely (-1). */
        switch (poll(polled_sockets, (nfds_t)polled_sockets_count,
                     client_count == 0 ? -1 : 0)) {
            case -1:
                printf("Poll error.\r\n");
                exit(EXIT_FAILURE);

            /* Timeout. No events. */
            case 0:
                goto no_events;
        }

        /* Check listener socket for incoming connection. */
        if (polled_sockets[0].revents & POLLIN) {
            struct sockaddr_storage addr_storage;
            socklen_t len = sizeof addr_storage;
            int socket;
            client_t client;

            /* Accept connection. */

            socket = accept(listener, (struct sockaddr *)&addr_storage, &len);

            if (socket == -1) {
                goto skip_new_connection;
            }

            if (!set_nonblocking(socket)) {
                printf("Failed to set client socket to non-blocking. "
                       "Disconnecting client.");
                close(socket);
                goto skip_new_connection;
            }

            /* Add client to array. */

            client.socket = socket;

            if (!inet_ntop(addr_storage.ss_family,
                           &(((struct sockaddr_in *)&addr_storage)->sin_addr),
                           client.ip, INET_ADDRSTRLEN)) {
                close(socket);
                printf("inet_ntop() failed.\r\n");
                goto skip_new_connection;
            }

            client.in        = NULL;
            client.in_count  = 0;
            client.in_cap    = 0;
            client.out       = NULL;
            client.out_count = 0;
            client.out_cap   = 0;

            ARR_APPEND(clients, client_count, client_cap, &client);

            printf("Client connected (%s).\r\n", client.ip);
        }

    skip_new_connection:

        for (i = 1; i < polled_sockets_count; i++) {
            if (polled_sockets[i].revents & POLLIN) {
                char buf[1024];
                int len;
                client_t *client = NULL;
                size_t j;

                len = (int)recv(polled_sockets[i].fd, buf, sizeof buf, 0);

                /* No data to read. */
                if (len == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                    continue;
                }

                /* Find client. */

                for (j = 0; j < client_count; j++) {
                    if (clients[j].socket == polled_sockets[i].fd) {
                        client = &clients[j];
                    }
                }

                assert(client);

                ARR_APPEND_N(client->in, client->in_count, client->in_cap,
                             (size_t)len, buf);

                /* Handle packets. */
                while (client->in_count >= sizeof(client_update_pkt_t)) {
                    client_update_pkt_t pkt;
                    size_t k;

                    memcpy(&pkt, client->in, sizeof(client_update_pkt_t));

                    pkt.sender_socket = client->socket;

                    for (k = 0; k < client_count; k++) {
                        if (clients[k].socket != client->socket) {
                            ARR_APPEND_N(clients[k].out, clients[k].out_count,
                                         clients[k].out_cap,
                                         sizeof(client_update_pkt_t), &pkt);
                        }
                    }

                    ARR_REMOVE_N(client->in, client->in_count, 0,
                                 sizeof(client_update_pkt_t));
                }
            }
        }

    no_events:

        polled_sockets_count = 0;

        /* Send outgoing data to clients. */
        for (i = 0; i < client_count; i++) {
            if (clients[i].out_count > 0) {
                ssize_t num_bytes_sent =
                    send(clients[i].socket, clients[i].out,
                         clients[i].out_count, 0);

                if (num_bytes_sent != -1) {
                    ARR_REMOVE_N(clients[i].out, clients[i].out_count, 0,
                                 (size_t)num_bytes_sent);
                }
            }
        }
    }

    return 0;
}
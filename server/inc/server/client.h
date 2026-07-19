#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include <arpa/inet.h>

#include "server/arr_u8.h"

typedef struct client_s {
    int socket;

    arr_u8_t in;
    arr_u8_t out;

    char ip[INET_ADDRSTRLEN];
} client_t;

#endif

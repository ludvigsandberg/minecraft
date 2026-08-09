#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <errno.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <glad/glad.h>
#include <GL/glx.h>

#include "macros.h"
#include "array.h"
#include "client/system.h"
#include "client/renderer.h"
#include "client/opengl.h"
#include "client/chunk.h"
#include "client/world.h"
#include "client/camera.h"

#pragma pack(push, 1)
typedef struct client_update_pkt_s {
    int sender_socket;
    struct vector3 position;
    struct vector3 velocity;
    float head_yaw;
    float head_pitch;
} client_update_pkt_t;
#pragma pack(pop)

typedef struct remote_player_s {
    int socket;
    struct vector3 position;
    struct vector3 velocity;
    float head_yaw;
    float head_pitch;
} remote_player_t;

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
    int multiplayer = FALSE;
    char *host;
    int port;

    window_t window;
    renderer_t renderer;
    camera_t camera;
    world_t world;
    double last_time;

    int server_socket;
    struct sockaddr_in server_addr;

    unsigned char *in_buf = NULL;
    size_t in_count       = 0;
    size_t in_cap         = 0;

    remote_player_t *remote_players = NULL;
    size_t remote_player_count      = 0;
    size_t remote_player_cap        = 0;

    struct vector3 last_pos;

    /* Parse server hostname and port. */
    if (argc >= 3) {
        host        = argv[1];
        port        = atoi(argv[2]);
        multiplayer = TRUE;
    }

    if (multiplayer) {
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket == -1) {
            printf("Failed to create socket.\r\n");
            exit(EXIT_FAILURE);
        }

        printf("Connecting to %s:%d...\r\n", host, port);

        server_addr.sin_family = AF_INET;
        server_addr.sin_port   = htons((unsigned short)port);
        inet_pton(AF_INET, host, &server_addr.sin_addr);

        if (connect(server_socket, (struct sockaddr *)&server_addr,
                    sizeof(server_addr)) == -1) {
            printf("Failed to connect to server.\r\n");
            exit(EXIT_FAILURE);
        }

        if (!set_nonblocking(server_socket)) {
            printf("Failed to set socket to non-blocking.\r\n");
            exit(EXIT_FAILURE);
        }
    } else {
        printf("Launching in singleplayer mode.\r\n");
        printf("To connect to a server, start the client with the server "
               "address and port.\r\n");
        printf("Example: ./client <address> <port>\r\n");
    }

    window_init(&window);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(opengl_debug_callback, NULL);
#endif

    camera_init(&camera);
    world_init(&world);
    renderer_init(&renderer);

    last_pos  = camera.pos;
    last_time = elapsed_time_seconds();

    while (TRUE) {
        double current_time;
        float delta_time;
        int vertical = 16;
        char recv_buf[1024];
        ssize_t bytes_received;
        size_t i;

        /* Network Receive */

        if (multiplayer) {
            bytes_received =
                recv(server_socket, recv_buf, sizeof(recv_buf), 0);
            if (bytes_received > 0) {
                ARR_APPEND_N(in_buf, in_count, in_cap, (size_t)bytes_received,
                             recv_buf);

                while (in_count >= sizeof(client_update_pkt_t)) {
                    client_update_pkt_t pkt;
                    int found = FALSE;

                    memcpy(&pkt, in_buf, sizeof(client_update_pkt_t));

                    for (i = 0; i < remote_player_count; i++) {
                        if (remote_players[i].socket == pkt.sender_socket) {
                            remote_players[i].position   = pkt.position;
                            remote_players[i].velocity   = pkt.velocity;
                            remote_players[i].head_yaw   = pkt.head_yaw;
                            remote_players[i].head_pitch = pkt.head_pitch;

                            found = TRUE;
                            break;
                        }
                    }

                    if (!found) {
                        remote_player_t rp;
                        rp.socket     = pkt.sender_socket;
                        rp.position   = pkt.position;
                        rp.velocity   = pkt.velocity;
                        rp.head_yaw   = pkt.head_yaw;
                        rp.head_pitch = pkt.head_pitch;
                        ARR_APPEND(remote_players, remote_player_count,
                                   remote_player_cap, &rp);
                    }

                    ARR_REMOVE_N(in_buf, in_count, 0,
                                 sizeof(client_update_pkt_t));
                }
            }
        }

        /* Update. */

        window_update(&window);

        current_time = elapsed_time_seconds();
        delta_time   = (float)(current_time - last_time);
        last_time    = current_time;

        camera_update(&camera, &window, delta_time);

        world_update(&world, &camera);

        /* Network Send */
        if (multiplayer) {
            if (delta_time > 0.0f) {
                client_update_pkt_t pkt;

                pkt.sender_socket = -1;
                pkt.position      = camera.pos;
                pkt.velocity.VEC_X =
                    (camera.pos.VEC_X - last_pos.VEC_X) / delta_time;
                pkt.velocity.VEC_Y =
                    (camera.pos.VEC_Y - last_pos.VEC_Y) / delta_time;
                pkt.velocity.VEC_Z =
                    (camera.pos.VEC_Z - last_pos.VEC_Z) / delta_time;
                pkt.head_yaw   = camera.yaw;
                pkt.head_pitch = camera.pitch;

                send(server_socket, &pkt, sizeof(client_update_pkt_t), 0);

                last_pos = camera.pos;
            }
        }

        /* Draw. */

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer_draw_sky(&renderer, &camera);

        renderer_draw_world(&renderer, &world, &camera);

        if (multiplayer) {
            for (i = 0; i < remote_player_count; i++) {
                renderer_draw_player(&renderer, &remote_players[i].position,
                                     &remote_players[i].velocity,
                                     remote_players[i].head_yaw,
                                     remote_players[i].head_pitch, &camera);
            }
        }

        if (multiplayer) {
            renderer_gui_draw_text(&renderer, 10, 10, "Minecraft Multiplayer");
        } else {
            renderer_gui_draw_text(&renderer, 10, 10,
                                   "Minecraft Singleplayer");
        }

        renderer_gui_draw_text(&renderer, 10, 10, "Minecraft");
        renderer_gui_draw_text(&renderer, 10, 10 + vertical, "Frame %.1fms",
                               (double)(delta_time * 1000.0f));

        renderer_gui_draw_text(&renderer, 10, 10 + 3 * vertical, "X %.2f",
                               (double)camera.pos.VEC_X);
        renderer_gui_draw_text(&renderer, 10, 10 + 4 * vertical, "Y %.2f",
                               (double)camera.pos.VEC_Y);
        renderer_gui_draw_text(&renderer, 10, 10 + 5 * vertical, "Z %.2f",
                               (double)camera.pos.VEC_Z);

        renderer_gui_draw_text(&renderer, 10, 10 + 7 * vertical, "Yaw %.2f",
                               (double)camera.yaw);
        renderer_gui_draw_text(&renderer, 10, 10 + 8 * vertical, "Pitch %.2f",
                               (double)camera.pitch);

        renderer_gui_flush(&renderer, &camera);

        glXSwapBuffers(window.display, window.handle);
    }
}

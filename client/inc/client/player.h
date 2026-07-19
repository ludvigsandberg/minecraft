/*****************************************************************************
 * File:        player.h
 * Author:      ludvigsandberg
 * Date:        2026-07-19
 * Description: -
 *****************************************************************************/

#ifndef CLIENT_PLAYER_H
#define CLIENT_PLAYER_H

#include "common/vec.h"
#include "client/camera.h"

void player_new(void);
void player_draw(const vec3_t *position, float yaw, const vec3_t *velocity,
                 float dt, const camera_t *camera);
void player_free(void);

#endif

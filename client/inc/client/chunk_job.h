/*****************************************************************************
 * File:        chunk_job.h
 * Author:      ludvigsandberg
 * Date:        2026-06-20
 * Description: -
 *****************************************************************************/

#ifndef CLIENT_CHUNK_JOB_H
#define CLIENT_CHUNK_JOB_H

#include "common/coord.h"

typedef struct {
    coord_t coord; /* Chunk coord, not world coord. */
} chunk_job_t;

#endif

#ifndef PRISMA_BACKEND_H
#define PRISMA_BACKEND_H

#include "prisma/error.h"

struct prisma_request
{
    void (*cb) (void *args);
    void *args;
};

enum prisma_error prisma_backend_init(void);

void prisma_backend_destroy(void);

enum prisma_error prisma_backend_queue_request(struct prisma_request *request);

#endif /* PRISMA_BACKEND_H */
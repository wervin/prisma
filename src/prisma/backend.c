#include <pthread.h>
#include <stdio.h>
#include <time.h>

#include <sake/macro.h>
#include <sake/vector.h>

#include "prisma/backend.h"
#include "prisma/window.h"
#include "prisma/log.h"

struct _backend
{
    struct prisma_request *requests;
};

struct _backend _backend = {0};

static void* _run(void *args);

enum prisma_error prisma_backend_init(void)
{
    _backend.requests = sake_vector_new(sizeof(struct prisma_request), NULL);
    if (!_backend.requests)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_MEMORY, "Cannot allocate requests vector");
        return PRISMA_ERROR_MEMORY;
    }
    
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, _run, NULL);
    pthread_detach(thread_id);
    return PRISMA_ERROR_NONE;
}

void prisma_backend_destroy(void)
{
    sake_vector_free(_backend.requests);
}

enum prisma_error prisma_backend_queue_request(struct prisma_request *request)
{
    _backend.requests = sake_vector_push_back(_backend.requests, request);
    if (!_backend.requests)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_MEMORY, "Cannot allocate pushback request");
        return PRISMA_ERROR_MEMORY;
    }
    return PRISMA_ERROR_NONE;
}

static void* _run(void *args)
{
    SAKE_MACRO_UNUSED(args);

    while (!prisma_window_should_close())
    {
        while (sake_vector_size(_backend.requests))
        {
            struct prisma_request *request = &_backend.requests[0];
            request->cb(request->args);
            sake_vector_erase(_backend.requests, 0);
        }

        /* Sleep during 100 ms */
        struct timespec ts = {0};
        ts.tv_sec = 0;
        ts.tv_nsec = 100000000;
        nanosleep(&ts, NULL);
    }

    pthread_exit(NULL);
}
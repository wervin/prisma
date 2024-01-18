#ifndef PRISMA_LOG_H
#define PRISMA_LOG_H

#include <stdio.h>
#include <stdint.h>

#include "prisma/error.h"

#define PRISMA_LOG_INFO(...) prisma_log(stdout, PRISMA_LOG_LEVEL_INFO, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define PRISMA_LOG_ERROR(error, msg) prisma_log(stderr, PRISMA_LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, "[%s] %s\n", prisma_error_str(error), msg)
#define PRISMA_LOG_ERROR_INFO(...) prisma_log(stderr, PRISMA_LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)

#ifdef NDEBUG
#define PRISMA_LOG_DEBUG(...)
#else
#define PRISMA_LOG_DEBUG(...) prisma_log(stderr, PRISMA_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)
#endif

#define PRISMA_LOG_LEVEL                  \
    X(PRISMA_LOG_LEVEL_INFO, 0, "INFO")   \
    X(PRISMA_LOG_LEVEL_ERROR, 1, "ERROR") \
    X(PRISMA_LOG_LEVEL_DEBUG, 2, "DEBUG")

enum prisma_log_level
{
#define X(def, id, str) def=id,

    PRISMA_LOG_LEVEL

#undef X
};

void prisma_log(FILE *stream, enum prisma_log_level level, const char *file, uint32_t line, const char *func, const char *format, ...);
const char * prisma_log_level_str(enum prisma_log_level level);

#endif /* PRISMA_LOG_H */
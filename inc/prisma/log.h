#ifndef PRISMA_LOG_H
#define PRISMA_LOG_H

#include <stdio.h>
#include <stdint.h>

#include "prisma/error.h"

#define PRISMA_LOG_INFO(...) prisma_log(PRISMA_LOG_LEVEL_INFO, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define PRISMA_LOG_ERROR(...) prisma_log(PRISMA_LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define PRISMA_LOG_ERROR_INFO(error, msg) prisma_log(PRISMA_LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, "[%s] %s\n", prisma_error_str(error), msg)

#ifdef NDEBUG
#define PRISMA_LOG_DEBUG(...)
#else
#define PRISMA_LOG_DEBUG(...) prisma_log(PRISMA_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)
#endif

#define PRISMA_LOG_LEVEL                      \
    X(PRISMA_LOG_LEVEL_CRITICAL, 0, "CRITICAL")     \
    X(PRISMA_LOG_LEVEL_ERROR, 1, "ERROR")     \
    X(PRISMA_LOG_LEVEL_WARNING, 2, "WARNING") \
    X(PRISMA_LOG_LEVEL_INFO, 3, "INFO")       \
    X(PRISMA_LOG_LEVEL_DEBUG, 4, "DEBUG")

struct prisma_logger;

enum prisma_log_level
{
#define X(def, id, str) def=id,

    PRISMA_LOG_LEVEL

#undef X
};

void prisma_log(enum prisma_log_level level, const char *file, uint32_t line, const char *func, const char * format, ...);
void prisma_log_register_gui_logger(struct prisma_logger *logger);
void prisma_log_unregister_gui_logger(void);
const char * prisma_log_level_str(enum prisma_log_level level);

#endif /* PRISMA_LOG_H */
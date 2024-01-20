#include <stdlib.h>

#include <cimgui.h>

#include <sake/macro.h>

#include "prisma/components/logger.h"

#include "prisma/log.h"

struct prisma_logger * prisma_logger_new(void)
{
    struct prisma_logger *logger;

    logger = calloc(1, sizeof(struct prisma_logger));
    if (!logger)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_MEMORY, "Cannot allocate logger");
        return NULL;
    }

    return logger;
}

void prisma_logger_free(struct prisma_logger *logger)
{
    free(logger);
}

enum prisma_error prisma_logger_draw(struct prisma_logger *logger)
{
    SAKE_MACRO_UNUSED(logger);
    return PRISMA_ERROR_NONE;
}
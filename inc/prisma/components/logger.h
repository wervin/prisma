#ifndef PRISMA_COMPONENTS_LOGGER_H
#define PRISMA_COMPONENTS_LOGGER_H

#include "prisma/component.h"

struct prisma_ui;

struct prisma_logger
{
    struct prisma_component base;
    struct prisma_ui *ui;
};

void prisma_logger_set_ui(struct prisma_logger *logger, struct prisma_ui *ui);

#endif /* PRISMA_COMPONENTS_LOGGER_H */
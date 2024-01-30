#ifndef PRISMA_COMPONENTS_LOGGER_H
#define PRISMA_COMPONENTS_LOGGER_H

#include <stdarg.h>
#include <stdint.h>

#include "prisma/component.h"

#include "prisma/error.h"
#include "prisma/log.h"

struct prisma_ui;

typedef struct ImGuiTextBuffer ImGuiTextBuffer;

struct prisma_logger
{
    struct prisma_component base;
    struct prisma_ui *ui;
    ImGuiTextBuffer* text_buffer;
    uint32_t *line_offsets;
};

void prisma_logger_set_ui(struct prisma_logger *logger, struct prisma_ui *ui);
enum prisma_error prisma_logger_add(struct prisma_logger *logger, 
                                    enum prisma_log_level level, 
                                    const char *file, uint32_t line, const char *func,
                                    const char * format, va_list args);

#endif /* PRISMA_COMPONENTS_LOGGER_H */
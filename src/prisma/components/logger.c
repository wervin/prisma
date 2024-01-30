#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#include <cimgui.h>

#include <sake/macro.h>
#include <sake/vector.h>

#include "prisma/components/logger.h"

#include "prisma/log.h"

static enum prisma_error _log(struct prisma_logger *logger, const char* format, ...);
static enum prisma_error _log_va(struct prisma_logger *logger, const char* format,  va_list args);

struct prisma_logger * prisma_logger_new(void)
{
    struct prisma_logger *logger;

    logger = calloc(1, sizeof(struct prisma_logger));
    if (!logger)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot allocate logger");
        return NULL;
    }

    logger->line_offsets = sake_vector_new(sizeof(uint32_t), NULL);
    if (!logger->line_offsets)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot allocate line offsets vector");
        return NULL;
    }

    int32_t initial_offset = 0;
    logger->line_offsets = sake_vector_push_back(logger->line_offsets, &initial_offset);
    if (!logger->line_offsets)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot push back line offset");
        return NULL;
    }

    logger->text_buffer = ImGuiTextBuffer_ImGuiTextBuffer();

    prisma_log_register_gui_logger(logger);

    return logger;
}

void prisma_logger_free(struct prisma_logger *logger)
{
    prisma_log_unregister_gui_logger();
    sake_vector_free(logger->line_offsets);
    ImGuiTextBuffer_destroy(logger->text_buffer);
    free(logger);
}

enum prisma_error prisma_logger_draw(struct prisma_logger *logger)
{
    SAKE_MACRO_UNUSED(logger);

    if (!igBeginChild_Str("Prisma##logger", (ImVec2){0, 0}, ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))
        goto end_child;

    igPushStyleVar_Vec2(ImGuiStyleVar_ItemSpacing, (ImVec2) {0, 0});

    ImGuiListClipper *clipper = ImGuiListClipper_ImGuiListClipper();

    ImGuiListClipper_Begin(clipper, sake_vector_size(logger->line_offsets), -1.0f);

    const char* buf = ImGuiTextBuffer_begin(logger->text_buffer);
    const char* buf_end = ImGuiTextBuffer_end(logger->text_buffer);
    while (ImGuiListClipper_Step(clipper))
    {
        for (int32_t line_no = clipper->DisplayStart; line_no < clipper->DisplayEnd; line_no++)
        {
            const char *line_start = buf + logger->line_offsets[line_no];
            const char *line_end = (line_no + 1 < (int32_t) sake_vector_size(logger->line_offsets)) ? (buf + (int32_t) logger->line_offsets[line_no + 1] - 1) : buf_end;
            igTextUnformatted(line_start, line_end);
        }
    }
    ImGuiListClipper_End(clipper);
    ImGuiListClipper_destroy(clipper);

    igPopStyleVar(1);

    if (igGetScrollY() >= igGetScrollMaxY())
        igSetScrollHereY(1.0f);

end_child:
    igEndChild();
    return PRISMA_ERROR_NONE;
}

void prisma_logger_set_ui(struct prisma_logger *logger, struct prisma_ui *ui)
{
    logger->ui = ui;
}

enum prisma_error prisma_logger_add(struct prisma_logger *logger, 
                                    enum prisma_log_level level, 
                                    const char *file, uint32_t line, const char *func,
                                    const char * format, va_list args)
{
    enum prisma_error error = PRISMA_ERROR_NONE;

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    error = _log(logger, "[%d-%02d-%02d %02d:%02d:%02d] ", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    if (error != PRISMA_ERROR_NONE)
        goto end;

#ifndef NDEBUG
    if (level == PRISMA_LOG_LEVEL_ERROR)
    {
        error = _log(logger, "[%s:%u:%s] ", file, line, func);
        if (error != PRISMA_ERROR_NONE)
            goto end;
    }
#endif
    error = _log(logger, "[%s] ", prisma_log_level_str(level));
    if (error != PRISMA_ERROR_NONE)
        goto end;

    _log_va(logger, format, args);

end:
    return error;
}

static enum prisma_error _log(struct prisma_logger *logger, const char* format, ...)
{
    enum prisma_error error = PRISMA_ERROR_NONE;
    va_list args;
    va_start (args, format);
    error = _log_va(logger, format, args);
    va_end(args);
    return error;
}

static enum prisma_error _log_va(struct prisma_logger *logger, const char* format,  va_list args)
{
    int32_t old_size = ImGuiTextBuffer_size(logger->text_buffer);
    ImGuiTextBuffer_appendfv(logger->text_buffer, format, args);
    for (int new_size = ImGuiTextBuffer_size(logger->text_buffer); old_size < new_size; old_size++)
    {
        if (logger->text_buffer->Buf.Data[old_size] == '\n')
        {
            int32_t offset = old_size + 1;
            logger->line_offsets = sake_vector_push_back(logger->line_offsets, &offset);
            if (!logger->line_offsets)
                return PRISMA_ERROR_MEMORY;
        }
    }

    return PRISMA_ERROR_NONE;
}
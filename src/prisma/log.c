#include <stdarg.h>
#include <time.h>

#include "prisma/components/logger.h"
#include "prisma/log.h"

struct _log
{
    void (*cli) (enum prisma_log_level level, const char *file, uint32_t line, const char *func, const char * format, va_list args);
    enum prisma_error  (*gui) (enum prisma_log_level level, const char *file, uint32_t line, const char *func, const char * format, va_list args);
    struct prisma_logger *gui_logger;
};

static void _fallback(void);
static void _cli_log(enum prisma_log_level level, const char *file, uint32_t line, const char *func, const char * format, va_list args);
static enum prisma_error _gui_log(enum prisma_log_level level, const char *file, uint32_t line, const char *func, const char * format, va_list args);

static struct _log _log = {
    .cli = _cli_log,
    .gui = _gui_log,
    .gui_logger = NULL
};

void prisma_log(enum prisma_log_level level, const char *file, uint32_t line, const char *func, const char * format, ...)
{
    va_list args;
    va_start (args, format);
    _log.cli(level, file, line, func, format, args);
    va_end(args);

    if (_log.gui_logger)
    {   
        va_list args;
        va_start (args, format);
        enum prisma_error error =_log.gui(level, file, line, func, format, args);
        va_end(args);
        if (error != PRISMA_ERROR_NONE)
            _fallback();
    }
}

void prisma_log_register_gui_logger(struct prisma_logger *logger)
{
    _log.gui_logger = logger;
}

void prisma_log_unregister_gui_logger(void)
{
    _log.gui_logger = NULL;
}

const char * prisma_log_level_str(enum prisma_log_level level)
{
    switch (level)
    {
#define X(def,id,str)   \
    case id:            \
        return str;     \
    
    PRISMA_LOG_LEVEL

#undef X

    default:
        return "Unknown level";
    }
}

static void _fallback(void)
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    fprintf(stderr, "[%d-%02d-%02d %02d:%02d:%02d] ", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    fprintf(stderr, "[%s] %s\n", prisma_log_level_str(PRISMA_LOG_LEVEL_CRITICAL), "GUI Logger is not available");
}

static void _cli_log(enum prisma_log_level level, const char *file, uint32_t line, const char *func, const char * format, va_list args)
{
    FILE *stream = level == PRISMA_LOG_LEVEL_INFO ? stdout :stderr;
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    fprintf(stream, "[%d-%02d-%02d %02d:%02d:%02d] ", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    
#ifndef NDEBUG
    if (level == PRISMA_LOG_LEVEL_ERROR)
        fprintf(stream, "[%s:%u:%s] ", file, line, func);
#endif
    
    fprintf(stream, "[%s] ", prisma_log_level_str(level));
    vfprintf (stream, format, args);
}

static enum prisma_error  _gui_log(enum prisma_log_level level, const char *file, uint32_t line, const char *func, const char * format, va_list args)
{
    return prisma_logger_add(_log.gui_logger, level, file, line, func, format, args);
}
#include "prisma/application.h"
#include "prisma/error.h"

int main(void)
{
    enum prisma_error error = PRISMA_ERROR_NONE;
    struct prisma_application application = {0};
    struct prisma_application_info info = {
        .application_name = "Prisma",
        .default_width = 800,
        .default_height = 600,
        .fullscreen = true
    };

    error = prisma_application_init(&application, &info);
    if (error != PRISMA_ERROR_NONE)
        goto clean;

    error = prisma_application_run(&application);
    if (error != PRISMA_ERROR_NONE)
        goto clean;

clean:
    prisma_application_destroy(&application);
    return error;
}
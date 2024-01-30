#include <stdlib.h>

#include <cimgui.h>

#include <sake/macro.h>

#include "prisma/components/info.h"

#include "prisma/log.h"

struct prisma_info * prisma_info_new(void)
{
    struct prisma_info *info;

    info = calloc(1, sizeof(struct prisma_info));
    if (!info)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot allocate info");
        return NULL;
    }

    return info;
}

void prisma_info_free(struct prisma_info *info)
{
    free(info);
}

enum prisma_error prisma_info_draw(struct prisma_info *info)
{
    SAKE_MACRO_UNUSED(info);

    ImGuiIO *io = igGetIO();

    igText("Dear ImGui %s", igGetVersion());
    igText("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
    igText("%d vertices, %d indices (%d triangles)", io->MetricsRenderVertices, io->MetricsRenderIndices, io->MetricsRenderIndices / 3);

    return PRISMA_ERROR_NONE;
}

void prisma_info_set_ui(struct prisma_info *info, struct prisma_ui *ui)
{
    info->ui = ui;
}
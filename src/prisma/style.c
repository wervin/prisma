#include <cimgui.h>

#include "prisma/style.h"

struct prisma_style prisma_style = {0};

void prisma_style_init(void)
{
    prisma_style = (struct prisma_style){0};

    ImGuiIO *io = igGetIO();

    ImFont *large_font = ImFontAtlas_AddFontFromFileTTF(io->Fonts, "assets/fonts/Cousine-Regular.ttf", 18.0f, NULL, NULL);
    ImFont *small_font = ImFontAtlas_AddFontFromFileTTF(io->Fonts, "assets/fonts/Cousine-Regular.ttf", 11.0f, NULL, NULL);
    ImFont *icon_font = ImFontAtlas_AddFontFromFileTTF(io->Fonts, "assets/fonts/fontawesome-webfont.ttf", 18.0f, NULL, NULL);
    io->FontDefault = large_font;

    prisma_style.icon_font = icon_font;
    prisma_style.large_font = large_font;
    prisma_style.small_font = small_font;
}
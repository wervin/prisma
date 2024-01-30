#include <stdlib.h>

#include <cimgui.h>

#include <nfd.h>

#include <sake/macro.h>

#include "prisma/components/menu.h"
#include "prisma/components/editor.h"

#include "prisma/ui.h"
#include "prisma/log.h"
#include "prisma/backend.h"

static void new_file_dialog(void *args);
static void open_file_dialog(void *args);
static void save_as_file_dialog(void *args);

struct prisma_menu * prisma_menu_new(void)
{
    struct prisma_menu *menu;

    menu = calloc(1, sizeof(struct prisma_menu));
    if (!menu)
    {
        PRISMA_LOG_ERROR_INFO(PRISMA_ERROR_MEMORY, "Cannot allocate menu");
        return NULL;
    }

    return menu;
}

void prisma_menu_free(struct prisma_menu *menu)
{
    free(menu);
}

enum prisma_error prisma_menu_draw(struct prisma_menu *menu)
{
    SAKE_MACRO_UNUSED(menu);

    enum prisma_error error = PRISMA_ERROR_NONE;

    if (igBeginMainMenuBar())
    {
        if (igBeginMenu("File", true))
        {
            if (igMenuItem_Bool("New", NULL, false, true))
            {
                struct prisma_request request = {
                    .cb = new_file_dialog,
                    .args = menu
                };
                error = prisma_backend_queue_request(&request);
                if (error != PRISMA_ERROR_NONE)
                    return error;
            }
            if (igMenuItem_Bool("Open", NULL, false, true))
            {
                struct prisma_request request = {
                    .cb = open_file_dialog,
                    .args = menu
                };
                error = prisma_backend_queue_request(&request);
                if (error != PRISMA_ERROR_NONE)
                    return error;
            }
            if (igMenuItem_Bool("Save", NULL, false, true))
            {
            }
            if (igMenuItem_Bool("Save As...", NULL, false, true))
            {
                struct prisma_request request = {
                    .cb = save_as_file_dialog,
                    .args = menu
                };
                error = prisma_backend_queue_request(&request);
                if (error != PRISMA_ERROR_NONE)
                    return error;
            }
            igEndMenu();
        }

        igEndMainMenuBar();
    }

    return error;
}

void prisma_menu_set_ui(struct prisma_menu *menu, struct prisma_ui *ui)
{
    menu->ui = ui;
}

static void new_file_dialog(void *args)
{
    nfdchar_t *path = NULL;
    nfdresult_t result = NFD_SaveDialog("frag", NULL, &path);
    if (result != NFD_OKAY)
        return;
    
    struct prisma_menu *menu = (struct prisma_menu *) args;

    free(path);
}

static void open_file_dialog(void *args)
{
    nfdchar_t *path = NULL;
    nfdresult_t result = NFD_OpenDialog("frag", NULL, &path);
    if (result != NFD_OKAY)
        return;
    
    struct prisma_menu *menu = (struct prisma_menu *) args;
    struct prisma_editor *editor = menu->ui->editor;

    prisma_editor_open(editor, path);
    free(path);
}

static void save_as_file_dialog(void *args)
{
    nfdchar_t *path = NULL;
    nfdresult_t result = NFD_SaveDialog("frag", NULL, &path);
    if (result != NFD_OKAY)
        return;
    
    struct prisma_menu *menu = (struct prisma_menu *) args;

    free(path);
}
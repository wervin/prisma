#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <cimgui.h>
#include <cimgui_impl.h>

#include "prisma/ui.h"
#include "prisma/renderer.h"
#include "prisma/window.h"

#include "prisma/components/editor.h"
#include "prisma/components/logger.h"
#include "prisma/components/menu.h"
#include "prisma/components/viewport.h"

struct ui {
    struct prisma_editor *editor;
    struct prisma_logger *logger;
    struct prisma_menu *menu;
    struct prisma_viewport *viewport;
};

static struct ui _ui = {0};

enum prisma_error prisma_ui_init(void)
{
    enum prisma_error status = PRISMA_ERROR_NONE;

    igCreateContext(NULL);
    ImGuiIO * io = igGetIO();
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    status = primsa_window_init_ui();
    if (status != PRISMA_ERROR_NONE)
        return status;

    status = prisma_renderer_init_ui();
    if (status != PRISMA_ERROR_NONE)
        return status;

    _ui.editor = (struct prisma_editor *) prisma_component_new(PRISMA_COMPONENT_TYPE_EDITOR);
    if (!_ui.editor)
        return PRISMA_ERROR_MEMORY;

    _ui.logger = (struct prisma_logger *)  prisma_component_new(PRISMA_COMPONENT_TYPE_LOGGER);
    if (!_ui.logger)
        return PRISMA_ERROR_MEMORY;

    _ui.menu = (struct prisma_menu *)  prisma_component_new(PRISMA_COMPONENT_TYPE_MENU);
    if (!_ui.menu)
        return PRISMA_ERROR_MEMORY;

    _ui.viewport = (struct prisma_viewport *)  prisma_component_new(PRISMA_COMPONENT_TYPE_VIEWPORT);
    if (!_ui.viewport)
        return PRISMA_ERROR_MEMORY;

    return PRISMA_ERROR_NONE;
}

void prisma_ui_draw(void)
{   
    prisma_window_refresh_ui();
    prisma_renderer_refresh_ui();

    igNewFrame();

    ImGuiWindowFlags flags = 0;
    flags |= ImGuiWindowFlags_NoDocking;
    flags |= ImGuiWindowFlags_NoTitleBar;
    flags |= ImGuiWindowFlags_NoCollapse;
    flags |= ImGuiWindowFlags_NoResize;
    flags |= ImGuiWindowFlags_NoMove;
    flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    flags |= ImGuiWindowFlags_NoNavFocus;

    ImGuiViewport *viewport = igGetMainViewport();
    igSetNextWindowPos(viewport->WorkPos, 0,  (ImVec2) {0, 0});
    igSetNextWindowSize(viewport->WorkSize, 0);
    igSetNextWindowViewport(viewport->ID);

    ImGuiStyle *style = igGetStyle();

    igPushStyleColor_Vec4(ImGuiCol_TitleBgActive, (ImVec4){0, 0, 0, 0});
    igPushStyleColor_Vec4(ImGuiCol_Tab, style->Colors[ImGuiCol_TabUnfocused]);
    igPushStyleColor_Vec4(ImGuiCol_TabHovered, style->Colors[ImGuiCol_TabHovered]);
    igPushStyleColor_Vec4(ImGuiCol_TabActive, style->Colors[ImGuiCol_TabUnfocusedActive]);
    igPushStyleColor_Vec4(ImGuiCol_TabUnfocused, style->Colors[ImGuiCol_TabUnfocused]);
    igPushStyleColor_Vec4(ImGuiCol_TabUnfocusedActive, style->Colors[ImGuiCol_TabUnfocused]);
    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2){0.f, 0.f});

    igBegin("Main", NULL, flags);

    ImGuiID dockspace_id = igGetID_Str("DockSpace");

    if (!igDockBuilderGetNode(dockspace_id))
    {
        igDockBuilderRemoveNode(dockspace_id);
        igDockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_None);

        ImGuiID dock_id = dockspace_id;
        ImGuiID editor_id = igDockBuilderSplitNode(dock_id, ImGuiDir_Left, 0.4f, NULL, &dock_id);
        ImGuiID log_id = igDockBuilderSplitNode(editor_id, ImGuiDir_Down, 0.25f, NULL, &editor_id);
        ImGuiID viewport_id = igDockBuilderSplitNode(dock_id, ImGuiDir_Right, 0.6f, NULL, &dock_id);

        igDockBuilderDockWindow("Editor", editor_id);
        igDockBuilderDockWindow("Log", log_id);
        igDockBuilderDockWindow("Viewport", viewport_id);

        igDockBuilderFinish(dock_id);
    }

    igDockSpace(dockspace_id, (ImVec2) {0.0f, 0.0f}, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoWindowMenuButton, NULL);

    prisma_component_draw((struct prisma_component *) _ui.menu);
  
    igBegin("Editor", NULL, 0);
    prisma_component_draw((struct prisma_component *) _ui.editor);
    igEnd();

    igBegin("Log", NULL, 0);
    prisma_component_draw((struct prisma_component *) _ui.logger);
    igEnd();

    igBegin("Viewport", NULL, 0);
    prisma_component_draw((struct prisma_component *) _ui.viewport);
    igEnd();

    igEnd();
    
    igPopStyleVar(1);
    igPopStyleColor(6);
    
    igRender();
}

void prisma_ui_destroy(void)
{
    if (_ui.editor)
        prisma_component_free((struct prisma_component *) _ui.editor);
    if (_ui.logger)
        prisma_component_free((struct prisma_component *) _ui.logger);
    if (_ui.menu)
        prisma_component_free((struct prisma_component *) _ui.menu);
    if (_ui.viewport)
        prisma_component_free((struct prisma_component *) _ui.viewport);
    
    prisma_renderer_destroy_ui();
    prisma_window_destroy_ui();
    ImGuiContext *context = igGetCurrentContext();
    igDestroyContext(context);
}
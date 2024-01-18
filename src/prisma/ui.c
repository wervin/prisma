#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <cimgui.h>
#include <cimgui_impl.h>

#include "prisma/ui.h"
#include "prisma/renderer.h"
#include "prisma/window.h"

struct _ui
{
    ImGuiContext *imgui_context;
    ImGuiIO *imgui_io;
};

static struct _ui _ui = {0};

enum prisma_error prisma_ui_init(void)
{
    enum prisma_error status = PRISMA_ERROR_NONE;

    _ui.imgui_context = igCreateContext(NULL);
    _ui.imgui_io = igGetIO();
    _ui.imgui_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    status = primsa_window_init_ui();
    if (status != PRISMA_ERROR_NONE)
        return status;

    status = prisma_renderer_init_ui();
    if (status != PRISMA_ERROR_NONE)
        return status;

    status = prisma_renderer_init_ui_viewport();
    if (status != PRISMA_ERROR_NONE)
        return status;

    return PRISMA_ERROR_NONE;
}

void prisma_ui_draw(void)
{   
    ImGuiWindowFlags flags = 0;

    prisma_window_refresh_ui();
    prisma_renderer_refresh_ui();

    igNewFrame();

    flags |= ImGuiWindowFlags_MenuBar;
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

    igBegin("Editor", NULL, 0);

    igEnd();

    igBegin("Log", NULL, 0);

    igEnd();

    prisma_renderer_draw_ui_viewport();

    igEnd();
    
    igPopStyleVar(1);
    igPopStyleColor(6);
    
    igRender();
}

void prisma_ui_destroy(void)
{
    prisma_renderer_destroy_ui_viewport();
    prisma_renderer_destroy_ui();
    prisma_window_destroy_ui();
    igDestroyContext(_ui.imgui_context);
}
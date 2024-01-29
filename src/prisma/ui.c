#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <cimgui.h>
#include <cimgui_impl.h>

#include <sake/vector.h>

#include "prisma/ui.h"
#include "prisma/renderer.h"
#include "prisma/style.h"
#include "prisma/window.h"
#include "prisma/log.h"

#include "prisma/components/editor.h"
#include "prisma/components/logger.h"
#include "prisma/components/menu.h"
#include "prisma/components/info.h"

static struct prisma_ui _ui = {0};

static void _init_imgui(void);

enum prisma_error prisma_ui_init(void)
{
    enum prisma_error status = PRISMA_ERROR_NONE;

    _init_imgui();
    
    prisma_style_init();

    status = primsa_window_init_ui();
    if (status != PRISMA_ERROR_NONE)
        return status;

    status = prisma_renderer_init_ui();
    if (status != PRISMA_ERROR_NONE)
        return status;

    status = prisma_renderer_init_viewport();
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

    _ui.info = (struct prisma_info *)  prisma_component_new(PRISMA_COMPONENT_TYPE_INFO);
    if (!_ui.info)
        return PRISMA_ERROR_MEMORY;

    prisma_editor_set_ui(_ui.editor, &_ui);
    prisma_logger_set_ui(_ui.logger, &_ui);
    prisma_menu_set_ui(_ui.menu, &_ui);
    prisma_info_set_ui(_ui.info, &_ui);

    return PRISMA_ERROR_NONE;
}

enum prisma_error prisma_ui_draw(void)
{   
    enum prisma_error error = PRISMA_ERROR_NONE;

    prisma_window_draw_ui();
    prisma_renderer_draw_ui();

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

    static ImGuiID dockspace_id = 0;
    static ImGuiID dock_id = 0;
    static ImGuiID editor_id = 0;
    static ImGuiID log_id = 0;
    static ImGuiID viewport_id = 0;
    static bool init_docking = true;

    dockspace_id = igGetID_Str("DockSpace");
    if (init_docking)
    {
        igDockBuilderRemoveNode(dockspace_id);
        igDockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_None);
        dock_id = dockspace_id;
        editor_id = igDockBuilderSplitNode(dock_id, ImGuiDir_Left, 0.4f, NULL, &dock_id);
        log_id = igDockBuilderSplitNode(editor_id, ImGuiDir_Down, 0.25f, NULL, &editor_id);
        viewport_id = igDockBuilderSplitNode(dock_id, ImGuiDir_Right, 0.6f, NULL, &dock_id);
        init_docking = false;
    }

    if (sake_vector_size(_ui.editor->contexts))
    {
        for (uint32_t i = 0; i < sake_vector_size(_ui.editor->contexts); i++)
        {
            const char *filename = prisma_editor_context_filename(_ui.editor->contexts[i]);
            igDockBuilderDockWindow(filename, editor_id);
        }
    }
        
    igDockBuilderDockWindow("Log", log_id);
    igDockBuilderDockWindow("Info", log_id);
    igDockBuilderDockWindow("Viewport", viewport_id);

    igDockBuilderFinish(dock_id);

    ImGuiDockNodeFlags dock_flags = 0;
    dock_flags |= ImGuiDockNodeFlags_PassthruCentralNode;
    dock_flags |= ImGuiDockNodeFlags_NoWindowMenuButton;
    dock_flags |= ImGuiDockNodeFlags_NoUndocking;
    igDockSpace(dockspace_id, (ImVec2) {0.0f, 0.0f}, dock_flags, NULL);

    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2){5.f, 5.f});
    error = prisma_component_draw((struct prisma_component *) _ui.menu);
    igPopStyleVar(1);
    if (error != PRISMA_ERROR_NONE)
        goto end;

    error = prisma_component_draw((struct prisma_component *) _ui.editor);
    if (error != PRISMA_ERROR_NONE)
        goto end;

    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2){5.f, 5.f});
    igBegin("Log", NULL, 0);
    error = prisma_component_draw((struct prisma_component *) _ui.logger);
    igEnd();
    igPopStyleVar(1);
    if (error != PRISMA_ERROR_NONE)
        goto end;

    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2){5.f, 5.f});
    igBegin("Info", NULL, 0);
    error = prisma_component_draw((struct prisma_component *) _ui.info);
    igEnd();
    igPopStyleVar(1);
    if (error != PRISMA_ERROR_NONE)
        goto end;

    igBegin("Viewport", NULL, 0);
    error = prisma_renderer_draw_viewport();
    igEnd();
    if (error != PRISMA_ERROR_NONE)
        goto end;

end:
    igEnd();
    igPopStyleVar(1);
    igPopStyleColor(6);

    if (error != PRISMA_ERROR_NONE)    
        return error;
    
    igRender();
    return PRISMA_ERROR_NONE;
}

void prisma_ui_destroy(void)
{
    if (_ui.editor)
        prisma_component_free((struct prisma_component *) _ui.editor);
    if (_ui.logger)
        prisma_component_free((struct prisma_component *) _ui.logger);
    if (_ui.menu)
        prisma_component_free((struct prisma_component *) _ui.menu);
    if (_ui.info)
        prisma_component_free((struct prisma_component *) _ui.info);
    
    prisma_renderer_destroy_viewport();
    prisma_renderer_destroy_ui();
    prisma_window_destroy_ui();
    ImGuiContext *context = igGetCurrentContext();
    igDestroyContext(context);
}

static void _init_imgui(void)
{
    igCreateContext(NULL);
    ImGuiIO * io = igGetIO();
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}
#include "prisma/application.h"

enum prisma_error prisma_application_init(struct prisma_application *application,
                                          struct prisma_application_info *info)
{   
    struct prisma_window_info window_info = {
        .default_width = info->default_width,
        .default_height = info->default_height,
        .fullscreen = info->fullscreen,
        .application_name = info->application_name
    };
    
    prisma_window_init(&application->window, &window_info);

    struct prisma_renderer_info renderer_info = {
        .application_name = info->application_name,
        .application_version_major = info->application_version_major,
        .application_version_minor = info->application_version_minor,
        .application_version_revision = info->application_version_revision,

        .vertex_shader_path = info->vertex_shader_path,
        .frag_shader_path = info->frag_shader_path,

        .max_frames_in_flight = info->max_frames_in_flight, 
        .features_requested = info->features_requested,
        .vk_surface_format = info->vk_surface_format,
        .device_extensions = info->device_extensions,
        .vk_present_mode = info->vk_present_mode,
        .image_count = info->image_count,
        .array_layer_count = info->array_layer_count,
        .transform = info->transform,
        .create_flags = info->create_flags,
        .image_usage_flags = info->image_usage_flags,
        .composite_alpha = info->composite_alpha,
        .clipped = info->clipped,
        .sample_count = info->sample_count,

#ifndef NDEBUG
        .validation_layers = info->validation_layers,
        .debug_extensions = info->debug_extensions
#endif
    };
    prisma_renderer_init(&application->renderer, &application->window, &renderer_info);

    struct prisma_ui_info ui_info = {0};
    prisma_ui_init(&application->ui, &ui_info);

    prisma_window_show(&application->window);

    return PRISMA_ERROR_NONE;
}

enum prisma_error prisma_application_run(struct prisma_application *application)
{    
    while (!prisma_window_should_close(&application->window))
    {
        prisma_window_poll_events(&application->window);
        prisma_ui_draw(&application->ui);
        prisma_renderer_draw(&application->renderer, &application->window);
    }

    return PRISMA_ERROR_NONE;
}

void prisma_application_destroy(struct prisma_application *application)
{
    prisma_ui_destroy(&application->ui);
    prisma_renderer_destroy(&application->renderer);
    prisma_window_destroy(&application->window);
}
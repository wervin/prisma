#include <stdalign.h>
#include <string.h>
#include <stdalign.h>
#include <stdlib.h>
#include <sys/time.h>

#include "prisma/application.h"
#include "prisma/renderer.h"
#include "prisma/log.h"

#include "prisma/backend/models/vertex2d.h"

struct uniformbuffer_object {
    alignas(16) vec3 resolution;
    alignas(4) float time;
};

static void _update_uniformbuffer(struct prisma_renderer *renderer, void *mapped_buffer);

static enum prisma_error _recreate_swapchain(struct prisma_renderer *renderer, struct prisma_window *window);

enum prisma_error prisma_renderer_init(struct prisma_renderer *renderer,
                                       struct prisma_window *window,
                                       struct prisma_renderer_info *info)
{
    enum prisma_error status;

    struct prisma_backend_instance_info instance_info = {
        .application_name = info->application_name,
        .application_version_major = info->application_version_major,
        .application_version_minor = info->application_version_minor,
        .application_version_revision = info->application_version_revision,
#ifndef NDEBUG
        .validation_layers = info->validation_layers,
        .debug_extensions = info->debug_extensions
#endif
    };

    status = prisma_backend_instance_init(&renderer->instance, window, &instance_info);
    if (status != PRISMA_ERROR_NONE)
        return status;

    struct prisma_backend_device_info device_info = {
        .features_requested = info->features_requested,
        .vk_surface_format = info->vk_surface_format,
        .device_extensions = info->device_extensions,
#ifndef NDEBUG
        .validation_layers = info->validation_layers,
#endif
    };

    status = prisma_backend_device_init(&renderer->device, &renderer->instance, &device_info);
    if (status != PRISMA_ERROR_NONE)
        return status;

    struct prisma_backend_swapchain_info swapchain_info = {
        .vk_surface_format = info->vk_surface_format,
        .vk_present_mode = info->vk_present_mode,
        .image_count = info->image_count,
        .array_layer_count = info->array_layer_count,
        .transform = info->transform,
        .create_flags = info->create_flags,
        .image_usage_flags = info->image_usage_flags,
        .composite_alpha = info->composite_alpha,
        .clipped = info->clipped
    };

    status = prisma_backend_swapchain_init(&renderer->swapchain, &renderer->instance, &renderer->device, window, &swapchain_info);
    if (status != PRISMA_ERROR_NONE)
        return status;
    
    struct prisma_backend_renderpass_info renderpass_info = {
        .format = info->vk_surface_format.format,
        .sample_count = info->sample_count
    };

    status = prisma_backend_renderpass_init(&renderer->renderpass, &renderer->device, &renderpass_info);
    if (status != PRISMA_ERROR_NONE)
        return status;

    status = prisma_backend_framebuffers_init(&renderer->framebuffers, &renderer->device, &renderer->swapchain, &renderer->renderpass);
    if (status != PRISMA_ERROR_NONE)
        return status;

    status = prisma_backend_commandpool_init(&renderer->commandpool, &renderer->device);
    if (status != PRISMA_ERROR_NONE)
        return status;

    struct prisma_backend_commandbuffers_info commandbuffers_info = {
        .max_frames_in_flight = info->max_frames_in_flight
    };

    status = prisma_backend_commandbuffers_init(&renderer->commandbuffers, &renderer->device, &renderer->commandpool, &commandbuffers_info);
    if (status != PRISMA_ERROR_NONE)
        return status;

    struct prisma_backend_sync_info sync_info = {
        .max_frames_in_flight = info->max_frames_in_flight
    };

    status = prisma_backend_sync_init(&renderer->sync, &renderer->device, &sync_info);
    if (status != PRISMA_ERROR_NONE)
        return status;

    struct prisma_backend_shader_info vertex_shader_info = {
        .path = info->vertex_shader_path
    };

    status = prisma_backend_shader_init(&renderer->vertex_shader, &renderer->device, &vertex_shader_info);
    if (status != PRISMA_ERROR_NONE)
        return status;

    struct prisma_backend_shader_info frag_shader_info = {
        .path = info->frag_shader_path
    };

    status = prisma_backend_shader_init(&renderer->frag_shader, &renderer->device, &frag_shader_info);
    if (status != PRISMA_ERROR_NONE)
        return status;

    status = prisma_backend_descriptorsetlayout_init(&renderer->descriptorsetlayout, &renderer->device);
    if (status != PRISMA_ERROR_NONE)
        return status;

    status = prisma_backend_pipelinelayout_init(&renderer->pipelinelayout, &renderer->device, &renderer->descriptorsetlayout);
    if (status != PRISMA_ERROR_NONE)
        return status;

    status = prisma_backend_pipeline_init(
        &renderer->pipeline, 
        &renderer->device,
        &renderer->renderpass,
        &renderer->pipelinelayout, 
        &renderer->vertex_shader, 
        &renderer->frag_shader);
    if (status != PRISMA_ERROR_NONE)
        return status;

    struct prisma_backend_models_vertex2d vertices[] = {
        {{1.f, 1.f}},   // 0
        {{-1.f, -1.f}}, // 1
        {{-1.f, 1.f}},  // 2
        {{1.f, -1.f}}   // 3
    };

    struct prisma_backend_buffers_vertexbuffer_info vertexbuffer_info = {
        .data = vertices,
        .vertex_count = sizeof(vertices) / sizeof(struct prisma_backend_models_vertex2d),
        .vertex_size = sizeof(struct prisma_backend_models_vertex2d),
    };

    status = prisma_backend_buffers_vertexbuffer_init(&renderer->vertexbuffer, &renderer->device, &renderer->commandpool, &vertexbuffer_info);
    if (status != PRISMA_ERROR_NONE)
        return status;

    int16_t indices[] = {0, 1, 2, 0, 1, 3};

    struct prisma_backend_buffers_indexbuffer_info indexbuffer_info = {
        .data = indices,
        .index_count = sizeof(indices) / sizeof(uint16_t),
        .index_size = sizeof(uint16_t)
    };

    status = prisma_backend_buffers_indexbuffer_init(&renderer->indexbuffer, &renderer->device, &renderer->commandpool, &indexbuffer_info);
    if (status != PRISMA_ERROR_NONE)
        return status;

    struct prisma_backend_buffers_uniformbuffer_info uniformbuffer_info = {
        .max_frames_in_flight = 2,
        .size = sizeof(struct uniformbuffer_object)
    };

    status = prisma_backend_buffers_uniformbuffer_init(&renderer->uniformbuffer, &renderer->device, &uniformbuffer_info);
    if (status != PRISMA_ERROR_NONE)
        return status;

    struct prisma_backend_descriptorpool_info descriptorpool_info = {
        .max_frames_in_flight = 2
    };

    status = prisma_backend_descriptorpool_init(&renderer->descriptorpool, &renderer->device, &descriptorpool_info);
    if (status != PRISMA_ERROR_NONE)
        return status;

    struct prisma_backend_descriptorsets_info descriptorsets_info = {
        .max_frames_in_flight = 2,
    };

    status = prisma_backend_descriptorsets_init(&renderer->descriptorsets, &renderer->device, &renderer->descriptorpool,
                                               &renderer->descriptorsetlayout, &renderer->uniformbuffer, &descriptorsets_info);
    if (status != PRISMA_ERROR_NONE)
        return status;


    renderer->info = *info;
    
    return PRISMA_ERROR_NONE;
}

enum prisma_error prisma_renderer_draw(struct prisma_renderer *renderer, struct prisma_window *window)
{
    VkResult result = VK_SUCCESS;

    if (vkWaitForFences(renderer->device.vk_device, 1, &renderer->sync.vk_render_fence[renderer->current_frame_in_flight], VK_TRUE, 1000000000) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to wait render fence");
        return PRISMA_ERROR_VK;
    }

    uint32_t swapchainImageIndex = 0;
    result = vkAcquireNextImageKHR(
        renderer->device.vk_device,
        renderer->swapchain.vk_swapchain,
        1000000000,
        renderer->sync.vk_present_semaphore[renderer->current_frame_in_flight],
        VK_NULL_HANDLE,
        &swapchainImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_TIMEOUT)
        return _recreate_swapchain(renderer, window);

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to acquire next image");
        return PRISMA_ERROR_VK;
    }

    _update_uniformbuffer(renderer, renderer->uniformbuffer.mapped_buffers[renderer->current_frame_in_flight]);

    if (vkResetFences(renderer->device.vk_device, 1, &renderer->sync.vk_render_fence[renderer->current_frame_in_flight]) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to reset render fence");
        return PRISMA_ERROR_VK;
    }

    if (vkResetCommandBuffer(renderer->commandbuffers.vk_commandbuffers[renderer->current_frame_in_flight], 0) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to reset command buffer");
        return PRISMA_ERROR_VK;
    }

    VkCommandBufferBeginInfo commandbufferbegin_info = {0};
    commandbufferbegin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandbufferbegin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    if (vkBeginCommandBuffer(renderer->commandbuffers.vk_commandbuffers[renderer->current_frame_in_flight], &commandbufferbegin_info) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to reset begin command buffer");
        return PRISMA_ERROR_VK;
    }

    VkClearValue clearValue = {0};
    clearValue.color = (VkClearColorValue) {{0.0f, 0.0f, 0.0f, 1.0f}};

    VkRenderPassBeginInfo renderpassbegin_info = {0};
    renderpassbegin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderpassbegin_info.renderPass = renderer->renderpass.vk_renderpass;
    renderpassbegin_info.renderArea.offset = (VkOffset2D) {.x = 0, .y = 0};
    renderpassbegin_info.renderArea.extent = renderer->swapchain.vk_extent;
    renderpassbegin_info.clearValueCount = 1;
    renderpassbegin_info.pClearValues = &clearValue;
    renderpassbegin_info.framebuffer = renderer->framebuffers.vk_framebuffers[swapchainImageIndex];
    vkCmdBeginRenderPass(renderer->commandbuffers.vk_commandbuffers[renderer->current_frame_in_flight], &renderpassbegin_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(renderer->commandbuffers.vk_commandbuffers[renderer->current_frame_in_flight], VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->pipeline.vk_pipeline);

    VkViewport viewport = {0};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) renderer->swapchain.vk_extent.width;
    viewport.height = (float) renderer->swapchain.vk_extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(renderer->commandbuffers.vk_commandbuffers[renderer->current_frame_in_flight], 0, 1, &viewport);

    VkRect2D scissor = {0};
    scissor.offset = (VkOffset2D) {.x = 0, .y = 0};
    scissor.extent = renderer->swapchain.vk_extent;
    vkCmdSetScissor(renderer->commandbuffers.vk_commandbuffers[renderer->current_frame_in_flight], 0, 1, &scissor);

    VkBuffer vertex_buffers[] = {renderer->vertexbuffer.buffer.vk_buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(renderer->commandbuffers.vk_commandbuffers[renderer->current_frame_in_flight], 0, 1, vertex_buffers, offsets);

    vkCmdBindIndexBuffer(renderer->commandbuffers.vk_commandbuffers[renderer->current_frame_in_flight], renderer->indexbuffer.buffer.vk_buffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(renderer->commandbuffers.vk_commandbuffers[renderer->current_frame_in_flight], 
                            VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->pipelinelayout.vk_pipelinelayout,
                            0, 1, &renderer->descriptorsets.vk_descriptorsets[renderer->current_frame_in_flight], 0, NULL);

    vkCmdDrawIndexed(renderer->commandbuffers.vk_commandbuffers[renderer->current_frame_in_flight], renderer->indexbuffer.index_count, 1, 0, 0, 0);

    vkCmdEndRenderPass(renderer->commandbuffers.vk_commandbuffers[renderer->current_frame_in_flight]);

    if (vkEndCommandBuffer(renderer->commandbuffers.vk_commandbuffers[renderer->current_frame_in_flight]) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to reset end command buffer");
        return PRISMA_ERROR_VK;
    }

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit_info = {0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &renderer->commandbuffers.vk_commandbuffers[renderer->current_frame_in_flight];
    submit_info.pWaitDstStageMask = &waitStage;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &renderer->sync.vk_present_semaphore[renderer->current_frame_in_flight];
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &renderer->sync.vk_render_semaphore[renderer->current_frame_in_flight];
    if (vkQueueSubmit(renderer->device.vk_graphic_queue, 1, &submit_info, renderer->sync.vk_render_fence[renderer->current_frame_in_flight]) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to submit command buffer to the queue");
        return PRISMA_ERROR_VK;
    }

    VkPresentInfoKHR present_info = {0};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pSwapchains = &renderer->swapchain.vk_swapchain;
    present_info.swapchainCount = 1;
    present_info.pWaitSemaphores = &renderer->sync.vk_render_semaphore[renderer->current_frame_in_flight];
    present_info.waitSemaphoreCount = 1;
    present_info.pImageIndices = &swapchainImageIndex;
    result = vkQueuePresentKHR(renderer->device.vk_graphic_queue, &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {   
        return _recreate_swapchain(renderer, window);
    }

    if (result != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to present rendered images");
        return PRISMA_ERROR_VK;
    }
    
    return PRISMA_ERROR_NONE;
}

void prisma_renderer_destroy(struct prisma_renderer *renderer)
{
    prisma_backend_device_wait_idle(&renderer->device);
    prisma_backend_descriptorsets_destroy(&renderer->descriptorsets);
    prisma_backend_descriptorpool_destroy(&renderer->descriptorpool, &renderer->device);
    prisma_backend_buffers_uniformbuffer_destroy(&renderer->uniformbuffer, &renderer->device);
    prisma_backend_buffers_indexbuffer_destroy(&renderer->indexbuffer, &renderer->device);
    prisma_backend_buffers_vertexbuffer_destroy(&renderer->vertexbuffer, &renderer->device);
    prisma_backend_shader_destroy(&renderer->vertex_shader, &renderer->device);
    prisma_backend_shader_destroy(&renderer->frag_shader, &renderer->device);
    prisma_backend_sync_destroy(&renderer->sync, &renderer->device);
    prisma_backend_commandbuffers_destroy(&renderer->commandbuffers, &renderer->device, &renderer->commandpool);
    prisma_backend_commandpool_destroy(&renderer->commandpool, &renderer->device);
    prisma_backend_pipeline_destroy(&renderer->pipeline, &renderer->device);
    prisma_backend_pipelinelayout_destroy(&renderer->pipelinelayout, &renderer->device);
    prisma_backend_descriptorsetlayout_destroy(&renderer->descriptorsetlayout, &renderer->device);
    prisma_backend_framebuffers_destroy(&renderer->framebuffers, &renderer->device);
    prisma_backend_renderpass_destroy(&renderer->renderpass, &renderer->device);
    prisma_backend_swapchain_destroy(&renderer->swapchain, &renderer->device);
    prisma_backend_device_destroy(&renderer->device);
    prisma_backend_instance_destroy(&renderer->instance);
}

static enum prisma_error _recreate_swapchain(struct prisma_renderer *renderer, struct prisma_window *window)
{
    enum prisma_error status;

    VkExtent2D extent = prisma_window_get_extent(window);
    while(extent.height == 0 || extent.width == 0)
    {
        extent = prisma_window_get_extent(window);
        prisma_window_wait_events(window);
    }

    prisma_backend_device_wait_idle(&renderer->device);

    prisma_backend_framebuffers_destroy(&renderer->framebuffers, &renderer->device);
    prisma_backend_swapchain_destroy(&renderer->swapchain, &renderer->device);

    struct prisma_backend_swapchain_info swapchain_info = {
        .vk_surface_format = renderer->info.vk_surface_format,
        .vk_present_mode = renderer->info.vk_present_mode,
        .image_count = renderer->info.image_count,
        .array_layer_count = renderer->info.array_layer_count,
        .transform = renderer->info.transform,
        .create_flags = renderer->info.create_flags,
        .image_usage_flags = renderer->info.image_usage_flags,
        .composite_alpha = renderer->info.composite_alpha,
        .clipped = renderer->info.clipped
    };

    status = prisma_backend_swapchain_init(&renderer->swapchain, &renderer->instance, &renderer->device, window, &swapchain_info);
    if (status != PRISMA_ERROR_NONE)
        return status;

    status = prisma_backend_framebuffers_init(&renderer->framebuffers, &renderer->device, &renderer->swapchain, &renderer->renderpass);
    if (status != PRISMA_ERROR_NONE)
        return status;

    return PRISMA_ERROR_NONE;
}

static void _update_uniformbuffer(struct prisma_renderer *renderer, void *mapped_buffer)
{
    struct timeval timestamp;
    gettimeofday(&timestamp, 0);
    float t = (timestamp.tv_sec & 0xFF) + timestamp.tv_usec*1e-6;

    struct uniformbuffer_object ubo = {
        .resolution = {(float)renderer->swapchain.vk_extent.width, (float)renderer->swapchain.vk_extent.height, 0.0f},
        .time = t};

    memcpy(mapped_buffer, &ubo, sizeof(ubo));
}
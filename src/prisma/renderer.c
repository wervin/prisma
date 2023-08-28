#include <stdalign.h>
#include <string.h>
#include <stdalign.h>
#include <stdlib.h>
#include <sys/time.h>

#include <vulkan/vulkan.h>

#include "prisma/window.h"
#include "prisma/log.h"

#include "prisma/backend/device.h"
#include "prisma/backend/instance.h"
#include "prisma/backend/swapchain.h"
#include "prisma/backend/renderpass.h"
#include "prisma/backend/framebuffers.h"
#include "prisma/backend/commandpool.h"
#include "prisma/backend/commandbuffers.h"
#include "prisma/backend/sync.h"
#include "prisma/backend/shader.h"
#include "prisma/backend/descriptorsetlayout.h"
#include "prisma/backend/pipelinelayout.h"
#include "prisma/backend/pipeline.h"
#include "prisma/backend/descriptorpool.h"
#include "prisma/backend/descriptorsets.h"

#include "prisma/backend/buffers/vertexbuffer.h"
#include "prisma/backend/buffers/indexbuffer.h"
#include "prisma/backend/buffers/uniformbuffer.h"

#include "prisma/backend/models/vertex2d.h"

struct _backend_info
{
  const char *vertex_shader_path;
  const char *frag_shader_path;
  
  uint32_t max_frames_in_flight;
  VkPhysicalDeviceFeatures features_requested;
  const char** device_extensions;
  VkSurfaceFormatKHR vk_surface_format;
  VkPresentModeKHR  vk_present_mode;
  uint32_t image_count;
  uint32_t array_layer_count;
  VkSurfaceTransformFlagBitsKHR transform;
  VkSwapchainCreateFlagBitsKHR create_flags;
  VkImageUsageFlags image_usage_flags;
  VkCompositeAlphaFlagBitsKHR composite_alpha;
  VkBool32 clipped;
  VkSampleCountFlagBits sample_count;

#ifndef NDEBUG
  const char** validation_layers;
  const char** debug_extensions;
#endif
};

struct _backend
{
  uint64_t frame_count;
  uint32_t current_frame_in_flight;

  struct prisma_backend_device device;
  struct prisma_backend_instance instance;
  struct prisma_backend_swapchain swapchain;
  struct prisma_backend_renderpass renderpass;
  struct prisma_backend_framebuffers framebuffers;
  struct prisma_backend_commandpool commandpool;
  struct prisma_backend_commandbuffers commandbuffers;
  struct prisma_backend_sync sync;
  struct prisma_backend_shader vertex_shader;
  struct prisma_backend_shader frag_shader;
  struct prisma_backend_descriptorsetlayout descriptorsetlayout;
  struct prisma_backend_pipelinelayout pipelinelayout;
  struct prisma_backend_pipeline pipeline;
  struct prisma_backend_buffers_vertexbuffer vertexbuffer;
  struct prisma_backend_buffers_indexbuffer indexbuffer;
  struct prisma_backend_buffers_uniformbuffer uniformbuffer;
  struct prisma_backend_descriptorpool descriptorpool;
  struct prisma_backend_descriptorsets descriptorsets;
};

struct _ubo
{
  alignas(16) vec3 resolution;
  alignas(4) float time;
};

static enum prisma_error _recreate_swapchain(void);

static void _update_uniformbuffer(void *mapped_buffer);

static struct _backend _backend = {0};

static struct _backend_info _backend_info = {
    .vertex_shader_path = "default.vert.spv",
    .frag_shader_path = "default.frag.spv",

    .max_frames_in_flight = 2,
    .features_requested = {
        .samplerAnisotropy = VK_TRUE
    },
    .vk_surface_format = {
        .format = VK_FORMAT_B8G8R8A8_SRGB, 
        .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
    },
    .array_layer_count = 1,
    .clipped = VK_TRUE,
    .composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    .create_flags = 0,
    .image_count = 3,
    .image_usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
    .transform = 0,
    .vk_present_mode = VK_PRESENT_MODE_FIFO_KHR,
    .sample_count = VK_SAMPLE_COUNT_1_BIT,
    .device_extensions = (const char *[]){"VK_KHR_swapchain"},
#ifndef NDEBUG
    .validation_layers = (const char *[]){"VK_LAYER_KHRONOS_validation"},
    .debug_extensions = (const char *[]){"VK_EXT_debug_utils"}
#endif
};

enum prisma_error prisma_renderer_init(void)
{
  enum prisma_error status;

  struct prisma_backend_instance_info instance_info = {
#ifndef NDEBUG
      .validation_layers = _backend_info.validation_layers,
      .debug_extensions = _backend_info.debug_extensions
#endif
  };

  status = prisma_backend_instance_init(&_backend.instance, &instance_info);
  if (status != PRISMA_ERROR_NONE)
      return status;

  struct prisma_backend_device_info device_info = {
      .features_requested = _backend_info.features_requested,
      .vk_surface_format = _backend_info.vk_surface_format,
      .device_extensions = _backend_info.device_extensions,
#ifndef NDEBUG
      .validation_layers = _backend_info.validation_layers,
#endif
  };

  status = prisma_backend_device_init(&_backend.device, &_backend.instance, &device_info);
  if (status != PRISMA_ERROR_NONE)
      return status;

  struct prisma_backend_swapchain_info swapchain_info = {
      .vk_surface_format = _backend_info.vk_surface_format,
      .vk_present_mode = _backend_info.vk_present_mode,
      .image_count = _backend_info.image_count,
      .array_layer_count = _backend_info.array_layer_count,
      .transform = _backend_info.transform,
      .create_flags = _backend_info.create_flags,
      .image_usage_flags = _backend_info.image_usage_flags,
      .composite_alpha = _backend_info.composite_alpha,
      .clipped = _backend_info.clipped};

  status = prisma_backend_swapchain_init(&_backend.swapchain, &_backend.instance, &_backend.device, &swapchain_info);
  if (status != PRISMA_ERROR_NONE)
      return status;

  struct prisma_backend_renderpass_info renderpass_info = {
      .format = _backend_info.vk_surface_format.format,
      .sample_count = _backend_info.sample_count};

  status = prisma_backend_renderpass_init(&_backend.renderpass, &_backend.device, &renderpass_info);
  if (status != PRISMA_ERROR_NONE)
      return status;

  status = prisma_backend_framebuffers_init(&_backend.framebuffers, &_backend.device, &_backend.swapchain, &_backend.renderpass);
  if (status != PRISMA_ERROR_NONE)
      return status;

  status = prisma_backend_commandpool_init(&_backend.commandpool, &_backend.device);
  if (status != PRISMA_ERROR_NONE)
      return status;

  struct prisma_backend_commandbuffers_info commandbuffers_info = {
      .max_frames_in_flight = _backend_info.max_frames_in_flight};

  status = prisma_backend_commandbuffers_init(&_backend.commandbuffers, &_backend.device, &_backend.commandpool, &commandbuffers_info);
  if (status != PRISMA_ERROR_NONE)
      return status;

  struct prisma_backend_sync_info sync_info = {
      .max_frames_in_flight = _backend_info.max_frames_in_flight};

  status = prisma_backend_sync_init(&_backend.sync, &_backend.device, &sync_info);
  if (status != PRISMA_ERROR_NONE)
      return status;

  struct prisma_backend_shader_info vertex_shader_info = {
      .path = _backend_info.vertex_shader_path};

  status = prisma_backend_shader_init(&_backend.vertex_shader, &_backend.device, &vertex_shader_info);
  if (status != PRISMA_ERROR_NONE)
      return status;

  struct prisma_backend_shader_info frag_shader_info = {
      .path = _backend_info.frag_shader_path};

  status = prisma_backend_shader_init(&_backend.frag_shader, &_backend.device, &frag_shader_info);
  if (status != PRISMA_ERROR_NONE)
      return status;

  status = prisma_backend_descriptorsetlayout_init(&_backend.descriptorsetlayout, &_backend.device);
  if (status != PRISMA_ERROR_NONE)
      return status;

  status = prisma_backend_pipelinelayout_init(&_backend.pipelinelayout, &_backend.device, &_backend.descriptorsetlayout);
  if (status != PRISMA_ERROR_NONE)
      return status;

  status = prisma_backend_pipeline_init(
      &_backend.pipeline,
      &_backend.device,
      &_backend.renderpass,
      &_backend.pipelinelayout,
      &_backend.vertex_shader,
      &_backend.frag_shader);
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

  status = prisma_backend_buffers_vertexbuffer_init(&_backend.vertexbuffer, &_backend.device, &_backend.commandpool, &vertexbuffer_info);
  if (status != PRISMA_ERROR_NONE)
      return status;

  int16_t indices[] = {0, 1, 2, 0, 1, 3};

  struct prisma_backend_buffers_indexbuffer_info indexbuffer_info = {
      .data = indices,
      .index_count = sizeof(indices) / sizeof(uint16_t),
      .index_size = sizeof(uint16_t)};

  status = prisma_backend_buffers_indexbuffer_init(&_backend.indexbuffer, &_backend.device, &_backend.commandpool, &indexbuffer_info);
  if (status != PRISMA_ERROR_NONE)
      return status;

  struct prisma_backend_buffers_uniformbuffer_info uniformbuffer_info = {
      .max_frames_in_flight = 2,
      .size = sizeof(struct _ubo)};

  status = prisma_backend_buffers_uniformbuffer_init(&_backend.uniformbuffer, &_backend.device, &uniformbuffer_info);
  if (status != PRISMA_ERROR_NONE)
      return status;

  struct prisma_backend_descriptorpool_info descriptorpool_info = {
      .max_frames_in_flight = 2};

  status = prisma_backend_descriptorpool_init(&_backend.descriptorpool, &_backend.device, &descriptorpool_info);
  if (status != PRISMA_ERROR_NONE)
      return status;

  struct prisma_backend_descriptorsets_info descriptorsets_info = {
      .max_frames_in_flight = 2,
  };

  status = prisma_backend_descriptorsets_init(&_backend.descriptorsets, &_backend.device, &_backend.descriptorpool,
                                              &_backend.descriptorsetlayout, &_backend.uniformbuffer, &descriptorsets_info);
  if (status != PRISMA_ERROR_NONE)
      return status;

  return PRISMA_ERROR_NONE;
}

enum prisma_error prisma_renderer_draw(void)
{
    VkResult result = VK_SUCCESS;

    _backend.current_frame_in_flight = _backend.frame_count % _backend_info.max_frames_in_flight;

    if (vkWaitForFences(_backend.device.vk_device, 1, &_backend.sync.vk_render_fence[_backend.current_frame_in_flight], VK_TRUE, 1000000000) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to wait render fence");
        return PRISMA_ERROR_VK;
    }

    uint32_t swapchainImageIndex = 0;
    result = vkAcquireNextImageKHR(
        _backend.device.vk_device,
        _backend.swapchain.vk_swapchain,
        1000000000,
        _backend.sync.vk_present_semaphore[_backend.current_frame_in_flight],
        VK_NULL_HANDLE,
        &swapchainImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_TIMEOUT)
        return _recreate_swapchain();

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to acquire next image");
        return PRISMA_ERROR_VK;
    }

    _update_uniformbuffer(_backend.uniformbuffer.mapped_buffers[_backend.current_frame_in_flight]);

    if (vkResetFences(_backend.device.vk_device, 1, &_backend.sync.vk_render_fence[_backend.current_frame_in_flight]) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to reset render fence");
        return PRISMA_ERROR_VK;
    }

    if (vkResetCommandBuffer(_backend.commandbuffers.vk_commandbuffers[_backend.current_frame_in_flight], 0) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to reset command buffer");
        return PRISMA_ERROR_VK;
    }

    VkCommandBufferBeginInfo commandbufferbegin_info = {0};
    commandbufferbegin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandbufferbegin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    if (vkBeginCommandBuffer(_backend.commandbuffers.vk_commandbuffers[_backend.current_frame_in_flight], &commandbufferbegin_info) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to reset begin command buffer");
        return PRISMA_ERROR_VK;
    }

    VkClearValue clearValue = {0};
    clearValue.color = (VkClearColorValue) {{0.0f, 0.0f, 0.0f, 1.0f}};

    VkRenderPassBeginInfo renderpassbegin_info = {0};
    renderpassbegin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderpassbegin_info.renderPass = _backend.renderpass.vk_renderpass;
    renderpassbegin_info.renderArea.offset = (VkOffset2D) {.x = 0, .y = 0};
    renderpassbegin_info.renderArea.extent = _backend.swapchain.vk_extent;
    renderpassbegin_info.clearValueCount = 1;
    renderpassbegin_info.pClearValues = &clearValue;
    renderpassbegin_info.framebuffer = _backend.framebuffers.vk_framebuffers[swapchainImageIndex];
    vkCmdBeginRenderPass(_backend.commandbuffers.vk_commandbuffers[_backend.current_frame_in_flight], &renderpassbegin_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(_backend.commandbuffers.vk_commandbuffers[_backend.current_frame_in_flight], VK_PIPELINE_BIND_POINT_GRAPHICS, _backend.pipeline.vk_pipeline);

    VkViewport viewport = {0};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) _backend.swapchain.vk_extent.width;
    viewport.height = (float) _backend.swapchain.vk_extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(_backend.commandbuffers.vk_commandbuffers[_backend.current_frame_in_flight], 0, 1, &viewport);

    VkRect2D scissor = {0};
    scissor.offset = (VkOffset2D) {.x = 0, .y = 0};
    scissor.extent = _backend.swapchain.vk_extent;
    vkCmdSetScissor(_backend.commandbuffers.vk_commandbuffers[_backend.current_frame_in_flight], 0, 1, &scissor);

    VkBuffer vertex_buffers[] = {_backend.vertexbuffer.buffer.vk_buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(_backend.commandbuffers.vk_commandbuffers[_backend.current_frame_in_flight], 0, 1, vertex_buffers, offsets);

    vkCmdBindIndexBuffer(_backend.commandbuffers.vk_commandbuffers[_backend.current_frame_in_flight], _backend.indexbuffer.buffer.vk_buffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(_backend.commandbuffers.vk_commandbuffers[_backend.current_frame_in_flight], 
                            VK_PIPELINE_BIND_POINT_GRAPHICS, _backend.pipelinelayout.vk_pipelinelayout,
                            0, 1, &_backend.descriptorsets.vk_descriptorsets[_backend.current_frame_in_flight], 0, NULL);

    vkCmdDrawIndexed(_backend.commandbuffers.vk_commandbuffers[_backend.current_frame_in_flight], _backend.indexbuffer.index_count, 1, 0, 0, 0);

    vkCmdEndRenderPass(_backend.commandbuffers.vk_commandbuffers[_backend.current_frame_in_flight]);

    if (vkEndCommandBuffer(_backend.commandbuffers.vk_commandbuffers[_backend.current_frame_in_flight]) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to reset end command buffer");
        return PRISMA_ERROR_VK;
    }

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit_info = {0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &_backend.commandbuffers.vk_commandbuffers[_backend.current_frame_in_flight];
    submit_info.pWaitDstStageMask = &waitStage;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &_backend.sync.vk_present_semaphore[_backend.current_frame_in_flight];
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &_backend.sync.vk_render_semaphore[_backend.current_frame_in_flight];
    if (vkQueueSubmit(_backend.device.vk_graphic_queue, 1, &submit_info, _backend.sync.vk_render_fence[_backend.current_frame_in_flight]) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to submit command buffer to the queue");
        return PRISMA_ERROR_VK;
    }

    VkPresentInfoKHR present_info = {0};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pSwapchains = &_backend.swapchain.vk_swapchain;
    present_info.swapchainCount = 1;
    present_info.pWaitSemaphores = &_backend.sync.vk_render_semaphore[_backend.current_frame_in_flight];
    present_info.waitSemaphoreCount = 1;
    present_info.pImageIndices = &swapchainImageIndex;
    result = vkQueuePresentKHR(_backend.device.vk_graphic_queue, &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {   
        return _recreate_swapchain();
    }

    if (result != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to present rendered images");
        return PRISMA_ERROR_VK;
    }

    _backend.frame_count++;
    
    return PRISMA_ERROR_NONE;
}

void prisma_renderer_destroy(void)
{
    prisma_backend_device_wait_idle(&_backend.device);
    prisma_backend_descriptorsets_destroy(&_backend.descriptorsets);
    prisma_backend_descriptorpool_destroy(&_backend.descriptorpool, &_backend.device);
    prisma_backend_buffers_uniformbuffer_destroy(&_backend.uniformbuffer, &_backend.device);
    prisma_backend_buffers_indexbuffer_destroy(&_backend.indexbuffer, &_backend.device);
    prisma_backend_buffers_vertexbuffer_destroy(&_backend.vertexbuffer, &_backend.device);
    prisma_backend_shader_destroy(&_backend.vertex_shader, &_backend.device);
    prisma_backend_shader_destroy(&_backend.frag_shader, &_backend.device);
    prisma_backend_sync_destroy(&_backend.sync, &_backend.device);
    prisma_backend_commandbuffers_destroy(&_backend.commandbuffers, &_backend.device, &_backend.commandpool);
    prisma_backend_commandpool_destroy(&_backend.commandpool, &_backend.device);
    prisma_backend_pipeline_destroy(&_backend.pipeline, &_backend.device);
    prisma_backend_pipelinelayout_destroy(&_backend.pipelinelayout, &_backend.device);
    prisma_backend_descriptorsetlayout_destroy(&_backend.descriptorsetlayout, &_backend.device);
    prisma_backend_framebuffers_destroy(&_backend.framebuffers, &_backend.device);
    prisma_backend_renderpass_destroy(&_backend.renderpass, &_backend.device);
    prisma_backend_swapchain_destroy(&_backend.swapchain, &_backend.device);
    prisma_backend_device_destroy(&_backend.device);
    prisma_backend_instance_destroy(&_backend.instance);
}

static enum prisma_error _recreate_swapchain(void)
{
    enum prisma_error status;
    uint32_t width, height;
    prisma_window_get_extent(&width, &height);
    while (height == 0 || width == 0)
    {
        prisma_window_get_extent(&width, &height);
        prisma_window_wait_events();
    }

    prisma_backend_device_wait_idle(&_backend.device);

    prisma_backend_framebuffers_destroy(&_backend.framebuffers, &_backend.device);
    prisma_backend_swapchain_destroy(&_backend.swapchain, &_backend.device);

    struct prisma_backend_swapchain_info swapchain_info = {
        .vk_surface_format = _backend_info.vk_surface_format,
        .vk_present_mode = _backend_info.vk_present_mode,
        .image_count = _backend_info.image_count,
        .array_layer_count = _backend_info.array_layer_count,
        .transform = _backend_info.transform,
        .create_flags = _backend_info.create_flags,
        .image_usage_flags = _backend_info.image_usage_flags,
        .composite_alpha = _backend_info.composite_alpha,
        .clipped = _backend_info.clipped
    };

    status = prisma_backend_swapchain_init(&_backend.swapchain, &_backend.instance, &_backend.device, &swapchain_info);
    if (status != PRISMA_ERROR_NONE)
        return status;

    status = prisma_backend_framebuffers_init(&_backend.framebuffers, &_backend.device, &_backend.swapchain, &_backend.renderpass);
    if (status != PRISMA_ERROR_NONE)
        return status;

    return PRISMA_ERROR_NONE;
}

static void _update_uniformbuffer(void *mapped_buffer)
{
    struct timeval timestamp;
    gettimeofday(&timestamp, 0);
    float t = (timestamp.tv_sec & 0xFF) + timestamp.tv_usec*1e-6;

    struct _ubo ubo = {
        .resolution = {(float)_backend.swapchain.vk_extent.width, (float)_backend.swapchain.vk_extent.height, 0.0f},
        .time = t};

    memcpy(mapped_buffer, &ubo, sizeof(ubo));
}
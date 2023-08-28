#ifndef PRISMA_RENDERER_H
#define PRISMA_RENDERER_H

#include <stdint.h>

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

#include "prisma/error.h"

struct prisma_window;

struct prisma_renderer_info
{
  const char *application_name;
  uint32_t application_version_major;
  uint32_t application_version_minor;
  uint32_t application_version_revision;

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

struct prisma_renderer
{
  uint64_t frame_count;
  uint32_t current_frame_in_flight;

  struct prisma_renderer_info info;

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

/*
 * @brief Init Prisma renderer
 * @param renderer Prisma renderer
 * @param window Prisma window
 * @param info intial information about Prisma renderer
 * @return Error if any
 */
enum prisma_error prisma_renderer_init(struct prisma_renderer *renderer,
                                       struct prisma_window *window,
                                       struct prisma_renderer_info *info);


/*
 * @brief Draw a frame from Prisma renderer
 * @param renderer Prisma renderer
 * @param window Prisma window
 * @return Error if any
 */
enum prisma_error prisma_renderer_draw(struct prisma_renderer *renderer, struct prisma_window *window);

/*
 * @brief destroy Prisma renderer
 * @param renderer Prisma renderer
 */
void prisma_renderer_destroy(struct prisma_renderer *renderer);

#endif /* PRISMA_RENDERER_H */
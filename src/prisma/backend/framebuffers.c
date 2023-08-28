#include <stdlib.h>

#include "prisma/backend/framebuffers.h"
#include "prisma/backend/renderpass.h"
#include "prisma/backend/swapchain.h"
#include "prisma/backend/device.h"

#include "prisma/log.h"

static enum prisma_error _create_framebuffers(struct prisma_backend_framebuffers *framebuffers, 
                                            struct prisma_backend_device *device,
                                            struct prisma_backend_swapchain *swapchain, 
                                            struct prisma_backend_renderpass *renderpass);

enum prisma_error prisma_backend_framebuffers_init(struct prisma_backend_framebuffers *framebuffers,
                                       struct prisma_backend_device *device,
                                       struct prisma_backend_swapchain *swapchain,
                                       struct prisma_backend_renderpass *renderpass)
{
    enum prisma_error status;

    status = _create_framebuffers(framebuffers, device, swapchain, renderpass);
    if (status != PRISMA_ERROR_NONE) {
        return status;
    }

    return PRISMA_ERROR_NONE; 
}

void prisma_backend_framebuffers_destroy(struct prisma_backend_framebuffers *framebuffers, struct prisma_backend_device *device)
{
    if (framebuffers->vk_framebuffers)
    {
        for (uint32_t i = 0; i < framebuffers->framebuffer_count; i++)
        {
            vkDestroyFramebuffer(device->vk_device, framebuffers->vk_framebuffers[i], NULL);
        }
        free(framebuffers->vk_framebuffers);
    }
}

static enum prisma_error _create_framebuffers(struct prisma_backend_framebuffers *framebuffers, 
                                            struct prisma_backend_device *device,
                                            struct prisma_backend_swapchain *swapchain, 
                                            struct prisma_backend_renderpass *renderpass)
{
    framebuffers->vk_framebuffers = malloc(swapchain->image_count * sizeof(VkFramebuffer));
    framebuffers->framebuffer_count = swapchain->image_count;
    if (framebuffers->vk_framebuffers == NULL) {
        PRISMA_LOG_ERROR(PRISMA_ERROR_MEMORY, "Failed to allocate memory");
        return PRISMA_ERROR_MEMORY;
    }

    for (uint32_t i = 0; i < swapchain->image_count; i++)
    {
        VkFramebufferCreateInfo framebuffer_info = {0};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = renderpass->vk_renderpass;
        framebuffer_info.attachmentCount = 1;
        framebuffer_info.width = swapchain->vk_extent.width;
        framebuffer_info.height = swapchain->vk_extent.height;
        framebuffer_info.layers = 1;
        framebuffer_info.pAttachments = &swapchain->vk_image_views[i];
        vkCreateFramebuffer(device->vk_device, &framebuffer_info, NULL, &framebuffers->vk_framebuffers[i]);
    }
    return PRISMA_ERROR_NONE;
}
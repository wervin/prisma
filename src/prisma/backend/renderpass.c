#include "prisma/backend/renderpass.h"
#include "prisma/backend/swapchain.h"
#include "prisma/backend/device.h"
#include "prisma/backend/instance.h"

#include "prisma/window.h"
#include "prisma/log.h"

static enum prisma_error _create_renderpass(struct prisma_backend_renderpass *renderpass,
                                            struct prisma_backend_device *device,
                                            struct prisma_backend_renderpass_info *info);

enum prisma_error prisma_backend_renderpass_init(struct prisma_backend_renderpass *renderpass,
                                                 struct prisma_backend_device *device,
                                                 struct prisma_backend_renderpass_info *info)
{
    enum prisma_error status;

    status = _create_renderpass(renderpass, device, info);
    if (status != PRISMA_ERROR_NONE) {
        return status;
    }

    return PRISMA_ERROR_NONE; 
}

void prisma_backend_renderpass_destroy(struct prisma_backend_renderpass *renderpass, struct prisma_backend_device *device)
{
    vkDestroyRenderPass(device->vk_device, renderpass->vk_renderpass, NULL);
}

static enum prisma_error _create_renderpass(struct prisma_backend_renderpass *renderpass,
                                            struct prisma_backend_device *device,
                                            struct prisma_backend_renderpass_info *info)
{
    VkAttachmentDescription color_attachment = {0};
    color_attachment.format = info->format;
    color_attachment.samples = info->sample_count;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = {0};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {0};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkSubpassDependency dependency = {0};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo render_pass_info = {0};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;
    if (vkCreateRenderPass(device->vk_device, &render_pass_info, NULL, &renderpass->vk_renderpass) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to create render pass");
        return PRISMA_ERROR_VK;
    }

    return PRISMA_ERROR_NONE;
}
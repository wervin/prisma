#include "prisma/backend/instance.h"

#include "prisma/log.h"
#include "prisma/window.h"

#include "sake/macro.h"

static enum prisma_error _create_instance(struct prisma_backend_instance *instance, struct prisma_backend_instance_info *info);
static enum prisma_error _create_surface(struct prisma_backend_instance *instance, struct prisma_window *window);

#ifndef NDEBUG
static enum prisma_error _create_debug_instance(struct prisma_backend_instance *instance);
static VKAPI_ATTR VkBool32 VKAPI_CALL _debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT * cb_data,
    void * user_data);
#endif

enum prisma_error prisma_backend_instance_init(
    struct prisma_backend_instance *instance,
    struct prisma_window *window,
    struct prisma_backend_instance_info *info)
{
    enum prisma_error status;

    status = _create_instance(instance, info);
    if (status != PRISMA_ERROR_NONE) {
        return status;
    }
#ifndef NDEBUG
    status = _create_debug_instance(instance);
    if (status != PRISMA_ERROR_NONE) {
        return status;
    }
#endif
    status = _create_surface(instance, window);
    if (status != PRISMA_ERROR_NONE) {
        return status;
    }
    return PRISMA_ERROR_NONE;
}

void prisma_backend_instance_destroy(struct prisma_backend_instance *instance)
{
#ifndef NDEBUG
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance->vk_instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL)
        func(instance->vk_instance, instance->vk_debug_messenger, NULL);
#endif
    vkDestroySurfaceKHR(instance->vk_instance, instance->vk_surface, NULL);
    vkDestroyInstance(instance->vk_instance, NULL);
}

static enum prisma_error _create_instance(struct prisma_backend_instance *instance, struct prisma_backend_instance_info *info)
{
    VkApplicationInfo vk_app_info = {0};
    vk_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    vk_app_info.pApplicationName = info->application_name;
    vk_app_info.applicationVersion = VK_MAKE_VERSION(info->application_version_major, info->application_version_minor, info->application_version_revision);
    vk_app_info.pEngineName = info->application_name;
    vk_app_info.engineVersion = VK_MAKE_VERSION(info->application_version_major, info->application_version_minor, info->application_version_revision);
    vk_app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &vk_app_info;

    uint32_t glfw_extension_count = 0;
    const char ** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

#ifdef NDEBUG
    create_info.enabledExtensionCount = glfw_extension_count;
    create_info.ppEnabledExtensionNames = glfw_extensions;
    create_info.enabledLayerCount = 0;
#else 
    uint32_t all_extension_count = glfw_extension_count + sizeof(info->debug_extensions) / sizeof(char*);
    const char* all_extensions[all_extension_count];
    for (uint32_t i = 0; i < glfw_extension_count; i++)
        all_extensions[i] = glfw_extensions[i];
    for (uint32_t i = 0; i < sizeof(info->debug_extensions) / sizeof(char*); i++)
        all_extensions[glfw_extension_count + i] = info->debug_extensions[i];

    create_info.enabledExtensionCount = all_extension_count;
    create_info.ppEnabledExtensionNames = (const char **) all_extensions;
    create_info.enabledLayerCount = sizeof(info->validation_layers) / sizeof(char*);
    create_info.ppEnabledLayerNames = info->validation_layers;

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {0};
    debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_create_info.pfnUserCallback = _debug_callback;
    debug_create_info.pUserData = NULL;

    create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debug_create_info;
#endif

    if (vkCreateInstance(&create_info, NULL, &instance->vk_instance) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to create instance");
        return PRISMA_ERROR_VK;
    }

    return PRISMA_ERROR_NONE;
}

static enum prisma_error _create_surface(struct prisma_backend_instance *instance, struct prisma_window *window)
{
    if (glfwCreateWindowSurface(instance->vk_instance, window->glfw_window, NULL, &instance->vk_surface) != VK_SUCCESS) 
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_GLFW, "Failed to create window surface");
        return PRISMA_ERROR_GLFW;
    }

    return PRISMA_ERROR_NONE;
}

#ifndef NDEBUG
static enum prisma_error _create_debug_instance(struct prisma_backend_instance *instance)
{
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {0};
    debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_create_info.pfnUserCallback = _debug_callback;
    debug_create_info.pUserData = NULL;

    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
        instance->vk_instance,
        "vkCreateDebugUtilsMessengerEXT");

    if (!func) {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to set up debug messenger");
        return PRISMA_ERROR_VK;
    }

    if (func(instance->vk_instance, &debug_create_info, NULL, &instance->vk_debug_messenger) != VK_SUCCESS) {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to set up debug messenger");
        return PRISMA_ERROR_VK;
    }

    return PRISMA_ERROR_NONE;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL _debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT * cb_data,
    void * user_data)
{
    SAKE_MACRO_UNUSED(message_severity);
    SAKE_MACRO_UNUSED(message_type);
    SAKE_MACRO_UNUSED(cb_data);
    SAKE_MACRO_UNUSED(user_data);
 
    PRISMA_LOG_DEBUG("[VULKAN] %s\n", cb_data->pMessage);    
    return VK_FALSE;
}
#endif
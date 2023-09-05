#pragma once

#include <vulkan/vulkan.h>

#define VK_CHECK(res) { VkResult __macro__res_ = res; if (__macro__res_ != VK_SUCCESS) printf("Vulkan failure at: %s\n", #res); }

typedef struct vulkan_queues {
    VkQueue present_queue;
    VkQueue transfer_queue;
    VkQueue graphics_queue;

    int32_t present_queue_index;
    int32_t transfer_queue_index;
    int32_t graphics_queue_index;
} vulkan_queues;

typedef struct vulkan_device {
    VkDevice logical_device;
    VkPhysicalDevice physical_device;

    VkPhysicalDeviceProperties2 properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;

    vulkan_queues queues;
} vulkan_device;

typedef struct vulkan_renderer {
    /* Application State */
    const char* application_name;
    uint32_t width, height;

    VkInstance instance;

    VkSurfaceKHR surface;

    vulkan_device device;

    VkDebugUtilsMessengerEXT vulkan_messenger;
} vulkan_renderer;
#pragma once

#include <vulkan/vulkan.h>

#define VK_CHECK(res) if (res != VK_SUCCESS) { printf("%s%s\n", "Vulkan failure at: ", #res); }

typedef struct vulkan_device {
    VkDevice logical_device;
    VkPhysicalDevice physical_device;

    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;
} vulkan_device;

typedef struct vulkan_renderer {
    /* Application State */
    const char* application_name;
    uint32_t width, height;

    VkInstance instance;

    vulkan_device device;
} vulkan_renderer;
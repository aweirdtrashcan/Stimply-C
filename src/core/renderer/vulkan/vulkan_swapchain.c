#include "vulkan_swapchain.h"
#include <string.h>
#include <stdlib.h>
#include <alloca.h>
#include <stdio.h>

uint8_t vulkan_swapchain_create(const vulkan_renderer* renderer, vulkan_swapchain* out_swapchain) {
    if (out_swapchain == 0) {
        printf("vulkan_swapchain_create - Function called with a out_swapchain provided as null.\n");
        return 0;
    }

    if (!renderer->surface) {
        printf("vulkan_swapchain_create - Cannot create swapchain will null pointer to surface.\n");
        return 0;
    }

    VkSurfaceCapabilitiesKHR surface_capabilities;
    memset(&surface_capabilities, 0, sizeof(VkSurfaceCapabilitiesKHR));

    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        renderer->device.physical_device,
        renderer->surface,
        &surface_capabilities
    ));

    VkSurfaceFormatKHR* surface_formats = 0;

    uint32_t surface_format_count = 0;
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
        renderer->device.physical_device,
        renderer->surface,
        &surface_format_count,
        0
    ));

    if (surface_format_count > 0) {
        surface_formats = alloca(sizeof(VkSurfaceFormatKHR) * surface_format_count);
    }

    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
        renderer->device.physical_device,
        renderer->surface,
        &surface_format_count,
        surface_formats
    ));

    VkSurfaceFormatKHR swapchain_image_format;
    memset(&swapchain_image_format, 0, sizeof(VkSurfaceFormatKHR));

    for (uint32_t i = 0; i < surface_format_count; i++) {
        if (surface_formats[i].format == VK_FORMAT_B8G8R8A8_UNORM &&
            surface_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            swapchain_image_format = surface_formats[i];
            break;
        }
    }

    if (swapchain_image_format.colorSpace == 0 && swapchain_image_format.format == 0) {
        swapchain_image_format = surface_formats[0];
    }

    uint32_t min_image_count = surface_capabilities.minImageCount;
    uint32_t max_image_count = surface_capabilities.maxImageCount;
    uint32_t desired_image_count = 3;

    if (desired_image_count < min_image_count) {
        desired_image_count = min_image_count;
    }
    // maxImageCount can be 0, according to the Vulkan spec, which means no limit of image is imposed.
    if (max_image_count != 0 && desired_image_count > max_image_count) {
        desired_image_count = max_image_count;
    }

    VkPresentModeKHR* surface_present_modes = 0;
    uint32_t surface_present_mode_count = 0;
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
        renderer->device.physical_device,
        renderer->surface,
        &surface_present_mode_count,
        0
    ));

    if (surface_present_mode_count > 0) {
        surface_present_modes = alloca(sizeof(VkPresentModeKHR) * surface_present_mode_count);
    }

    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
        renderer->device.physical_device,
        renderer->surface,
        &surface_present_mode_count,
        surface_present_modes
    ));

    VkPresentModeKHR choosen_present_mode;

    for (uint32_t i = 0; i < surface_present_mode_count; i++) {
        if (surface_present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            choosen_present_mode = surface_present_modes[i];
            break;
        }
        if (surface_present_modes[i] == VK_PRESENT_MODE_FIFO_KHR) {
            choosen_present_mode = surface_present_modes[i];
            break;
        }
    }

    VkSwapchainCreateInfoKHR swapchain_create_info = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    swapchain_create_info.pNext = 0;
    swapchain_create_info.flags = 0;
    swapchain_create_info.surface = renderer->surface;
    swapchain_create_info.minImageCount = desired_image_count;
    swapchain_create_info.imageFormat = swapchain_image_format.format;
    swapchain_create_info.imageColorSpace = swapchain_image_format.colorSpace;
    swapchain_create_info.imageExtent = surface_capabilities.currentExtent;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (renderer->device.queues.graphics_queue_index == renderer->device.queues.present_queue_index) {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        uint32_t queue_indices[] = {
            renderer->device.queues.graphics_queue_index,
            renderer->device.queues.transfer_queue_index
        };
        swapchain_create_info.queueFamilyIndexCount = sizeof(uint32_t) / sizeof(queue_indices);
        swapchain_create_info.pQueueFamilyIndices = queue_indices;
    } else {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    swapchain_create_info.preTransform = surface_capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = choosen_present_mode;
    swapchain_create_info.clipped = VK_TRUE;
    swapchain_create_info.oldSwapchain = 0;

    VK_CHECK(vkCreateSwapchainKHR(
        renderer->device.logical_device,
        &swapchain_create_info,
        0,
        &out_swapchain->handle
    ));

    VK_CHECK(vkGetSwapchainImagesKHR(
        renderer->device.logical_device,
        out_swapchain->handle,
        &out_swapchain->image_count,
        0
    ));

    if (out_swapchain->image_count > 0) {

        if (out_swapchain->images != 0) {
            free(out_swapchain->images);
            out_swapchain->images = 0;
        }

        if (out_swapchain->views != 0) {
            free(out_swapchain->views);
            out_swapchain->views = 0;
        }

        out_swapchain->images = malloc(sizeof(VkImage) * out_swapchain->image_count);
        out_swapchain->views = malloc(sizeof(VkImageView) * out_swapchain->image_count);

        VK_CHECK(vkGetSwapchainImagesKHR(
            renderer->device.logical_device,
            out_swapchain->handle,
            &out_swapchain->image_count,
            out_swapchain->images
        ));

        for (uint32_t i = 0; i < out_swapchain->image_count; i++) {
            VkImageViewCreateInfo view_create_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
            view_create_info.components.a = VK_COMPONENT_SWIZZLE_A;
            view_create_info.components.b = VK_COMPONENT_SWIZZLE_B;
            view_create_info.components.g = VK_COMPONENT_SWIZZLE_G;
            view_create_info.components.r = VK_COMPONENT_SWIZZLE_R;
            view_create_info.flags = 0;
            view_create_info.format = swapchain_create_info.imageFormat;
            view_create_info.image = out_swapchain->images[i];
            view_create_info.pNext = 0;
            view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            view_create_info.subresourceRange.baseArrayLayer = 0;
            view_create_info.subresourceRange.baseMipLevel = 0;
            view_create_info.subresourceRange.layerCount = 1;
            view_create_info.subresourceRange.levelCount = 1;
            view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;

            VK_CHECK(vkCreateImageView(
                renderer->device.logical_device,
                &view_create_info,
                0,
                &out_swapchain->views[i]
            ));
        }
    }
    printf("Vulkan Swapchain created successfully!\n");
    return 1;
}

void vulkan_swapchain_destroy(const vulkan_renderer* renderer, vulkan_swapchain* swapchain) {
    printf("Destroying Vulkan Swapchain image views...\n");
    if (swapchain->image_count > 0) {

        for (uint32_t i = 0; i < swapchain->image_count; i++) {
            vkDestroyImageView(renderer->device.logical_device, swapchain->views[i], 0);
        }

        free(swapchain->images);
        free(swapchain->views);
    }

    swapchain->images = 0;
    swapchain->views = 0;
    swapchain->image_count = 0;

    printf("Destroying Vulkan swapchain...\n");
    if (swapchain->handle) {
        vkDestroySwapchainKHR(renderer->device.logical_device, swapchain->handle, 0);
    }
    swapchain->handle = 0;
}
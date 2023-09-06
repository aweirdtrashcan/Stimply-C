#pragma once

#include "vulkan_types.h"

uint8_t vulkan_swapchain_create(const vulkan_renderer* renderer, vulkan_swapchain* out_swapchain);

void vulkan_swapchain_destroy(const vulkan_renderer* renderer, vulkan_swapchain* swapchain);
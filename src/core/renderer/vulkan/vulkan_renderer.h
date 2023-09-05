#pragma once

#include "vulkan_types.h"

uint64_t vulkan_renderer_get_state_size();

uint8_t vulkan_renderer_initialize(void* vulkan_renderer_state, const char* application_name, uint32_t width, uint32_t height, void* window);

void vulkan_renderer_destroy(void* vulkan_renderer_state);
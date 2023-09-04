#pragma once

#include <stdint.h>

uint64_t renderer_get_state_size();

uint8_t renderer_initialize(void* allocated_memory, const char* application_name, uint32_t width, uint32_t height, void* window);

uint8_t renderer_shutdown(void* allocated_memory);
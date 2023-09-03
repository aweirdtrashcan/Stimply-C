#include "renderer.h"
#include "vulkan/vulkan_renderer.h"

#include <stdlib.h>
#include <stdio.h>

typedef struct renderer_state {
    void* vulkan_state;
} renderer_state;

static renderer_state* state;

uint64_t renderer_get_state_size() {
    return sizeof(renderer_state);
}

uint8_t renderer_initialize(void* allocated_memory, const char* application_name, uint32_t width, uint32_t height) {
    if (allocated_memory) {
        state = allocated_memory;
    } else {
        printf("%s\n", "renderer_initialized called with null pointer");
        return 0;
    }

    uint64_t vulkan_state_size = vulkan_renderer_get_state_size();
    state->vulkan_state = malloc(vulkan_state_size);
    vulkan_renderer_initialize(state->vulkan_state, application_name, width, height);

    return 1;
}

uint8_t renderer_shutdown(void* allocated_memory) {
    vulkan_renderer_destroy(state->vulkan_state);
    
    if (state->vulkan_state) {
        free(state->vulkan_state);
    }

    state->vulkan_state = 0;
    
    return 1;
}
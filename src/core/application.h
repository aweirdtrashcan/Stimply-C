#pragma once

#include <stdint.h>

uint8_t application_start(
    const uint32_t width,
    const uint32_t height,
    const char* application_name,
    void** out_application_state
);

uint8_t application_run(void* application_state);

void application_destroy(void* application_state);

void* application_get_window(void* application_state);
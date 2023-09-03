#include "application.h"
#include "renderer/renderer.h"

#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <string.h>

typedef struct application_state {
    GLFWwindow* window;
    uint32_t width;
    uint32_t height;
    const char* application_name;
    void* renderer_state;
} application_state;

void check_key_state(uint32_t key, GLFWwindow* window);

uint8_t application_start(
    const uint32_t width,
    const uint32_t height,
    const char* application_name,
    void** out_application_state
) {
    application_state* state = malloc(sizeof(application_state));
    *out_application_state = state;
    memset(state, 0, sizeof(application_state));

    state->width = width;
    state->height = height;
    state->application_name = application_name;

    if (glfwInit() != GLFW_TRUE) {
        return 0;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window = glfwCreateWindow(state->width, state->height, state->application_name, 0, 0);

    if (window == 0) {
        return 0;
    }

    state->window = window;

    glfwShowWindow(window);

    uint64_t renderer_size = renderer_get_state_size();
    state->renderer_state = malloc(renderer_size);
    renderer_initialize(state->renderer_state, state->application_name, state->width, state->height);

    return 1;
}

uint8_t application_run(void* app_state) {
    application_state* state = app_state;

    while (!glfwWindowShouldClose(state->window)) {
        glfwPollEvents();
        check_key_state(GLFW_KEY_ESCAPE, state->window);
        glfwSwapBuffers(state->window);
    }

    renderer_shutdown(state->renderer_state);

    return 1;
}

void application_destroy(void* app_state) {
    application_state* state = app_state;

    glfwDestroyWindow(state->window);
    glfwTerminate();
    free(app_state);
}

void check_key_state(uint32_t key, GLFWwindow* window) {
    if (glfwGetKey(window, key) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}
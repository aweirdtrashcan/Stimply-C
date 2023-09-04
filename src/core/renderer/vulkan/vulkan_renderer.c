#include "vulkan_renderer.h"
#include "vulkan_device.h"
#include "../../application.h"

#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <string.h>

#include <GLFW/glfw3.h>

#define V1_0 (VK_MAKE_API_VERSION(0, 1, 0, 0))

/* Don't hold on to this memory */
typedef struct vulkan_renderer_initialization_data {
    uint32_t supported_layer_extension_count;
    VkExtensionProperties* supported_layer_extensions;

    uint32_t requested_layer_extension_count;
    const char** requested_layer_extensions;
} vulkan_renderer_initialization_data;

static vulkan_renderer_initialization_data* temp;

uint64_t vulkan_renderer_get_state_size() {
    return sizeof(vulkan_renderer);
}

uint8_t vulkan_renderer_initialize(void* vulkan_renderer_state, const char* application_name, uint32_t width, uint32_t height, void* window) {
    vulkan_renderer* state = vulkan_renderer_state;
    memset(state, 0, sizeof(vulkan_renderer));

    VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app_info.pApplicationName = application_name;
    app_info.pEngineName = "Stimply Engine";
    app_info.applicationVersion = V1_0;
    app_info.engineVersion = V1_0;
    app_info.pNext = 0;

    VkInstanceCreateInfo instance_create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    instance_create_info.pApplicationInfo = &app_info;

    temp = malloc(sizeof(vulkan_renderer_initialization_data));
    memset(temp, 0, sizeof(vulkan_renderer_initialization_data));

    // Query for instance supported extensions/version
    uint32_t api_version;
    if (vkEnumerateInstanceVersion(&api_version) != VK_SUCCESS) {
        printf("Failed to enumerate Vulkan instance version.\n");
        return 0;
    }
    app_info.apiVersion = api_version;

    printf("%s: %i.%i.%i.%i\n", "Found Vulkan Instance version of: ",
        VK_API_VERSION_VARIANT(api_version),
        VK_API_VERSION_MAJOR(api_version),
        VK_API_VERSION_MINOR(api_version),
        VK_API_VERSION_PATCH(api_version)
    );

    VK_CHECK(vkEnumerateInstanceExtensionProperties(0, &temp->supported_layer_extension_count, 0));
    temp->supported_layer_extensions = malloc(sizeof(VkExtensionProperties) * temp->supported_layer_extension_count);
    VK_CHECK(vkEnumerateInstanceExtensionProperties(0, &temp->supported_layer_extension_count, temp->supported_layer_extensions));

    printf("Supported Instance Extensions:\n");
    for (uint32_t i = 0; i < temp->supported_layer_extension_count; i++) {
        printf("  %s\n", temp->supported_layer_extensions[i].extensionName);
    }

    // 1 KiloByte... should be enough
    temp->requested_layer_extensions = malloc(1000);
    memset(temp->requested_layer_extensions, 0, sizeof(1000));

    temp->requested_layer_extensions[temp->requested_layer_extension_count++] = "VK_KHR_surface";
    
#ifdef S_LINUX_
    temp->requested_layer_extensions[temp->requested_layer_extension_count++] = "VK_KHR_xcb_surface";
#elif S_WINDOWS_
    temp->requested_layer_extensions[temp->requested_layer_extension_count++] = "VK_KHR_win32_surface";
#endif

#ifdef _DEBUG
    temp->requested_layer_extensions[temp->requested_layer_extension_count++] = "VK_EXT_debug_utils";
#endif

    instance_create_info.enabledExtensionCount = temp->requested_layer_extension_count;
    instance_create_info.ppEnabledExtensionNames = temp->requested_layer_extensions;
    instance_create_info.ppEnabledLayerNames = 0;
    instance_create_info.enabledLayerCount = 0;
    instance_create_info.flags = 0;
    instance_create_info.pNext = 0;

    // Check if requested extensions are supported.
    for (uint32_t i = 0; i < temp->requested_layer_extension_count; i++) {
        char found = 0;
        for (uint32_t j = 0; j < temp->supported_layer_extension_count; j++) {
            if (strcmp(temp->supported_layer_extensions[j].extensionName, temp->requested_layer_extensions[i]) == 0) {
                printf("Instance Extension %s was found successfully!\n", temp->supported_layer_extensions[j].extensionName);
                found = 1;
            }
        }
        if (!found) {
            printf("Failed to find Instance extension: %s. Leaving application...\n", temp->requested_layer_extensions[i]);
            return 0;
        }
    }

    VK_CHECK(vkCreateInstance(
        &instance_create_info,
        0,
        &state->instance
    ));

    VK_CHECK(glfwCreateWindowSurface(state->instance, window, 0, &state->surface));

    if (!vulkan_device_create(state)) {
        printf("%s\n", "Failed to create Vulkan device.");
        return 0;
    }

    // Renderer all initialized, no need to keep the temporary data...
    free(temp->requested_layer_extensions);
    free(temp);
    temp = 0;

    printf("Vulkan renderer initialized successfully\n");

    return 1;
}

void vulkan_renderer_destroy(void* vulkan_renderer_state) {
    vulkan_renderer* state = vulkan_renderer_state;
    
    vulkan_device_destroy(state);

    printf("%s\n", "Destroying Vulkan instance...");
    vkDestroyInstance(state->instance, 0);
}
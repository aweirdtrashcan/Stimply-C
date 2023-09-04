#include "vulkan_types.inl"

#include <alloca.h>
#include <stdio.h>

uint8_t vulkan_device_create(vulkan_renderer* renderer) {
    // Choose physical device
    uint32_t physical_devices_count = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(renderer->instance, &physical_devices_count, 0));
    VkPhysicalDevice* physical_devices = alloca(sizeof(VkPhysicalDevice) * physical_devices_count);
    VkResult result = vkEnumeratePhysicalDevices(renderer->instance, &physical_devices_count, physical_devices);

    if (result == VK_INCOMPLETE) {
        printf("%s%s%i\n", "Not all the physical devices available were listed on the physical_devices array.", __FILE__, __LINE__);
    } else if (result != VK_SUCCESS) {
        printf("%s\n", "Failed to list all the available physical_devices array.");
    }

    uint32_t memory_size = 0;

    for (uint32_t i = 0; i < physical_devices_count; i++) {
        VkPhysicalDeviceProperties2 properties = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR};
        VkPhysicalDeviceDriverProperties driver_props = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES_KHR};
        properties.pNext = &driver_props;
        vkGetPhysicalDeviceProperties2(physical_devices[i], &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physical_devices[i], &features);

        VkPhysicalDeviceMemoryProperties memory;
        vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &memory);

        uint8_t discrete = properties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        
        uint32_t gpu_mem;
        for (uint32_t i = 0; i < memory.memoryHeapCount; i++) {
            gpu_mem += memory.memoryHeaps[i].size;
        }

        if (!(gpu_mem > memory_size)) {
            continue;
        }

        if (features.samplerAnisotropy != VK_TRUE) {
            continue;
        }

        if (!discrete) {
            printf("Device: %s is not a discrete device.\n", properties.properties.deviceName);
            continue;
        }

        // If gets to this point, this will be the selected device.

        renderer->device.features = features;
        renderer->device.memory = memory;
        renderer->device.properties = properties;
        renderer->device.physical_device = physical_devices[i];

        uint32_t queue_family_property_count;
        vkGetPhysicalDeviceQueueFamilyProperties(
            physical_devices[i],
            &queue_family_property_count,
            0
        );

        VkQueueFamilyProperties* queue_family_properties = alloca(sizeof(VkQueueFamilyProperties) * queue_family_property_count);
        vkGetPhysicalDeviceQueueFamilyProperties(
            physical_devices[i],
            &queue_family_property_count,
            queue_family_properties
        );

        renderer->device.queues.graphics_queue_index = UINT32_MAX;
        renderer->device.queues.present_queue_index = UINT32_MAX;
        renderer->device.queues.transfer_queue_index = UINT32_MAX;

        for (uint32_t i = 0; i < queue_family_property_count; i++) {
            uint8_t is_graphics = queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;
            uint8_t is_compute = queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT;
            uint8_t is_transfer = queue_family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT; 
            VkBool32 is_present;
            VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(
                renderer->device.physical_device,
                i,
                renderer->surface,
                &is_present
            ));

            if (is_transfer && !is_graphics && !is_present && !is_compute) {
                renderer->device.queues.transfer_queue_index = i;
                continue;
            }

            if (is_present == VK_TRUE) {
                renderer->device.queues.present_queue_index = i;
            }

            if (is_graphics) {
                renderer->device.queues.graphics_queue_index = i;
            }

            if (is_transfer) {
                renderer->device.queues.transfer_queue_index = i;
            }
        }
    }

    // Create a logical device.

    uint32_t extensions_supported_count;
    VK_CHECK(vkEnumerateDeviceExtensionProperties(
        renderer->device.physical_device,
        0,
        &extensions_supported_count,
        0
    ));

    VkExtensionProperties* supported_extensions = alloca(sizeof(VkExtensionProperties) * extensions_supported_count);
    VK_CHECK(vkEnumerateDeviceExtensionProperties(
        renderer->device.physical_device,
        0,
        &extensions_supported_count,
        supported_extensions
    ));

    printf("Supported device extensions:\n");
    for (uint32_t i = 0; i < extensions_supported_count; i++) {
        printf("  %s\n", supported_extensions[i].extensionName);
    }

    uint32_t requested_extensions_count = 0;
    const char** requested_extensions = alloca(1000);

    requested_extensions[requested_extensions_count++] = "VK_KHR_swapchain";

    VkDeviceCreateInfo device_create_info = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    device_create_info.enabledLayerCount = 0;
    device_create_info.ppEnabledLayerNames = 0;
    device_create_info.enabledExtensionCount = requested_extensions_count;
    device_create_info.ppEnabledExtensionNames = requested_extensions;

    uint8_t queue_indices[3] = {
        renderer->device.queues.graphics_queue_index,
        renderer->device.queues.present_queue_index,
        renderer->device.queues.transfer_queue_index
    };

    VkDeviceQueueCreateInfo device_queue_create_infos[3];
    for (uint32_t i = 0; i < 3; i++) {
        device_queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        device_queue_create_infos[i].pNext = 0;
        device_queue_create_infos[i].flags = 0;
        device_queue_create_infos[i].queueFamilyIndex = queue_indices[i];
        device_queue_create_infos[i].queueCount = 1;
        const float priorities = 1.0f;
        device_queue_create_infos[i].pQueuePriorities = &priorities;
    }

    device_create_info.queueCreateInfoCount = 3;
    device_create_info.pQueueCreateInfos = device_queue_create_infos;
    VkPhysicalDeviceFeatures features = {};
    features.samplerAnisotropy = VK_TRUE;
    device_create_info.pEnabledFeatures = &features;

    VK_CHECK(vkCreateDevice(
        renderer->device.physical_device,
        &device_create_info,
        0,
        &renderer->device.logical_device
    ));

    printf("Vulkan device created successfully!\n");

    return 1;
}

void vulkan_device_destroy(vulkan_renderer* renderer) {

    if (renderer->device.logical_device) {
        printf("Destroying vulkan device...\n");
        vkDestroyDevice(renderer->device.logical_device, 0);
    }

    printf("Vulkan device destroyed successfully!\n");

}
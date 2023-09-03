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

    for (uint32_t i = 0; i < physical_devices_count; i++) {
        VkPhysicalDeviceProperties2 properties = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR};
        VkPhysicalDeviceDriverProperties driver_props = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES_KHR};
        properties.pNext = &driver_props;
        vkGetPhysicalDeviceProperties2(physical_devices[i], &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physical_devices[i], &features);

        VkPhysicalDeviceMemoryProperties memory;
        vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &memory);

        
    }

    return 1;
}

void vulkan_device_destroy(vulkan_renderer* renderer) {

}
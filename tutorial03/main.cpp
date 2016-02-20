///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  TUTORIAL 03 - DEVICE CREATION                                            //
//                                                                           //
//  Creation of a logical device                                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Application Name
const char *g_appName = "vulkan-tut03";

///////////////////////////////////////////////////////////////////////////////
//
//  DESCRIPTION:
//            It creates a Vulkan Instance
//  INPUT:
//            OUT   vkInst   - Pointer where the new instance is created
//  OUTPUT:
//            OUT   vkResult - VK_SUCCESS if everything went right.
//                           - On error any of these values (check documentation
//                             for further details):
//                              VK_ERROR_OUT_OF_HOST_MEMORY
//                              VK_ERROR_OUT_OF_DEVICE_MEMORY
//                              VK_ERROR_INITIALIZATION_FAILED
//                              VK_ERROR_LAYER_NOT_PRESENT
//                              VK_ERROR_EXTENSION_NOT_PRESENT
//                              VK_ERROR_INCOMPATIBLE_DRIVER
//
VkResult createInstance(VkInstance *vkInst)
{
    VkApplicationInfo     appInfo = {};
    VkInstanceCreateInfo  instInfo = {};

    VkResult res = VK_SUCCESS;

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = NULL;
    appInfo.pApplicationName = g_appName;
    appInfo.pEngineName = NULL;
    appInfo.engineVersion = 1;
    appInfo.apiVersion = VK_API_VERSION;

    instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instInfo.pNext = NULL;
    instInfo.flags = 0;
    instInfo.pApplicationInfo = &appInfo;

    // Neither layers nor extension are going to be available
    instInfo.enabledLayerCount = 0;
    instInfo.ppEnabledLayerNames = NULL;
    instInfo.enabledExtensionCount = 0;
    instInfo.enabledExtensionCount = NULL;

    res = vkCreateInstance(&instInfo, NULL, vkInst);

    return res;
}

///////////////////////////////////////////////////////////////////////////////
//
//  DESCRIPTION:
//          Get Queue Families available in a physical device
//
//  INPUT:
//          IN    vkPhysDev    - Physical Device to be queried
//          OUT   i_Count      - Number of queue families found
//          OUT   vkQuFamProps - Array of VkQueueFamilyProperties objects
//
//  OUTPUT:
//                vkResult   - VK_SUCCESS if everything went right.
//                           - On error any of these values (check documentation
//                             for further details):
//                              VK_ERROR_OUT_OF_HOST_MEMORY
//
VkResult getQueueFamilyProperites(VkPhysicalDevice vkPhysDev,
    uint32_t *i_Count, VkQueueFamilyProperties **vkQuFamProps)
{
    // Get number of queue families available
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysDev, i_Count, NULL);

    *vkQuFamProps = (VkQueueFamilyProperties *)
        malloc(sizeof(VkQueueFamilyProperties)*(*i_Count));

    if (*vkQuFamProps == NULL)
    {
        printf("Couldn't allocate memory for the physical device array: %d\n", errno);
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    // Get a list with the queue family properties
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysDev, i_Count, *vkQuFamProps);

    return VK_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//
//  DESCRIPTION:
//          It creates a VkPhysicalDevice array with all present physical
//          devices.
//  INPUT:
//          IN    vkInst     - Vulkan instance previously initialized
//          OUT   i_devCount - Pointer where the number of present devices will be
//                             stored.
//          OUT   vkPhysDev  - Pointer where the array of VkPhysicalDevice objects
//                             will be created.
//  OUTPUT:
//                vkResult   - VK_SUCCESS if everything went right.
//                           - On error any of these values (check documentation
//                             for further details):
//                              VK_ERROR_OUT_OF_HOST_MEMORY
//                              VK_ERROR_OUT_OF_DEVICE_MEMORY
//                              VK_ERROR_INITIALIZATION_FAILED
//

VkResult createPhysicalDevicesArray(VkInstance vkInst,
    uint32_t *i_devCount, VkPhysicalDevice **vkPhysDev)
{
    VkResult res = VK_SUCCESS;

    // First query how many devices are present in the system
    res = vkEnumeratePhysicalDevices(vkInst, i_devCount, NULL);
    if (res != VK_SUCCESS)
    {
        printf("Failed to query number of physical devices present: %d\n", res);
        return res;
    }

    // Verify that there are at least 1 available device present
    if (*i_devCount == 0)
    {
        printf("Couldn't detect any device present with Vulkan support: %d\n", res);
        return res;
    }

    // Allocate enough memory to enumarate all physical devices
    *vkPhysDev = (VkPhysicalDevice *)malloc(sizeof(VkPhysicalDevice) * (*i_devCount));
    if (*vkPhysDev == NULL)
    {
        printf("Couldn't allocate memory for the physical device array: %d\n", errno);
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    // Enumarate all physical devices present
    res = vkEnumeratePhysicalDevices(vkInst, i_devCount, *vkPhysDev);
    if (res != VK_SUCCESS)
    {
        printf("Failed to enumerate physical devices present: %d\n", res);
        return res;
    }

    return res;
}

///////////////////////////////////////////////////////////////////////////////
//
//  DESCRIPTION:
//               Device Creation
//   
VkResult createDevice(VkPhysicalDevice vkPhysDev, VkDevice *vkDev)
{
    VkDeviceCreateInfo vkDevInfo = {};
    VkQueueFamilyProperties *vkQuFamProps = NULL;
    VkDeviceQueueCreateInfo  vkDevQuInfo = {};

    VkResult res = VK_SUCCESS;

    uint32_t i_quFamCnt = 0;

    // Only one queue will be created, so only one priority 
    // is needed
    float f_quPriority = 1.0;

    // Initialize device creation struct with mandatory values
    vkDevInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    vkDevInfo.pNext = NULL;
    vkDevInfo.flags = 0;

    // Neither layers nor extensions will be enabled
    vkDevInfo.enabledLayerCount = 0;
    vkDevInfo.ppEnabledLayerNames = NULL;
    vkDevInfo.enabledExtensionCount = 0;
    vkDevInfo.ppEnabledExtensionNames = NULL;

    // Initialize queue creation structure
        // First, get a list of queue families to be verify that there are
        // queue families available
        res = getQueueFamilyProperites(vkPhysDev, &i_quFamCnt, &vkQuFamProps);
        if (res != VK_SUCCESS)
        {
            goto cleanup;
        }

        // Fill mandatory values
        vkDevQuInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        vkDevQuInfo.pNext = NULL;
        vkDevQuInfo.flags = 0;
        // Use first queue family in the family list
        vkDevQuInfo.queueFamilyIndex = 0;
        // Create only one queue
        vkDevQuInfo.queueCount = 1;
        vkDevQuInfo.pQueuePriorities = &f_quPriority;
    
    // Setting previously defined queue creation struct
    // into our device creation struct
    vkDevInfo.queueCreateInfoCount = 1;
    vkDevInfo.pQueueCreateInfos = &vkDevQuInfo;

    // Calling the device creation function
    res = vkCreateDevice(vkPhysDev, &vkDevInfo, NULL, vkDev);

 cleanup:
    if (vkQuFamProps)
    {
        free(vkQuFamProps);
        vkQuFamProps = NULL;
    }
    return res;
}

///////////////////////////////////////////////////////////////////////////////
//
//    DESCRIPTION:
//                main function                                                
//                          

int main(int argc, char **argv)
{
    VkInstance vkInst = (VkInstance)0;
    VkPhysicalDevice *vkPhysDev = NULL;
    VkDevice vkDev = (VkDevice)0;

    VkResult res = VK_SUCCESS;

    uint32_t    i_devCount = 0;

    // Create an instance
    res = createInstance(&vkInst);
    if (res != VK_SUCCESS)
    {
        printf("Failed to create instance: %d\n", res);
        return -1;
    }

    printf(" Instance created.\n");

    // Create an array with all present devices
    res = createPhysicalDevicesArray(vkInst, &i_devCount, &vkPhysDev);
    if (res != VK_SUCCESS)
    {
        printf("Failed to create physical devices list: %d\n", res);
        goto cleanup;
    }

    printf(" List of queue families, obtained.\n");

    // Print queue family information for each device found
    res = createDevice(vkPhysDev[0], &vkDev);
    if (res != VK_SUCCESS)
    {
        printf("Failed to create device: %d\n", res);
        goto cleanup;
    }

    printf(" Logical device created.\n");

cleanup:
    // Destroy device
    vkDestroyDevice(vkDev, NULL);

    // Free allocated resources
    if (vkPhysDev)
    {
        free(vkPhysDev);
    }

    // Destroy Vulkan Instance
    vkDestroyInstance(vkInst, NULL);

    printf(" Cleanup done.\n");

    return 0;
}
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  TUTORIAL 02 - QUEUE FAMILY ENUMERATION                                   //
//                                                                           //
//  Enumeration of queue families of each avalaible physica device           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Application Name
const char *g_appName = "vulkan-tut02";

///////////////////////////////////////////////////////////////////////////////
//
//  DESCRIPTION:
//            It creates a Vulkan Instance
//  INPUT:
//            OUT        vkInst     - Pointer where the new instance is created
//  OUTPUT:
//            OUT        vkResult - VK_SUCCESS if everything went right.
//                             - On error any of these values (check documentation
//                               for further details):
//                               VK_ERROR_OUT_OF_HOST_MEMORY
//                               VK_ERROR_OUT_OF_DEVICE_MEMORY
//                               VK_ERROR_INITIALIZATION_FAILED
//                               VK_ERROR_LAYER_NOT_PRESENT
//                               VK_ERROR_EXTENSION_NOT_PRESENT
//                               vK_ERROR_INCOMPATIBLE_DRIVER
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
//               Print information of the queue families of all of the 
//               physical devices found
//   
//  INPUT:
//          IN   vkPhysDev  - List of physical devices to query
//          IN   i_Count    - Number of physical devices in the previous list
//  OUTPUT:
//  
void printQueueFamilyInfo(VkPhysicalDevice *vkPhysDev, uint32_t i_Count)
{
    VkPhysicalDeviceProperties vkDevProp = {};
    VkQueueFamilyProperties *vkQuFamProp = NULL;

    VkResult res;

    uint32_t i, j;
    uint32_t i_famCount;

    for (i = 0; i < i_Count; i++)
    {
        i_famCount = 0;

        // Get general device properties
        memset(&vkDevProp, 0, sizeof(VkPhysicalDeviceProperties));
        vkGetPhysicalDeviceProperties(vkPhysDev[i], &vkDevProp);

        printf(" Device Name: %s (Type: %d)\n", 
            vkDevProp.deviceName, vkDevProp.deviceType);

        // Get the list of queue family properties for the current device
        res = getQueueFamilyProperites(vkPhysDev[i], &i_famCount, &vkQuFamProp);
        printf(" Device Queue Family Count: %d\n", i_famCount);

        if (res != VK_SUCCESS)
        {
            printf("\tError: Could no get the list of queue families: %d\n", res);
        }
        else
        {
            for (j = 0; j < i_famCount; j++)
            {
                // Queue Family index is used in several other vulkan operations
                printf("\t Index: %d\n", j);

                printf("\t Count of Queues: %d\n", vkQuFamProp[j].queueCount);
                printf("\t Minimum Image Transfer Granularity (Width Height Depth): (%d %d %d)\n",
                    vkQuFamProp[j].minImageTransferGranularity.width,
                    vkQuFamProp[j].minImageTransferGranularity.height,
                    vkQuFamProp[j].minImageTransferGranularity.depth);

                printf("\t Supported operations on this queue:\n");
                if (vkQuFamProp[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    printf("\t\t Graphics\n");
                if (vkQuFamProp[j].queueFlags & VK_QUEUE_COMPUTE_BIT)
                    printf("\t\t Compute\n");
                if (vkQuFamProp[j].queueFlags & VK_QUEUE_TRANSFER_BIT)
                    printf("\t\t Transfer\n");
                if (vkQuFamProp[j].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
                    printf("\t\t Sparse Binding\n");

                printf("\n");
            }
        }

        // Free allocated resources
        if (vkQuFamProp != NULL)
        {
            free(vkQuFamProp);
            vkQuFamProp = NULL;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//    DESCRIPTION:
//                main function                                                
//                          

int main(int argc, char **argv)
{
    VkInstance vkInst=(VkInstance) 0;
    VkPhysicalDevice *vkPhysDev = NULL;
    VkResult res = VK_SUCCESS;

    uint32_t    i_devCount = 0;

    // Create an instance
    res = createInstance(&vkInst);
    if (res != VK_SUCCESS)
    {
        printf("Failed to create instance: %d\n", res);
        return -1;
    }

    // Create an array with all present devices
    res = createPhysicalDevicesArray(vkInst, &i_devCount, &vkPhysDev);
    if (res != VK_SUCCESS)
    {
        printf("Failed to create physical devices list: %d\n", res);
        goto cleanup;
    }

    // Print queue family information for each device found
    printQueueFamilyInfo(vkPhysDev, i_devCount);
    
cleanup:
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
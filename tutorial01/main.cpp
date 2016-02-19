///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  TUTORIAL 01 - PHYSICAL DEVICES ENUMERATION                               //
//                                                                           //
//  Enumeration of all physical devices present on the system                //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Application Name
const char *g_appName = "vulkan-tut01";

// Macros to decode API version
// From the API documentation:
// The major version number is a 10 - bit integer packed into bits 31 - 22.
// The minor version number is a 10 - bit integer packed into bits 21 - 12.
// The patch version number is a 12 - bit integer packed into bits 11 - 0.
#define VK_API_MAJOR_VERSION(x) ((x>>22)&0x3FF)
#define VK_API_MINOR_VERSION(x) ((x>>12)&0x3FF)
#define VK_API_PATCH_VERSION(x) (x&0xFFF)


///////////////////////////////////////////////////////////////////////////////
//
//	DESCRIPTION:
//				It creates a Vulkan Instance
//  INPUT:
//			OUT		vkInst	 - Pointer where the new instance is created
//  OUTPUT:
//			OUT		vkResult - VK_SUCCESS if everything went right.
//							 - On error any of these values (check documentation
//							   for further details):
//							   VK_ERROR_OUT_OF_HOST_MEMORY
//							   VK_ERROR_OUT_OF_DEVICE_MEMORY
//							   VK_ERROR_INITIALIZATION_FAILED
//							   VK_ERROR_LAYER_NOT_PRESENT
//							   VK_ERROR_EXTENSION_NOT_PRESENT
//							   vK_ERROR_INCOMPATIBLE_DRIVER
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
//	DESCRIPTION:
//				It creates a VkPhysicalDevice array with all present physical
//              devices.
//  INPUT:
//          IN  vkInst     - Vulkan instance previously initialized
//			OUT	i_devCount - Pointer where the number of present devices will be
//                           stored.
//          OUT vkPhysDev  - Pointer where the array of VkPhysicalDevice objects
//                           will be created.
//  OUTPUT:
//				vkResult	- VK_SUCCESS if everything went right.
//							- On error any of these values (check documentation
//							  for further details):
//							  VK_ERROR_OUT_OF_HOST_MEMORY
//							  VK_ERROR_OUT_OF_DEVICE_MEMORY
//							  VK_ERROR_INITIALIZATION_FAILED
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
//	DESCRIPTION:
//				It prints the device (BASIC) properties associated to each 
//              VkPhysicalDevice object in the array.
//  INPUT:
//			IN i_devCount - Number of entries in the devices array
//          IN vkPhysDev  - Array of VkPhysicalDevice objects
//  OUTPUT:
//

void printPhysicalDevicesProperties(
    uint32_t i_devCount, VkPhysicalDevice *vkPhysDev)
{
    VkPhysicalDeviceProperties vkDevProp = {};
    uint32_t i=0;

    printf(" %d physical device(s) found: \n", i_devCount);

    for (i = 0; i < i_devCount; i++)
    {
        memset(&vkDevProp, 0, sizeof(VkPhysicalDeviceProperties));

        vkGetPhysicalDeviceProperties(vkPhysDev[i], &vkDevProp);

        printf("\tAPI Version: %d.%d.%d\n",
            VK_API_MAJOR_VERSION(vkDevProp.apiVersion),
            VK_API_MINOR_VERSION(vkDevProp.apiVersion),
            VK_API_PATCH_VERSION(vkDevProp.apiVersion));
        printf("\tDriver Version: %d\n", vkDevProp.driverVersion);
        printf("\tDevice Name: %s\n", vkDevProp.deviceName);
        // From the API documentation:
        //     VK_PHYSICAL_DEVICE_TYPE_OTHER = 0,
        //     VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU = 1,
        //     VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU = 2,
        //     VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU = 3,
        //     VK_PHYSICAL_DEVICE_TYPE_CPU = 4,
        printf("\tDevice Type: %d\n", vkDevProp.deviceType);

        printf("\n");
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//	DESCRIPTION:
//				main function                                                
//                          

int main(int argc, char **argv)
{
	VkInstance vkInst;
	VkPhysicalDevice *vkDev = NULL;
	VkResult res = VK_SUCCESS;

	uint32_t	i_devCount=0;

	// Create an instance
	res = createInstance(&vkInst);
	if (res != VK_SUCCESS)
	{
		printf("Failed to create instance: %d\n", res);
		return -1;
	}

    // Create an array with all present devices
    res = createPhysicalDevicesArray(vkInst, &i_devCount, &vkDev);
    if (res != VK_SUCCESS)
    {
        printf("Failed to create instance: %d\n", res);
        return -1;
    }

    // Print the properties of all of the collected device objects
    printPhysicalDevicesProperties(i_devCount, vkDev);

    // Free allocated resources
	if (vkDev)
	{
		free(vkDev);
	}

	vkDestroyInstance(vkInst, NULL);

	return 0;
}
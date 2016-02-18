#include <vulkan/vulkan.h>

#include <stdio.h>

const char *g_appName = "vulkan-tut00";

int main(int argc, char **argv)
{
	// Structures to describe application and instance information
	VkApplicationInfo     appInfo = {};
	VkInstanceCreateInfo  instInfo = {};

	// Vulkan Instance Handle
	VkInstance	vkInst;

	// Result variable
	VkResult res = VK_SUCCESS;

	// Filling out application description
	// Mandatory sType value
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	// Mandatory pNext value
	appInfo.pNext = NULL;
	// The name of our application
	appInfo.pApplicationName = g_appName;
	// Name of our engine. None for the moment
	appInfo.pEngineName = NULL;
	// Just setting an initial value
	appInfo.engineVersion = 1;
	// Using default API version
	appInfo.apiVersion = VK_API_VERSION;

	// Filling out instance description
	// Mandatory sType value
	instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	// Mandatory pNext value
	instInfo.pNext = NULL;
	// Mandatory flags value
	instInfo.flags = 0;
	// Using our previous defined application info
	instInfo.pApplicationInfo = &appInfo;
	// Don't enable any layer
	instInfo.enabledLayerCount = 0;
	instInfo.ppEnabledLayerNames = NULL;
	// Don't enable any extension
	instInfo.enabledExtensionCount = 0;
	instInfo.enabledExtensionCount = NULL;

	// Creating the desired instance
	res = vkCreateInstance(&instInfo, NULL, &vkInst);

	// Checking errors
	if (res != VK_SUCCESS)
	{
		printf("Failed to create instance: %d\n", res);
		return -1;
	}

	printf("Instance created successfully\n");

	// Destroying the instance
	vkDestroyInstance(vkInst, NULL);

	printf("Instance destroyed\n");

	return 0;
}
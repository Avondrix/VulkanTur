//
// Created by Avon on 2022/11/17.
//

#include "TriangleApp.hpp"

void TriangleApp::run() {
    initWindow();
    initVulkan();
    mainloop();
    cleanup();
}

void TriangleApp::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Tur", nullptr, nullptr);
}

void TriangleApp::initVulkan() {
    createInstance();
    setupDebugMessenger();
    pickPhysicalDevice();
    createLogicalDevice();
}

void TriangleApp::mainloop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

void TriangleApp::cleanup() {
    if (enableValidation) {
        DestroyDebugMessenger(instance, debugMessenger, nullptr);
    }
    vkDestroyDevice(logicalDevice, nullptr);
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);
    glfwTerminate();
}

void TriangleApp::createInstance() {
    if (enableValidation && !checkValidationLayersSupport()) {
        throw std::runtime_error("validation layers required, but not support.");
    }

    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pApplicationName = "Vulkan Triangle";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = "NO ENGINE";
    applicationInfo.apiVersion = VK_API_VERSION_1_0;

    displaySupportExtensions();
    auto extensions{getRequiredExtensions()};

    VkInstanceCreateInfo instanceCreateInfo{};

    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
    instanceCreateInfo.enabledExtensionCount = extensions.size();

    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoExt{};
    if (enableValidation) {
        instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
        instanceCreateInfo.enabledLayerCount = validationLayers.size();
        populateDebugMessengerCreateInfo(debugUtilsMessengerCreateInfoExt);
        instanceCreateInfo.pNext = &debugUtilsMessengerCreateInfoExt;
    } else {
        instanceCreateInfo.enabledLayerCount = 0;
        instanceCreateInfo.ppEnabledLayerNames = nullptr;
    }

    auto result{vkCreateInstance(&instanceCreateInfo, nullptr, &instance)};
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create instance.");
    }
}

void TriangleApp::displaySupportExtensions() {
    uint32_t extensionCount{0};
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties > extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::cout << "support extensions: " << std::endl;
    for (const auto &extension: extensions) {
        std::cout << "\t" << extension.extensionName << std::endl;
    }
}

std::vector<const char *> TriangleApp::getRequiredExtensions() const {
    uint32_t glfwExtensionCount{0};
    auto glfwExtensions{glfwGetRequiredInstanceExtensions(&glfwExtensionCount)};

    std::vector<const char *> extensions;

    for (uint32_t i{0}; i < glfwExtensionCount; i++) {
        extensions.emplace_back(glfwExtensions[i]);
    }

    // Mac
    extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    extensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    if (enableValidation) {
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

VkBool32 TriangleApp::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                    VkDebugUtilsMessageTypeFlagsEXT messageFlag,
                                    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

void TriangleApp::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfoExt) {
    createInfoExt = {};
    createInfoExt.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfoExt.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
    createInfoExt.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
    createInfoExt.pUserData = nullptr;
    createInfoExt.pfnUserCallback = debugCallback;
}

void TriangleApp::setupDebugMessenger() {
    if (!enableValidation) {
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfoExt{};
    populateDebugMessengerCreateInfo(createInfoExt);

    auto result{CreateDebugMessenger(instance, &createInfoExt, nullptr, &debugMessenger)};
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create debug messenger.");
    }
}

bool TriangleApp::checkValidationLayersSupport() {
    uint32_t layerCount{0};
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> layers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

    std::cout << "support layers: " << std::endl;
    for (const auto &layer: layers) {
        std::cout << "\t" << layer.layerName << std::endl;
    }

    for (const auto validationLayer: validationLayers) {
        for (const auto layer: layers) {
            if (strcmp(validationLayer, layer.layerName) == 0) {
                return true;
            }
        }
    }

    return false;
}

void TriangleApp::pickPhysicalDevice() {
    uint32_t physicalDeviceCount{0};
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

    std::cout << "physical devices count: " << physicalDeviceCount << std::endl;
    if (physicalDeviceCount == 0) {
        throw std::runtime_error("Can not find a GPU that support Vulkan.");
    }

    for (const auto &device: physicalDevices) {
        if (isDeviceSuitable(device)) {
            physicalDevice = device;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to find a GPU that fulfill requirement.");
    }
}

bool TriangleApp::isDeviceSuitable(const VkPhysicalDevice &device) {
    VkPhysicalDeviceFeatures deviceFeatures;
    VkPhysicalDeviceProperties deviceProperties;

    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    if (deviceFeatures.shaderInt64 && findQueueFamily(device).isCompletion()) {
        return true;
    }

    return false;
}

QueueFamilyIndices TriangleApp::findQueueFamily(const VkPhysicalDevice& device) {
    QueueFamilyIndices indices{};

    uint32_t queueFamilyCount{0};
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    uint32_t i{0};
    for (const auto &queueFamily: queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }
        if (indices.isCompletion()) {
            break;
        }
        i++;
    }

    return indices;
}

void TriangleApp::createLogicalDevice() {
    auto indices{findQueueFamily(physicalDevice)};

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;

    float queuePriority{1.0f};
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{
    };

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;
    std::vector<const char*> extensions = {
            "VK_KHR_portability_subset"
    };
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();
    if (enableValidation) {
        createInfo.enabledLayerCount = validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    auto result{vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice)};
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create logical device.");
    }

    vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &queue);
}


VkResult CreateDebugMessenger(VkInstance instance,
                          const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                          const VkAllocationCallbacks *pAllocator,
                          VkDebugUtilsMessengerEXT *debugMessenger) {
    auto func{(PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")};
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, debugMessenger);
    }
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugMessenger(VkInstance instance,
                           VkDebugUtilsMessengerEXT debugMessenger,
                           const VkAllocationCallbacks *pAllocator) {
    auto func{(PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT")};
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

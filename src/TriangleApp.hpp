//
// Created by Avon on 2022/11/17.
//

#ifndef VULKANTUR_TRIANGLEAPP_HPP
#define VULKANTUR_TRIANGLEAPP_HPP

#include <iostream>
#include <stdexcept>
#include <cstring>
#include <vector>
#include <optional>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

VkResult CreateDebugMessenger(VkInstance instance,
                             const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                             const VkAllocationCallbacks *pAllocator,
                             VkDebugUtilsMessengerEXT *debugMessenger);

void DestroyDebugMessenger(VkInstance instance,
                        VkDebugUtilsMessengerEXT debugMessenger,
                        const VkAllocationCallbacks *pAllocator);


struct QueueFamilyIndices {
    std::optional<uint32_t > graphicsFamily;

    bool isCompletion() {
        return graphicsFamily.has_value();
    }
};

class TriangleApp{
public:
    void run();

private:
    const int WIDTH{800};
    const int HEIGHT{600};

    std::vector<const char*> validationLayers{
        "VK_LAYER_KHRONOS_validation"
    };

#ifdef NDEBUG
    bool enableValidation{false};
#else
    bool enableValidation{true};
#endif

    VkInstance instance;

    GLFWwindow *window;

    VkDebugUtilsMessengerEXT debugMessenger;

    VkPhysicalDevice physicalDevice{VK_NULL_HANDLE};

    VkDevice logicalDevice;

    VkQueue queue;

    void initWindow();

    void initVulkan();

    void mainloop();

    void cleanup();

    void createInstance();

    static void displaySupportExtensions();

    [[nodiscard]] std::vector<const char *> getRequiredExtensions() const;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageFlag,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                        void *pUserData);

    static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfoExt);

    void setupDebugMessenger();

    bool checkValidationLayersSupport();

    void pickPhysicalDevice();

    bool isDeviceSuitable(const VkPhysicalDevice &device);

    QueueFamilyIndices findQueueFamily(const VkPhysicalDevice& device);

    void createLogicalDevice();


};

#endif //VULKANTUR_TRIANGLEAPP_HPP

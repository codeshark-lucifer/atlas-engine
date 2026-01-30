#define VK_NO_PROTOTYPES
#define VOLK_IMPLEMENTATION
#define VK_USE_PLATFORM_WIN32_KHR

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <volk/volk.h>

#include <iostream>
#include <vector>
#include <optional>

const char* WINDOW_NAME = "atlas - engine";
const int WINDOW_WIDTH = 956;
const int WINDOW_HEIGHT = 540;

int main() {
    // ===== SDL Init =====
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Failed to initialize SDL3: " << SDL_GetError() << "\n";
        return EXIT_FAILURE;
    }

    // Load Vulkan through SDL + volk
    SDL_Vulkan_LoadLibrary(nullptr);
    volkInitializeCustom(reinterpret_cast<PFN_vkGetInstanceProcAddr>(
        SDL_Vulkan_GetVkGetInstanceProcAddr()
    ));

    // ===== Vulkan Instance =====
    VkApplicationInfo appInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = WINDOW_NAME,
        .applicationVersion = VK_MAKE_VERSION(1,0,0),
        .pEngineName = "atlas",
        .engineVersion = VK_MAKE_VERSION(1,0,0),
        .apiVersion = VK_API_VERSION_1_3
    };

    Uint32 extCount = 0;
    const char* const* sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&extCount);
    std::vector<const char*> extensions(sdlExtensions, sdlExtensions + extCount);

    VkInstanceCreateInfo instanceInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = extCount,
        .ppEnabledExtensionNames = extensions.data()
    };

    VkInstance instance;
    SDL_assert(!vkCreateInstance(&instanceInfo, nullptr, &instance));
    volkLoadInstance(instance);

    // ===== Physical Device =====
    uint32_t physicalDeviceCount = 0;
    SDL_assert(!vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr));
    SDL_assert(physicalDeviceCount > 0);
    std::vector<VkPhysicalDevice> devices(physicalDeviceCount);
    SDL_assert(!vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, devices.data()));
    VkPhysicalDevice physicalDevice = devices[0];

    // ===== SDL Window =====
    SDL_Window* window = SDL_CreateWindow(WINDOW_NAME, WINDOW_WIDTH, WINDOW_HEIGHT,
                                          SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
    SDL_assert(window);

    // ===== Vulkan Surface =====
    VkSurfaceKHR surface;
    SDL_assert(SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface));

    // ===== Queue Family =====
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) graphicsFamily = i;

        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
        if (presentSupport) presentFamily = i;

        if (graphicsFamily && presentFamily) break;
    }

    SDL_assert(graphicsFamily.has_value() && presentFamily.has_value());
    uint32_t queueFamilyIndex = graphicsFamily.value();

    // ===== Logical Device & Queue =====
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = queueFamilyIndex,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };

    const char* deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    VkDeviceCreateInfo deviceInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueCreateInfo,
        .enabledExtensionCount = 1,
        .ppEnabledExtensionNames = deviceExtensions
    };

    VkDevice device;
    SDL_assert(!vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &device));

    VkQueue graphicsQueue;
    vkGetDeviceQueue(device, queueFamilyIndex, 0, &graphicsQueue);

    // ===== Swapchain =====
    VkSurfaceCapabilitiesKHR surfaceCaps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCaps);

    VkExtent2D swapExtent = surfaceCaps.currentExtent.width != UINT32_MAX
                                ? surfaceCaps.currentExtent
                                : VkExtent2D{WINDOW_WIDTH, WINDOW_HEIGHT};

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    SDL_assert(formatCount > 0);
    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());

    VkSurfaceFormatKHR surfaceFormat = surfaceFormats[0];
    for (auto& f : surfaceFormats) if (f.format == VK_FORMAT_B8G8R8A8_SRGB) { surfaceFormat = f; break; }

    VkSwapchainCreateInfoKHR swapInfo{
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = surfaceCaps.minImageCount + 1,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = swapExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = surfaceCaps.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,
        .clipped = VK_TRUE
    };

    VkSwapchainKHR swapchain;
    SDL_assert(!vkCreateSwapchainKHR(device, &swapInfo, nullptr, &swapchain));

    uint32_t imageCount;
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
    std::vector<VkImage> swapImages(imageCount);
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapImages.data());

    // ===== Image Views =====
    std::vector<VkImageView> swapImageViews(imageCount);
    for (uint32_t i = 0; i < imageCount; ++i) {
        VkImageViewCreateInfo viewInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = surfaceFormat.format,
            .components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
                            VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY },
            .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
        };
        SDL_assert(!vkCreateImageView(device, &viewInfo, nullptr, &swapImageViews[i]));
    }

    // ===== Command Pool & Buffers =====
    VkCommandPool commandPool;
    VkCommandPoolCreateInfo poolInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queueFamilyIndex
    };
    SDL_assert(!vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool));

    std::vector<VkCommandBuffer> commandBuffers(imageCount);
    VkCommandBufferAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = imageCount
    };
    SDL_assert(!vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()));

    for (uint32_t i = 0; i < imageCount; ++i) {
        VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr, VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT};
        SDL_assert(!vkBeginCommandBuffer(commandBuffers[i], &beginInfo));

        VkClearValue clearColor{.color = {{0.1f, 0.2f, 0.3f, 1.0f}}};
        VkImageSubresourceRange range{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

        VkImageMemoryBarrier barrier{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_GENERAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = swapImages[i],
            .subresourceRange = range
        };

        vkCmdPipelineBarrier(commandBuffers[i],
                             VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             0, 0, nullptr, 0, nullptr, 1, &barrier);

        vkCmdClearColorImage(commandBuffers[i], swapImages[i], VK_IMAGE_LAYOUT_GENERAL, &clearColor.color, 1, &range);
        SDL_assert(!vkEndCommandBuffer(commandBuffers[i]));
    }

    // ===== Semaphores & Fence =====
    VkSemaphore imageAvailable, renderFinished;
    VkSemaphoreCreateInfo semInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    vkCreateSemaphore(device, &semInfo, nullptr, &imageAvailable);
    vkCreateSemaphore(device, &semInfo, nullptr, &renderFinished);

    VkFence fence;
    VkFenceCreateInfo fenceInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr, VK_FENCE_CREATE_SIGNALED_BIT};
    vkCreateFence(device, &fenceInfo, nullptr, &fence);

    // ===== Main Loop =====
    bool running = true;
    while (running) {
        for (SDL_Event event; SDL_PollEvent(&event);)
            if (event.type == SDL_EVENT_QUIT) running = false;

        uint32_t imageIndex;
        vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailable, VK_NULL_HANDLE, &imageIndex);

        VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
        VkSemaphore waitSemaphores[] = {imageAvailable};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
        VkSemaphore signalSemaphores[] = {renderFinished};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(device, 1, &fence);
        vkQueueSubmit(graphicsQueue, 1, &submitInfo, fence);
        vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);

        VkPresentInfoKHR presentInfo{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapchain;
        presentInfo.pImageIndices = &imageIndex;

        vkQueuePresentKHR(graphicsQueue, &presentInfo);
    }

    // ===== Cleanup =====
    for (auto view : swapImageViews) vkDestroyImageView(device, view, nullptr);
    vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
    vkDestroyCommandPool(device, commandPool, nullptr);
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

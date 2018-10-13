#pragma once

#include "common.h"

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#define NOMINMAX
#endif
#include "volk/volk.h"
#include "vk_enum_string_helper.h"

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#include "vk_mem_alloc.h"

#include "sdl/SDL_syswm.h"

#include <functional>
#include <string>
#include <vector>

#define VK_CHECK(function_call) { \
    VkResult result = function_call; \
    if (result < 0) \
        error(std::string("Vulkan: ") + string_VkResult(result) + " returned by " + #function_call); \
}

struct Vk_Pipeline_Def {
    VkShaderModule      vs_module;
    VkShaderModule      fs_module;
    VkRenderPass        render_pass;
    VkPipelineLayout    pipeline_layout;

    bool operator==(const Vk_Pipeline_Def& other) const {
        return vs_module == other.vs_module &&
               fs_module == other.fs_module &&
               render_pass == other.render_pass &&
               pipeline_layout == other.pipeline_layout;
    }
};

struct Vk_Create_Info {
    SDL_SysWMinfo               windowing_system_info;

    const VkDescriptorPoolSize* descriptor_pool_sizes;
    uint32_t                    descriptor_pool_size_count;
    uint32_t                    max_descriptor_sets;

    bool                        enable_validation_layers;
    bool                        use_debug_names;
};

struct Vk_Image {
    VkImage         handle;
    VkImageView     view;
    VmaAllocation   allocation;

    void destroy();
};

//
// Initialization.
//

// Initializes VK_Instance structure.
// After calling this function we get fully functional vulkan subsystem.
void vk_initialize(const Vk_Create_Info& create_info);

// Shutdown vulkan subsystem by releasing resources acquired by Vk_Instance.
void vk_shutdown();

void vk_release_resolution_dependent_resources();
void vk_restore_resolution_dependent_resources(bool vsync);

//
// Resources allocation.
//
void vk_ensure_staging_buffer_allocation(VkDeviceSize size);
VkBuffer vk_create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, const char* name);
VkBuffer vk_create_host_visible_buffer(VkDeviceSize size, VkBufferUsageFlags usage, void** buffer_ptr, const char* name);
Vk_Image vk_create_texture(int width, int height, VkFormat format, bool generate_mipmaps, const uint8_t* pixels, int bytes_per_pixel, const char*  name);
Vk_Image vk_create_image(int width, int height, VkFormat format, VkImageCreateFlags usage_flags, const char* name);
VkPipeline vk_find_pipeline(const Vk_Pipeline_Def& def);

//
// Rendering setup.
//
void vk_begin_frame();
void vk_end_frame();

void vk_record_and_run_commands(VkCommandPool command_pool, VkQueue queue, std::function<void(VkCommandBuffer)> recorder);

void vk_record_image_layout_transition(
    VkCommandBuffer command_buffer, VkImage image, VkImageAspectFlags aspect_mask,
    VkAccessFlags src_access_flags, VkAccessFlags dst_access_flags,
    VkImageLayout old_layout, VkImageLayout new_layout,
    uint32_t mip_level = VK_REMAINING_MIP_LEVELS
);

struct Swapchain_Info {
    VkSwapchainKHR           handle;
    std::vector<VkImage>     images;
    std::vector<VkImageView> image_views;
};

struct Depth_Buffer_Info {
    VkImage                 image;
    VkImageView             image_view;
    VmaAllocation           allocation;
    VkFormat                format;
};

// Vk_Instance contains vulkan resources that do not depend on applicaton logic.
// This structure is initialized/deinitialized by vk_initialize/vk_shutdown functions correspondingly.
struct Vk_Instance {
    Vk_Create_Info                  create_info;

    VkInstance                      instance;
    VkPhysicalDevice                physical_device;
    uint32_t                        queue_family_index;
    VkDevice                        device;
    VkQueue                         queue;

    VmaAllocator                    allocator;

    VkSurfaceKHR                    surface;
    VkSurfaceFormatKHR              surface_format;
    VkExtent2D                      surface_size;
    Swapchain_Info                  swapchain_info;

    uint32_t                        swapchain_image_index = -1; // current swapchain image

    VkCommandPool                   command_pool;
    VkCommandBuffer                 command_buffer;

    VkDescriptorPool                descriptor_pool;

    VkSemaphore                     image_acquired;
    VkSemaphore                     rendering_finished;
    VkFence                         rendering_finished_fence;

    // Host visible memory used to copy image data to device local memory.
    VkBuffer                        staging_buffer;
    VmaAllocation                   staging_buffer_allocation;
    VkDeviceSize                    staging_buffer_size;
    uint8_t*                        staging_buffer_ptr; // pointer to mapped staging buffer

    std::vector<Vk_Pipeline_Def>    pipeline_defs;
    std::vector<VkPipeline>         pipelines;

    Depth_Buffer_Info               depth_info;
    VkDebugUtilsMessengerEXT        debug_utils_messenger;
};

extern Vk_Instance vk;

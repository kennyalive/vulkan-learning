#pragma once

#include "matrix.h"
#include "vk.h"

#include "sdl/SDL_syswm.h"
#include <vector>

struct Uniform_Buffer;

struct Demo_Create_Info {
    Vk_Create_Info  vk_create_info;
    SDL_Window*     window;
};

struct Rasterization_Resources {
    VkDescriptorSetLayout       descriptor_set_layout;
    VkPipelineLayout            pipeline_layout;
    VkPipeline                  pipeline;
    VkDescriptorSet             descriptor_set;

    VkRenderPass                render_pass;
    VkFramebuffer               framebuffer;

    VkBuffer                    uniform_buffer;
    Uniform_Buffer*             mapped_uniform_buffer;

    Vk_Image                    texture;
    VkSampler                   sampler;

    void create(VkImageView output_image_view);
    void destroy();
    void create_framebuffer(VkImageView output_image_view);
    void destroy_framebuffer();
    void update(const Matrix3x4& model_transform, const Matrix3x4& view_transform);
};

struct Raytracing_Resources {
    uint32_t                    shader_header_size;

    VkAccelerationStructureNVX  bottom_level_accel;
    VmaAllocation               bottom_level_accel_allocation;

    VkAccelerationStructureNVX  top_level_accel;
    VmaAllocation               top_level_accel_allocation;

    VkDescriptorSetLayout       descriptor_set_layout;
    VkPipelineLayout            pipeline_layout;
    VkPipeline                  pipeline;
    VkDescriptorSet             descriptor_set;

    VkBuffer                    shader_binding_table;

    void create(VkImageView output_image_view, const VkGeometryTrianglesNVX& model_triangles);
    void destroy();
    void update_resolution_dependent_descriptor(VkImageView output_image_view);
};

struct Copy_To_Swapchain {
    VkDescriptorSetLayout           set_layout;
    VkPipelineLayout                pipeline_layout;
    VkPipeline                      pipeline;
    std::vector<VkDescriptorSet>    sets; // per swapchain image

    void create(VkImageView output_image_view);
    void destroy();
    void update_resolution_dependent_descriptors(VkImageView output_image_view);
};

class Vk_Demo {
public:
    void initialize(const Demo_Create_Info& create_info);
    void shutdown();

    void run_frame(bool draw_only_background = false);

    void release_resolution_dependent_resources();
    void restore_resolution_dependent_resources();

private:
    void create_ui_framebuffer();
    void destroy_ui_framebuffer();
    void create_output_image();

    void setup_imgui();
    void release_imgui();
    void do_imgui();

private:
    Demo_Create_Info            create_info;
    bool                        show_ui                 = true;
    bool                        vsync                   = true;
    bool                        animate                 = false;
    bool                        raytracing              = false;

    VkRenderPass                ui_render_pass;
    VkFramebuffer               ui_framebuffer;
    Vk_Image                    output_image;
    Copy_To_Swapchain           copy_to_swapchain;

    VkBuffer                    vertex_buffer;
    VkBuffer                    index_buffer;
    uint32_t                    model_vertex_count;
    uint32_t                    model_index_count;

    Matrix3x4                   model_transform;
    Matrix3x4                   view_transform;

    Rasterization_Resources     raster;
    Raytracing_Resources        rt;
};

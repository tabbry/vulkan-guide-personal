#pragma once

#include <vk_types.h>

/// <summary>
/// https://vkguide.dev/docs/new_chapter_2/vulkan_shader_code/
/// </summary>
struct DescriptorLayoutBuilder {

    std::vector<VkDescriptorSetLayoutBinding> bindings;

    void add_binding(uint32_t binding, VkDescriptorType type);
    void clear();
    VkDescriptorSetLayout build(VkDevice device, VkShaderStageFlags shaderStages, void* pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0);
};

/// <summary>
/// https://vkguide.dev/docs/new_chapter_2/vulkan_shader_code/
/// </summary>
struct DescriptorAllocator {
public:
    bool poolInitialized{ false };

    struct PoolSizeRatio {
        VkDescriptorType type;
        float ratio;
    };

    // Maintaining one big pool for the whole engine involves a lot of effort.
    // We try to have a pool for each part of the engine.
    VkDescriptorPool pool;

    /// <summary>
    /// https://vkguide.dev/docs/new_chapter_2/vulkan_shader_code/
    /// </summary>
    /// <param name="device"></param>
    /// <param name="maxSets">The number of descriptors that can be created in total</param>
    /// <param name="poolRatios">The ratios which define on how the poolsize should be distributed across the descriptor types</param>
    void init_pool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);

    /// <summary>
    /// This will reset all the descriptors of this pool and reset the pool.
    /// </summary>
    /// <param name="device"></param>
    void clear_descriptors(VkDevice device);

    void destroy_pool(VkDevice device);

    VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout);
    void allocate(VkDevice device, VkDescriptorSetLayout layout, uint32_t count, VkDescriptorSet* descriptorSets);
};
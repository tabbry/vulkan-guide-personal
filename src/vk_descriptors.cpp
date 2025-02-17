#include <vk_descriptors.h>

/// <summary>
/// https://vkguide.dev/docs/new_chapter_2/vulkan_shader_code/
/// </summary>
/// <param name="binding"></param>
/// <param name="type"></param>
void DescriptorLayoutBuilder::add_binding(uint32_t binding, VkDescriptorType type)
{
    VkDescriptorSetLayoutBinding newbind{};
    newbind.binding = binding;
    newbind.descriptorCount = 1;
    newbind.descriptorType = type;

    bindings.push_back(newbind);
}

/// <summary>
/// https://vkguide.dev/docs/new_chapter_2/vulkan_shader_code/
/// </summary>
void DescriptorLayoutBuilder::clear()
{
    bindings.clear();
}

/// <summary>
/// https://vkguide.dev/docs/new_chapter_2/vulkan_shader_code/
/// </summary>
/// <param name="device"></param>
/// <param name="shaderStages">The shader stages will be added to all bindings</param>
/// <param name="pNext"></param>
/// <param name="flags"></param>
/// <returns></returns>
VkDescriptorSetLayout DescriptorLayoutBuilder::build(VkDevice device, VkShaderStageFlags shaderStages, void* pNext, VkDescriptorSetLayoutCreateFlags flags)
{
    for (auto& b : bindings) {
        // Usually the bindings can have differing stage flags, however for our purpose we set the same flags for all bindings.
        b.stageFlags |= shaderStages;
    }

    VkDescriptorSetLayoutCreateInfo info = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    info.pNext = pNext;

    info.pBindings = bindings.data();
    info.bindingCount = (uint32_t)bindings.size();
    info.flags = flags;

    VkDescriptorSetLayout set;
    VK_CHECK(vkCreateDescriptorSetLayout(device, &info, nullptr, &set));

    return set;
}

void DescriptorAllocator::init_pool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios)
{
    assert(!poolInitialized);

    std::vector<VkDescriptorPoolSize> poolSizes;
    for (PoolSizeRatio ratio : poolRatios) {
        poolSizes.push_back(VkDescriptorPoolSize{
            .type = ratio.type,
            .descriptorCount = uint32_t(ratio.ratio * maxSets)
            });
    }

    VkDescriptorPoolCreateInfo pool_info = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
    pool_info.flags = 0;
    pool_info.maxSets = maxSets;
    pool_info.poolSizeCount = (uint32_t)poolSizes.size();
    pool_info.pPoolSizes = poolSizes.data();

    vkCreateDescriptorPool(device, &pool_info, nullptr, &pool);

    poolInitialized = true;
}

void DescriptorAllocator::clear_descriptors(VkDevice device)
{
    vkResetDescriptorPool(device, pool, 0);
}

void DescriptorAllocator::destroy_pool(VkDevice device)
{
    vkDestroyDescriptorPool(device, pool, nullptr);
}


void DescriptorAllocator::allocate(VkDevice device, VkDescriptorSetLayout layout, uint32_t count, VkDescriptorSet* descriptorSets)
{
    assert(poolInitialized);
    assert(descriptorSets != nullptr);

    VkDescriptorSetAllocateInfo allocInfo = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
    allocInfo.pNext = nullptr;
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = count;
    allocInfo.pSetLayouts = &layout;

    VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, descriptorSets));
}

VkDescriptorSet DescriptorAllocator::allocate(VkDevice device, VkDescriptorSetLayout layout)
{
    VkDescriptorSet ds;
    allocate(device, layout, 1, &ds);
    return ds;
}

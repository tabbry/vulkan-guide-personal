This is just the repo for the personal learning progress of the [vulkan-guide](https://vkguide.dev/).

# Branching

- "working chapters": `tabbry/chapter-N` (branched from `starting-point` -> `tabbry/starting-point`)
- "solution": `all-chapters-2`

# Prerequisites Visual Studio 2022

When opening the rego, VS will ask to install all missing components.

# Project Setup

[Link](https://vkguide.dev/docs/new_chapter_0/building_project/)

## Installations

1. Clone Repo
2. Install [Vulkan SDK ](https://vulkan.lunarg.com/sdk/home) (restart may be required after installation)
3. Install [CMake](https://cmake.org/download/)

### Optional installations

- GLSL language Integration (VS Extension)

## Building

1. In Solution Exlporer select item `CMake Targets View`
2. Select any project and choose `Set as Startup Item`

# Personal Notes

## Introduction

### Vulkan API

https://vkguide.dev/docs/introduction/vulkan_overview/

#### Validation Layers

- Validation Layers reduce performance.
- Not good indicator, for it works on all hardware.
- Good for catching errors: incorrect configurations, wrong enums, sync issues, object lifetimes.
- Does not catch bugs like unitizialized memory or bad pointers.

#### Usage and general mindset

- Objects like pipelines are expensive to create (better create offline, than per-frame).
- Some objects like `DescriptorSets` are cheaper.
- All commands are submitted to queue using a `CommandBuffer`.
- Different queues can run in parallel.
- Vulkan has no concept of "frames", it could run in headless-mode.

### Vulkan Usage

https://vkguide.dev/docs/introduction/vulkan_execution/

#### Vulkan main objects and their use

Following concepts are summarized in the previous link:

- `VkInstance` (Vulkan context), 
- `VkPhysicalDevice` (Physical GPU), 
- `VkDevice` (Logical GPU), 
- `VkBuffer` (GPU Memory), 
- `VkImage` (Texture), 
- `VkPipeline` (GPU states)
- `VkRenderPass` (holds Target Image Information, legacy vkguide)
- `VkFrameBuffer` (target image, for render pass. legacy vkguide)
- `VkQueue` (could be graphics, memory, commands)
- `VkDescriptorSet` (GPU-side pointers)
- `VkSwapchainKHR`  (swapchain for the screen (extension: KHR))
- `VkSemaphore` (sync GPU to CPU)
- `VkFence` (sync GPU to CPU, has GPU command finished?)

#### High level Vulkan application flow

##### Engine initialization

1. create `VkInstance`
1. query available `VkPhysicalDevice`
1. assess properties and select suitable `VkPhysicalDevice`
1. create `VkDevice` from selected `VkPhysicalDevice`
1. get `VkQueue` handles from `VkDevice`
1. create `VkCommandPool` objects to use `VkQueue` handles
1. init `VkSwapchainKHR`

##### Asset initialization

- load materials -> `VkPipeline` objects
- meshes -> vertex data to `VkBuffer` objects
- textures -> `VkImage` (readable layout)
- for every pass (main, shadow, depth) -> `VkRenderPass`

##### Render Loop

1. ask `VkSwapchainKHR` for image to render to.
1. create or reuse `VkCommandBuffer` from `VkCommandBufferPool` and start it.
1. begin rendering by starting `VkRenderPass` to image received from `VkSwapchainKHR`.
1. in a loop bind `VkPipeline`, some `VkDescriptorSet`s, vertex buffer and execute a draw call.
1. end `VkRenderPass`. end `VkCommandBuffer` after everything is rendered.
1. use a semaphore to wait until rendering is finished, before presenting to the screen.

The link of this chapter contains pseude-code of the basic render loop.

### Project layout and Libraries

https://vkguide.dev/docs/introduction/project_libs/

#### Project layout

The chapter explains the folder structure of the project.

#### Libraries

- stored in `/third_party`
- SDL is not vendored

Summary of used libraries:

- **GLM**: mathematics, directly compatible with OpenGL and Vulkan
- **SDL**: windowing and inputs: seperatily built, crossplatform
- **dear IMGUI**: GUI library
- **STB Image**: image loading
- **Tiny Obj loader**: loads .obj 3d models
- **Vk Bootstrap**: Vulkan boilerplate abstraction
- **VMA**: vulkan memory allocator

## 0. Project Setup

### Building Project

My notes for this chapter are compiled in an earlier chapter (Project Setup) of this readme.

### Walkthrough

https://vkguide.dev/docs/new_chapter_0/code_walkthrough/

- The site contains short summaries about the .h/cpp files of the project.
- `vk_engine` is the core of the project.
- `vk_main` starting point. starts the engine. could be used to set parameters.
- `VK_CHECK` is a preprocessor macro for printing `VkResult` errors (defined in `vk_types.h`).
- `{fmt} lib` is used instead of `std::cout`
- Parameters for SDL are set in `vk_engine`
- SDL is a C library; SDL objects have to be deleted manually (`VulkanEngine::cleanup`).
- `VulkanEngine::run` in a nutshell so far: call `draw()` until the loop is stopped and not while it is suspended.

## 1. Initializing Vulkan

### Vulkan Initialization

- Vulkan has a lot to setup, therefore we use `VkBootstrap`
- Vulkan has no global state, `VkDevice`/`VkInstance` will be called in every call.

#### `VkInstance`

- enable validation layers
- set instance extensions

#### `VkPhysicalDevice`

- query GPU properties
- select GPU for the purpose

#### `VkDevice`

- enable only extensions which are needed.

#### Swapchain

- used for online-rendering
- swapchains are not part of Vulkan core specs
- recreate swapchain when window resizes
- swapchain holds images and views, accessible by the OS
- use 2/3 images for double/triple-buffering
- select `VkPresentModeKHR` based on needs (latency, vsync, ...)

### Vulkan Initialization Code

https://vkguide.dev/docs/new_chapter_1/vulkan_init_code/

- include `#include "VkBootstrap.h"` in `vk_engine.cpp`.
- The site links a tutorial in case someone wants to know in detail what bootstrap does.
- add some properties and methods to `vk_engine.h` to store instance, device and debug utils.

followed remaining steps...

- After implementing `VulkanEngine::init_vulkan()` the guide asks to start the app to confirm that the selected features are supported. In my case it did!

#### Cleaning up resources

- `VkPhysicalDevice` cannot be destroyed.
- After updating the `VulkanEngine::cleanup()` method, the guide says that running the app should not produce any errors. As expected, it didn't!


#### Validation layer errors

- For demonstration purposes the guide asks to use a wrong order in the cleanup function to provoke an error when closing the app. It did!

### Setting up Vulkan commands

https://vkguide.dev/docs/new_chapter_1/vulkan_commands_code/

- a lot of `VkCreateXXX` functions need the `sType` and `pNext` set.
- Init vulkan structs with ` = {};`, which zeroes all values of the struct. (in a way defaulting).

### Rendering Loop

https://vkguide.dev/docs/new_chapter_1/vulkan_mainloop/

#### Synchronization

- Commands once sent to GPU, will be executed in any order, unless specifically controlled with fences and semaphores.

##### `VkFence`

- Used for GPU to CPU communication
- `vkQueueSubmit` allows for *optional* fence parameter.
- `VkWaitForFences` will let the CPU wait until the commands are finished.
- for each `FrameData` a fence will be used. 

##### `VkSemaphore`

- Used for controlling order of executed GPU commands.
- semaphores can be *signaled* and *waited* upon.

The guide gives an example on how to force the GPU to run 3 commands linearly.

#### Render Loop

- for drawing an image to the screen, a image draw request has to be made to the OS.

#### Image Layouts

- image layout is the vulkan abstraction of GPU image formats.
- pipeline barriers also do transitions of image layouts.
- image layout transitions are very vendor specific. validation layers should be used!
- before `VkCmdDraw` can be called on an image from the swapchain, it has to be transitioned into a writeable image.
- For screen output yet another transition is necessary.
- For Vulkan 1.3 using dynamic rendering, those transitions have to be made manually.

### Mainloop Code

- The guide asks to extend `FrameData` with semaphores and a fence.
- The guide gives a summary of the inserted semaphores and the fence.
- short summary of `vkinit::fence_create_info` and `vkinit::semaphore_create_info`
- code and explanation of `VulkanEngine::init_sync_structures`

#### Draw loop

- `vkWaitForFences` with a timeout of `0` (nano-seconds) can be used to check if a fence still is used.
- The guide links to a Khronos resource about use cases of memory barriers as the presented approach is very generic, but also inefficient.
- `VK_IMAGE_LAYOUT_GENERAL` is not very efficient but generic. Also good when writing from a compute shader.
- The guide links to more resources about image layouts.
- later the draw command clears the color which slowly oscilates through time.
- later the guide gives introduction about the `submit_info` structs and commands.
- then the semaphores and the fence are prepared in the submit info for the command buffer.
- then the swapchain is used to present the result to the screen.
- finally destroying the fence and the semaphores is made part of the clenaup
- the guide asks to start the app and confirm that the window is changing color as expecetd and that no validation errors accur. Worked as expected!

## Drawing with Compute

### Improving the render loop

https://vkguide.dev/docs/new_chapter_2/vulkan_new_rendering/

#### Deletion queue

- we don't want to delete all the stuff in the cleanup, therefore we keep everything organized in a deletion queue.
- The guide shows code for defining the `DeletionQueue` struct.
- `FrameData` and `VulkanEngine` receive a deletion queue, which is used in `draw` and `cleanup`.

#### Memory Allocation

- VMA removes a lot of boilerplate.
- `VmaAllocator` is added to `VulkanEngine` and initialized in `init_vulkan`.

#### New draw loop

- The guide lists few reasons, why rendering directly to the swapchain is not recommended.
- instead rendering is done to an image with full format control and the copied to the swapchain.

#### Vulkan Images

- creating `AllocatedImage`
- The guide explains the "upload heap" and why to use `GPU_ONLY` for the draw image.

#### New draw loop

- There are two ways for copying images `VkCmdCopyImage` or `VkCmdBlitImage`.
- `VkCmdCopyImage` is faster, but restricted to format and dimensions.
- `VkCmdBlitImage` slower, but less restrictiond on format and dimensions.

### Vulkan Shaders

https://vkguide.dev/docs/new_chapter_2/vulkan_shader_drawing/

#### `VkShaderModule`

- in Vulkan shaders have to be pre-compiled into a *SpirV* file.
- CMake is configured to do this automatically

#### Descriptor Sets

- Data is given to shaders with `VkDescriptorSet` references.
- They are allocated in `VkDescriptorPool`.
- Data is updated calling `vkUpdateDescriptorSets`.
- Before it is available, it first has to be bound `VkBindDescriptorSets`

#### Push constants

- a few bytes of information can be pushed directly together with the graphics command.
- It's good for per-object data

#### The Compute Pipeline

1. create a compute pipeline layout
1. hook single shader module
1. execute by calling `VkCmdBindPipeline` and then `VkCmdDispatch`

- `VkCmdDispatch` expects a number of work groups in a X\*Y\*Z fashion
- In the shader the variables `gl_LocalInvocationID`, `gl_GlobalIncovationID` and `gl_WorkGroupID` give identity information about the process and can be used to identify the pixel.

### Vulkan Shaders - Code

https://vkguide.dev/docs/new_chapter_2/vulkan_shader_code/

#### Setting up the descriptor layout

- struct `DescriptorLayoutBuilder` is defined in `vk_descriptor.h/cpp`
- For the compute shader in question, the binding `0` is used (as in the shader defined) and the type will be `VK_DESCRIPTOR_TYPE_STORAGE_IMAGE`

#### Descriptor Allocator

- resetting `VkDescriptorPool`s is a rather quick process.

#### Initializing the layout and descriptors

### Setting up IMGUI

https://vkguide.dev/docs/new_chapter_2/vulkan_imgui_setup/

#### Immediate GPU commands

- imgui requires some commands outside of the normal draw loop.
- "Immediate Submit" structures are created.
- The `immediate_submit` executed a `VkCommandBuffer` command immediately.

#### IMGUI Setup

### Push Constants and new shaders

## 3. Graphics Pipelines

### The graphics pipeline

https://vkguide.dev/docs/new_chapter_3/render_pipeline/

#### The render pipeline

Simplified render pipeline

```
Data -> Vertex Shader -> Rasterization -> Fragment Shader -> Render Output
```

- **Fragment Shader** and **Render Output** are **fixed** stages
- **Data**: all inputs: vertices, textures, material parameters...
- **Vertex Shader**: transform vertices for rasterizer
- **Fragment Shader**: calculate color for all pixels

### Setting up render pipeline

https://vkguide.dev/docs/new_chapter_3/building_pipeline/

#### `VkGraphicsPipelineCreateInfo`

- The guide links to page with detailed info about [`VkGraphicsPipelineCreateInfo`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap10.html#VkGraphicsPipelineCreateInfo)

- `VkPipelineVertexInputStateCreateInfo`: config vertex info. Vertex pulling possible!
- `VkPipelineInputAssemblyStateCreateInfo`: what type of geometry is drawn.
- `VkPipelineTessellationStateCreateInfo`: fixed tesselation, ignore for now.
- `VkPipelineViewportStateCreateInfo`: viewport info for the pipeline, ignore for dynamic.
- `VkPipelineRasterizationStateCreateInfo`: depth bias, wireframe/solid, draw/skip backfaces
- `VkPipelineMultisampleStateCreateInfo`: Anti-Aliasing, skipped for now.
- `VkPipelineDepthStencilStateCreateInfo`: depth testing and stencil
- `VkPipelineColorBlendStateCreateInfo`: color blending = transparency.
- `VkPipelineDynamicStateCreateInfo`: 
	- pipeline configs are usually hardcoded, every config needs different pipeline
	- building pipelines is expensive
	- some states can be set to dynamic, and can be modified with commands
	- which states are dynamic depends on the device

### Mesh Buffers

- loading vertices from big gpu buffers gives performance and flexibility

#### Vulkan Buffers

- buffers provide general purpose memory, which don't need samplers.
- **Uniform Buffers (UBO)**:
	- read-only
	- small size (min 16 kB)
	- preload possible
	- good for material and global scene config
- **Storage Buffers (SSBO)**:
	- fully generic read-write buffers 
	- big size (min 128MB up to 4Gb)
	- in general can be used for anything.

The guide links a [benchmark](https://github.com/sebbbi/perftest) of accessing buffers.

- Descriptors can be used as dynamic buffer.
- A Descriptor can be used for multiple draw calls.
- This works well with uniform buffers.
- For Storage buffers, device-address is recommended.

#### Buffer Device Adress

- Accessing Buffer by the Buffer Device Address is possible with Vulkan 1.3
- Accessing a SSBO using the Buffer Device Address is faster than with Descriptor Sets.
- Pointer math can be applied to Buffer Device Addresses

#### Creating Buffers

- a `Span` is a pointer plus size which can easily be converted from C-style arrays or `std::vector` without having to perform a copy.

#### Drawing a Mesh

### Mesh Loading

- `std::optional<>` allows a value to be errored/null.

### Blending

https://vkguide.dev/docs/new_chapter_3/blending/

#### Blending in the pipeline

- in context of blending, source is the pixel to render and destination is the current pixel value of its position.

### Window Resizing

https://vkguide.dev/docs/new_chapter_3/resizing_window/

- instead of reallocating all the images, the guide recommends to adjust the sacling of the rendering.

# Open Questions

- What is DynamicRendering? Why would I want to skip renderpasses/framebuffers?
- What is new with `features.synchronization2`?
- What is the benefit of GPU pointers without binding buffers?
- What are bindless textures?
- What is a dynamic buffer?
- What is a forward declaration?
- Does it make sense that all `VkDescriptorSetLayoutBinding` instances of the same `VkDescriptorSetLayout` have the same `stageFlags`?
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

## Setting up Vulkan commands

https://vkguide.dev/docs/new_chapter_1/vulkan_commands_code/

- a lot of `VkCreateXXX` functions need the `sType` and `pNext` set.
- Init vulkan structs with ` = {};`, which zeroes all values of the struct. (in a way defaulting).

# Open Questions

- What is DynamicRendering? Why would I want to skip renderpasses/framebuffers?
- What is new with `features.synchronization2`?
- What is the benefit of GPU pointers without binding buffers?
- What are bindless textures?
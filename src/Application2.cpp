#include "Application.h"
#include "webgpu-utils.h"
#include "createFvertices.h"

#include <dawn/webgpu.h>
#ifdef WEBGPU_BACKEND_WGPU
#  include <webgpu/wgpu.h>
#endif // WEBGPU_BACKEND_WGPU

#include <GLFW/glfw3.h>
#include <glfw3webgpu.h>

#ifdef __EMSCRIPTEN__
#  include <emscripten.h>
#endif // __EMSCRIPTEN__

#include <iostream>
#include <cassert>
#include <vector>

#include <fstream>
#include <sstream>
#include <stdexcept>

// Function to print GLFW errors
void glfwErrorCallback(int error, const char* description) {
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}

// Callback functions
void Application::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        std::cout << "Key Pressed: " << key << std::endl;
    } else if (action == GLFW_RELEASE) {
        std::cout << "Key Released: " << key << std::endl;
    }
}

void Application::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        std::cout << "Mouse Button Pressed: " << button << std::endl;
    } else if (action == GLFW_RELEASE) {
        std::cout << "Mouse Button Released: " << button << std::endl;
    }
}

void Application::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    std::cout << "Mouse Position: (" << xpos << ", " << ypos << ")" << std::endl;
}

bool Application::Initialize() {
    depth = 4;
    asset_name = "monsterfrog";

    createFVertices(asset_name, depth);

    // Set GLFW error callback
    glfwSetErrorCallback(glfwErrorCallback);

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); // or GLFW_FALSE
    window = glfwCreateWindow(1280, 720, "Learn WebGPU", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);

    WGPUInstance instance = wgpuCreateInstance(nullptr);

    std::cout << "Requesting adapter..." << std::endl;
    surface = glfwGetWGPUSurface(instance, window);
    WGPURequestAdapterOptions adapterOpts = {};
    adapterOpts.nextInChain = nullptr;
    adapterOpts.compatibleSurface = surface;
    WGPUAdapter adapter = requestAdapterSync(instance, &adapterOpts);
    std::cout << "Got adapter: " << adapter << std::endl;

    wgpuInstanceRelease(instance);

    std::cout << "Requesting device..." << std::endl;
    WGPUDeviceDescriptor deviceDesc = {};
    deviceDesc.nextInChain = nullptr;
    deviceDesc.label = "My Device";
    deviceDesc.requiredFeatureCount = 0;
    deviceDesc.requiredLimits = nullptr;
    deviceDesc.defaultQueue.nextInChain = nullptr;
    deviceDesc.defaultQueue.label = "The default queue";
    deviceDesc.deviceLostCallback = [](WGPUDeviceLostReason reason, char const* message, void* /* pUserData */) {
        std::cout << "Device lost: reason " << reason;
        if (message) std::cout << " (" << message << ")";
        std::cout << std::endl;
    };
    device = requestDeviceSync(adapter, &deviceDesc);
    std::cout << "Got device: " << device << std::endl;

    auto onDeviceError = [](WGPUErrorType type, char const* message, void* /* pUserData */) {
        std::cout << "Uncaptured device error: type " << type;
        if (message) std::cout << " (" << message << ")";
        std::cout << std::endl;
    };
    wgpuDeviceSetUncapturedErrorCallback(device, onDeviceError, nullptr /* pUserData */);

    queue = wgpuDeviceGetQueue(device);

    // Configure the surface
    WGPUSurfaceConfiguration config = {};
    config.nextInChain = nullptr;
    config.width = 1280;
    config.height = 720;
    config.usage = WGPUTextureUsage_RenderAttachment;
    surfaceFormat = wgpuSurfaceGetPreferredFormat(surface, adapter);
    config.format = surfaceFormat;
    config.viewFormatCount = 0;
    config.viewFormats = nullptr;
    config.device = device;
    config.presentMode = WGPUPresentMode_Fifo;
    config.alphaMode = WGPUCompositeAlphaMode_Auto;

    wgpuSurfaceConfigure(surface, &config);

    // Release the adapter only after it has been fully utilized
    wgpuAdapterRelease(adapter);

    InitializePipeline();
    InitializeBuffers();
    return true;
}

void Application::Terminate() {
//    wgpuBufferRelease(pointBuffer);
//    wgpuBufferRelease(indexBuffer);
//    wgpuRenderPipelineRelease(pipeline);
//    wgpuSurfaceUnconfigure(surface);
//    wgpuQueueRelease(queue);
//    wgpuSurfaceRelease(surface);
//    wgpuDeviceRelease(device);
//    glfwDestroyWindow(window);
//    glfwTerminate();

    for (auto& buffer : buffers.connectivity) {
        wgpuBufferRelease(buffer);
    }
    for (auto& buffer : buffers.vertexBuffers) {
        wgpuBufferRelease(buffer);
    }
    for (auto& buffer : buffers.indexBuffers) {
        wgpuBufferRelease(buffer);
    }
    for (auto& buffer : buffers.colorStorageBuffer) {
        wgpuBufferRelease(buffer);
    }
    wgpuBufferRelease(buffers.base_index);
    wgpuBufferRelease(buffers.uniformBuffer);
    wgpuBufferRelease(buffers.resolveBuffer);
    wgpuBufferRelease(buffers.resultBuffer);
    wgpuBindGroupRelease(bindGroups.bindGroup_Face);
    wgpuBindGroupRelease(bindGroups.bindGroup_Edge);
    wgpuBindGroupRelease(bindGroups.bindGroup_Vertex);
    for (auto& bindGroup : bindGroups.changedBindGroups) {
        wgpuBindGroupRelease(bindGroup);
    }
    for (auto& bindGroup : bindGroups.fixedBindGroups) {
        wgpuBindGroupRelease(bindGroup);
    }
    wgpuBindGroupRelease(bindGroups.OrdinaryPointfixedBindGroup);
    wgpuBindGroupRelease(bindGroups.animeBindGroup);
    wgpuDeviceRelease(device);
    wgpuSurfaceRelease(surface);
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::MainLoop() {
//    glfwPollEvents();
//
//    // Get the next target texture view
//    WGPUTextureView targetView = GetNextSurfaceTextureView();
//    if (!targetView) return;
//
//    // Create a command encoder for the draw call
//    WGPUCommandEncoderDescriptor encoderDesc = {};
//    encoderDesc.nextInChain = nullptr;
//    encoderDesc.label = "My command encoder";
//    WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, &encoderDesc);
//
//    // Create the render pass that clears the screen with our color
//    WGPURenderPassDescriptor renderPassDesc = {};
//    renderPassDesc.nextInChain = nullptr;
//
//    // The attachment part of the render pass descriptor describes the target texture of the pass
//    WGPURenderPassColorAttachment renderPassColorAttachment = {};
//    renderPassColorAttachment.view = targetView;
//    renderPassColorAttachment.resolveTarget = nullptr;
//    renderPassColorAttachment.loadOp = WGPULoadOp_Clear;
//    renderPassColorAttachment.storeOp = WGPUStoreOp_Store;
//    renderPassColorAttachment.clearValue = WGPUColor{ 0.05, 0.05, 0.05, 1.0 };
//#ifndef WEBGPU_BACKEND_WGPU
//    renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
//#endif // NOT WEBGPU_BACKEND_WGPU
//
//    renderPassDesc.colorAttachmentCount = 1;
//    renderPassDesc.colorAttachments = &renderPassColorAttachment;
//    renderPassDesc.depthStencilAttachment = nullptr;
//    renderPassDesc.timestampWrites = nullptr;
//
//    WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);
//
//    // Select which render pipeline to use
//    wgpuRenderPassEncoderSetPipeline(renderPass, pipeline);
//
//    // Set vertex buffer while encoding the render pass
//    wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, pointBuffer, 0, wgpuBufferGetSize(pointBuffer));
//
//    // The second argument must correspond to the choice of uint16_t or uint32_t
//    // we've done when creating the index buffer.
//    wgpuRenderPassEncoderSetIndexBuffer(renderPass, indexBuffer, WGPUIndexFormat_Uint16, 0, wgpuBufferGetSize(indexBuffer));
//
//    // Replace `draw()` with `drawIndexed()` and `vertexCount` with `indexCount`
//    // The extra argument is an offset within the index buffer.
//    wgpuRenderPassEncoderDrawIndexed(renderPass, indexCount, 1, 0, 0, 0);
//
//    wgpuRenderPassEncoderEnd(renderPass);
//    wgpuRenderPassEncoderRelease(renderPass);
//
//    // Encode and submit the render pass
//    WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
//    cmdBufferDescriptor.nextInChain = nullptr;
//    cmdBufferDescriptor.label = "Command buffer";
//    WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);
//    wgpuCommandEncoderRelease(encoder);
//
////    std::cout << "Submitting command..." << std::endl;
//    wgpuQueueSubmit(queue, 1, &command);
//    wgpuCommandBufferRelease(command);
////    std::cout << "Command submitted." << std::endl;
//
//    // At the end of the frame
//    wgpuTextureViewRelease(targetView);
//#ifndef __EMSCRIPTEN__
//    wgpuSurfacePresent(surface);
//#endif
//
//#if defined(WEBGPU_BACKEND_DAWN)
//    wgpuDeviceTick(device);
//#elif defined(WEBGPU_BACKEND_WGPU)
//    wgpuDevicePoll(device, false, nullptr);
//#endif
}

bool Application::IsRunning() {
    return !glfwWindowShouldClose(window);
}

WGPUTextureView Application::GetNextSurfaceTextureView() {
    // Get the surface texture
    WGPUSurfaceTexture surfaceTexture;
    wgpuSurfaceGetCurrentTexture(surface, &surfaceTexture);
    if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_Success) {
        return nullptr;
    }

    // Create a view for this surface texture
    WGPUTextureViewDescriptor viewDescriptor;
    viewDescriptor.nextInChain = nullptr;
    viewDescriptor.label = "Surface texture view";
    viewDescriptor.format = wgpuTextureGetFormat(surfaceTexture.texture);
    viewDescriptor.dimension = WGPUTextureViewDimension_2D;
    viewDescriptor.baseMipLevel = 0;
    viewDescriptor.mipLevelCount = 1;
    viewDescriptor.baseArrayLayer = 0;
    viewDescriptor.arrayLayerCount = 1;
    viewDescriptor.aspect = WGPUTextureAspect_All;
    WGPUTextureView targetView = wgpuTextureCreateView(surfaceTexture.texture, &viewDescriptor);

    return targetView;
}

void Application::InitializePipeline() {
    // Load the shader module
    WGPUShaderModuleDescriptor shaderDesc{};
#ifdef WEBGPU_BACKEND_WGPU
    shaderDesc.hintCount = 0;
	shaderDesc.hints = nullptr;
#endif

    // We use the extension mechanism to specify the WGSL part of the shader module descriptor
    WGPUShaderModuleWGSLDescriptor shaderCodeDesc{};
    // Set the chained struct's header
    shaderCodeDesc.chain.next = nullptr;
    shaderCodeDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
    // Connect the chain
    shaderDesc.nextInChain = &shaderCodeDesc.chain;
    // Create shader modules for compute pipelines
    shaderCodeDesc.code = readFile("./shaders/face.wgsl").c_str();
    WGPUShaderModule faceModule = wgpuDeviceCreateShaderModule(device, &shaderDesc);

    shaderCodeDesc.code = readFile("./shaders/edge.wgsl").c_str();
    WGPUShaderModule edgeModule = wgpuDeviceCreateShaderModule(device, &shaderDesc);

    shaderCodeDesc.code = readFile("./shaders/vertex.wgsl").c_str();
    WGPUShaderModule vertexModule = wgpuDeviceCreateShaderModule(device, &shaderDesc);

    shaderCodeDesc.code = readFile("./shaders/patch.wgsl").c_str();
    WGPUShaderModule module1 = wgpuDeviceCreateShaderModule(device, &shaderDesc);

    shaderCodeDesc.code = readFile("./shaders/vertex.wgsl").c_str();
    WGPUShaderModule module2 = wgpuDeviceCreateShaderModule(device, &shaderDesc);

    shaderCodeDesc.code = readFile("./shaders/anime.wgsl").c_str();
    WGPUShaderModule animeModule = wgpuDeviceCreateShaderModule(device, &shaderDesc);

    shaderCodeDesc.code = readFile("./shaders/xyz.wgsl").c_str();
    WGPUShaderModule xyzModule = wgpuDeviceCreateShaderModule(device, &shaderDesc);

    // Create compute pipelines
    WGPUComputePipelineDescriptor computePipelineDesc{};
    computePipelineDesc.layout = nullptr;

    computePipelineDesc.compute.module = faceModule;
    computePipelineDesc.compute.entryPoint = "compute_FacePoint";
    WGPUComputePipeline pipelineFace = wgpuDeviceCreateComputePipeline(device, &computePipelineDesc);

    computePipelineDesc.compute.module = edgeModule;
    computePipelineDesc.compute.entryPoint = "compute_EdgePoint";
    WGPUComputePipeline pipelineEdge = wgpuDeviceCreateComputePipeline(device, &computePipelineDesc);

    computePipelineDesc.compute.module = vertexModule;
    computePipelineDesc.compute.entryPoint = "compute_VertexPoint";
    WGPUComputePipeline pipelineVertex = wgpuDeviceCreateComputePipeline(device, &computePipelineDesc);

    computePipelineDesc.compute.module = animeModule;
    computePipelineDesc.compute.entryPoint = "cs";
    WGPUComputePipeline pipelineAnime = wgpuDeviceCreateComputePipeline(device, &computePipelineDesc);

    // Create render pipelines
    WGPURenderPipelineDescriptor renderPipelineDesc{};
    renderPipelineDesc.layout = nullptr;

    // Configure the vertex pipeline
    WGPUVertexBufferLayout vertexBufferLayout{};
    vertexBufferLayout.arrayStride = 2 * sizeof(float);
    vertexBufferLayout.stepMode = WGPUVertexStepMode_Vertex;

    std::vector<WGPUVertexAttribute> vertexAttribs(1);
    vertexAttribs[0].shaderLocation = 0;
    vertexAttribs[0].format = WGPUVertexFormat_Float32x2;
    vertexAttribs[0].offset = 0;

    vertexBufferLayout.attributeCount = static_cast<uint32_t>(vertexAttribs.size());
    vertexBufferLayout.attributes = vertexAttribs.data();

    renderPipelineDesc.vertex.bufferCount = 1;
    renderPipelineDesc.vertex.buffers = &vertexBufferLayout;
    renderPipelineDesc.vertex.module = module1;
    renderPipelineDesc.vertex.entryPoint = "vs";
    renderPipelineDesc.vertex.constantCount = 0;
    renderPipelineDesc.vertex.constants = nullptr;

    // Fragment state
    WGPUFragmentState fragmentState{};
    fragmentState.module = module1;
    fragmentState.entryPoint = "fs";
    fragmentState.constantCount = 0;
    fragmentState.constants = nullptr;

    WGPUBlendState blendState{};
    blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
    blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
    blendState.color.operation = WGPUBlendOperation_Add;
    blendState.alpha.srcFactor = WGPUBlendFactor_Zero;
    blendState.alpha.dstFactor = WGPUBlendFactor_One;
    blendState.alpha.operation = WGPUBlendOperation_Add;

    WGPUColorTargetState colorTarget{};
    colorTarget.format = surfaceFormat;
    colorTarget.blend = &blendState;
    colorTarget.writeMask = WGPUColorWriteMask_All;

    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTarget;
    renderPipelineDesc.fragment = &fragmentState;

    // Primitive state
    renderPipelineDesc.primitive.topology = WGPUPrimitiveTopology_PointList;
    renderPipelineDesc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
    renderPipelineDesc.primitive.frontFace = WGPUFrontFace_CCW;
    renderPipelineDesc.primitive.cullMode = WGPUCullMode_None;

    // Depth stencil state
    WGPUDepthStencilState depthStencilState{};
    depthStencilState.depthWriteEnabled = true;
    depthStencilState.depthCompare = WGPUCompareFunction_Less;
    depthStencilState.format = WGPUTextureFormat_Depth24Plus;
    renderPipelineDesc.depthStencil = &depthStencilState;

    // Multisample state
    renderPipelineDesc.multisample.count = 1;
    renderPipelineDesc.multisample.mask = ~0u;
    renderPipelineDesc.multisample.alphaToCoverageEnabled = false;

    WGPURenderPipeline pipelinePointList = wgpuDeviceCreateRenderPipeline(device, &renderPipelineDesc);
    renderPipelineDesc.primitive.topology = WGPUPrimitiveTopology_LineList;
    WGPURenderPipeline pipelineLineList = wgpuDeviceCreateRenderPipeline(device, &renderPipelineDesc);
    renderPipelineDesc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    WGPURenderPipeline pipelineFaceList = wgpuDeviceCreateRenderPipeline(device, &renderPipelineDesc);

    std::vector<WGPURenderPipeline> pipelines = { pipelinePointList, pipelineLineList, pipelineFaceList };

    // Create second render pipeline
    renderPipelineDesc.vertex.bufferCount = 1;
    vertexBufferLayout.arrayStride = 4 * sizeof(float);
    vertexAttribs[0].shaderLocation = 0;
    vertexAttribs[0].format = WGPUVertexFormat_Float32x4;
    vertexAttribs[0].offset = 0;

    vertexBufferLayout.attributeCount = static_cast<uint32_t>(vertexAttribs.size());
    vertexBufferLayout.attributes = vertexAttribs.data();
    renderPipelineDesc.vertex.buffers = &vertexBufferLayout;

    renderPipelineDesc.vertex.module = module2;
    renderPipelineDesc.vertex.entryPoint = "vs";
    fragmentState.module = module2;
    fragmentState.entryPoint = "fs";
    renderPipelineDesc.primitive.topology = WGPUPrimitiveTopology_TriangleList;

    WGPURenderPipeline pipeline2 = wgpuDeviceCreateRenderPipeline(device, &renderPipelineDesc);

    // Create XYZ pipeline
    renderPipelineDesc.vertex.module = xyzModule;
    renderPipelineDesc.vertex.entryPoint = "vs";
    fragmentState.module = xyzModule;
    fragmentState.entryPoint = "fs";
    renderPipelineDesc.primitive.topology = WGPUPrimitiveTopology_LineList;

    WGPURenderPipeline xyzPipeline = wgpuDeviceCreateRenderPipeline(device, &renderPipelineDesc);

    // Cleanup shader modules
    wgpuShaderModuleRelease(faceModule);
    wgpuShaderModuleRelease(edgeModule);
    wgpuShaderModuleRelease(vertexModule);
    wgpuShaderModuleRelease(module1);
    wgpuShaderModuleRelease(module2);
    wgpuShaderModuleRelease(animeModule);
    wgpuShaderModuleRelease(xyzModule);
}

// If you do not use webgpu.hpp, I suggest you create a function to init the
// WGPULimits structure:
void setDefault(WGPULimits &limits) {
    limits.maxTextureDimension1D = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxTextureDimension2D = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxTextureDimension3D = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxTextureArrayLayers = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxBindGroups = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxBindGroupsPlusVertexBuffers = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxBindingsPerBindGroup = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxDynamicUniformBuffersPerPipelineLayout = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxDynamicStorageBuffersPerPipelineLayout = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxSampledTexturesPerShaderStage = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxSamplersPerShaderStage = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxStorageBuffersPerShaderStage = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxStorageTexturesPerShaderStage = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxUniformBuffersPerShaderStage = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxUniformBufferBindingSize = WGPU_LIMIT_U64_UNDEFINED;
    limits.maxStorageBufferBindingSize = WGPU_LIMIT_U64_UNDEFINED;
    limits.minUniformBufferOffsetAlignment = WGPU_LIMIT_U32_UNDEFINED;
    limits.minStorageBufferOffsetAlignment = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxVertexBuffers = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxBufferSize = WGPU_LIMIT_U64_UNDEFINED;
    limits.maxVertexAttributes = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxVertexBufferArrayStride = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxInterStageShaderComponents = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxInterStageShaderVariables = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxColorAttachments = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxColorAttachmentBytesPerSample = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxComputeWorkgroupStorageSize = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxComputeInvocationsPerWorkgroup = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxComputeWorkgroupSizeX = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxComputeWorkgroupSizeY = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxComputeWorkgroupSizeZ = WGPU_LIMIT_U32_UNDEFINED;
    limits.maxComputeWorkgroupsPerDimension = WGPU_LIMIT_U32_UNDEFINED;
}

WGPURequiredLimits Application::GetRequiredLimits(WGPUAdapter adapter) const {
    // Get adapter supported limits, in case we need them
    WGPUSupportedLimits supportedLimits;
    supportedLimits.nextInChain = nullptr;
    wgpuAdapterGetLimits(adapter, &supportedLimits);

    WGPURequiredLimits requiredLimits{};
    setDefault(requiredLimits.limits);

    // We use at most 2 vertex attributes
    requiredLimits.limits.maxVertexAttributes = 2;
    //                                          ^ This was 1
    // We should also tell that we use 1 vertex buffers
    requiredLimits.limits.maxVertexBuffers = 1;
    // Maximum size of a buffer is 6 vertices of 5 float each
    requiredLimits.limits.maxBufferSize = 6 * 5 * sizeof(float);
    //                                        ^ This was a 2
    // Maximum stride between 2 consecutive vertices in the vertex buffer
    requiredLimits.limits.maxVertexBufferArrayStride = 5 * sizeof(float);
    //                                                 ^ This was a 2

    // There is a maximum of 3 float forwarded from vertex to fragment shader
    requiredLimits.limits.maxInterStageShaderComponents = 3;

    // These two limits are different because they are "minimum" limits,
    // they are the only ones we are may forward from the adapter's supported
    // limits.
    requiredLimits.limits.minUniformBufferOffsetAlignment = supportedLimits.limits.minUniformBufferOffsetAlignment;
    requiredLimits.limits.minStorageBufferOffsetAlignment = supportedLimits.limits.minStorageBufferOffsetAlignment;

    return requiredLimits;
}

void Application::InitializeBuffers() {

    for (int i = 0; i <= depth; i++) {
        WGPUBufferDescriptor bufferDesc = {};
        bufferDesc.label = "storage buffer vertices";
        bufferDesc.size = connectivitys[i].size() * sizeof(uint32_t); // sizeof(uint32_t) 제거 가능성 있음.
        bufferDesc.usage = WGPUBufferUsage_Storage | WGPUBufferUsage_CopyDst;

        WGPUBuffer connectivityStorageBuffer = wgpuDeviceCreateBuffer(device, &bufferDesc);
        wgpuQueueWriteBuffer(queue, connectivityStorageBuffer, 0, connectivitys[i].data(), connectivitys[i].size());
        buffers.connectivity.push_back(connectivityStorageBuffer);
    }


    for (int i = 0; i <= depth; ++i) {
        makeBuffer(depth, i, buffers);
        buffers.texcoordDatas.push_back(buffers.texcoordDatas[i]);
        buffers.indexBuffers.push_back(buffers.indexBuffers[i]);
        buffers.indices.push_back(buffers.indices[i]);
        buffers.vertexBuffers.push_back(buffers.vertexBuffers[i]);
    }

    std::array<std::array<float, 8>, 8> colors = {
            std::array<float, 8>{0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},
            std::array<float, 8>{1.0f, 0.5f, 0.5f, 1.0f, 0.0001f, 0.0001f, 0.0001f, 0.0f},
            std::array<float, 8>{1.0f, 1.0f, 0.0f, 1.0f, 0.0002f, 0.0002f, 0.00002f, 0.0f},
            std::array<float, 8>{0.0f, 1.0f, 0.0f, 1.0f, 0.0003f, 0.0003f, 0.0003f, 0.0f},
            std::array<float, 8>{0.0f, 0.0f, 1.0f, 1.0f, 0.0004f, 0.0004f, 0.0004f, 0.0f},
            std::array<float, 8>{1.0f, 0.0f, 1.0f, 1.0f, 0.0005f, 0.0005f, 0.0005f, 0.0f},
            std::array<float, 8>{0.0f, 1.0f, 1.0f, 1.0f, 0.0006f, 0.0006f, 0.0006f, 0.0f},
            std::array<float, 8>{1.0f, 1.0f, 1.0f, 1.0f, 0.0007f, 0.0007f, 0.0007f, 0.0f}
    };

    buffers.colorStorageBuffer.reserve(depth + 1);

    for (int i = 0; i <= depth; ++i) {
        WGPUBufferDescriptor bufferDesc = {};
        bufferDesc.label = "storage buffer colors";
        bufferDesc.size = colors.size() * sizeof(colors[0]);
        bufferDesc.usage = WGPUBufferUsage_Storage | WGPUBufferUsage_CopyDst;

        WGPUBuffer colorStorageBuffer = wgpuDeviceCreateBuffer(device, &bufferDesc);
        wgpuQueueWriteBuffer(queue, colorStorageBuffer, 0, colors.data(), colors.size() * sizeof(colors[0]));
        buffers.colorStorageBuffer.push_back(colorStorageBuffer);
    }
}

void Application::makeBuffer(int depth, int patchLevel, Buffers &buffers) {
    for (int i = 0; i <= depth; ++i) {
        int N = std::max(static_cast<int>(std::pow(2, depth - i - patchLevel)), 1);
        std::vector<float> texcoordData((N + 1) * (N + 1) * 2);
        int offset = 0;
        for (int row = 0; row <= N; ++row) {
            for (int col = 0; col <= N; ++col) {
                texcoordData[offset++] = static_cast<float>(row) / N;
                texcoordData[offset++] = static_cast<float>(col) / N;
            }
        }
        buffers.texcoordDatas.push_back(texcoordData);

        std::vector<uint32_t> index(N * N * 6);
        offset = 0;
        for (int row = 0; row < N; ++row) {
            for (int col = 0; col < N; ++col) {
                index[offset++] = row + col * (N + 1);
                index[offset++] = row + (col + 1) * (N + 1);
                index[offset++] = row + col * (N + 1) + 1;
                index[offset++] = row + col * (N + 1) + 1;
                index[offset++] = row + (col + 1) * (N + 1);
                index[offset++] = (row + 1) + (col + 1) * (N + 1);
            }
        }
        buffers.indices.push_back(index);

        WGPUBufferDescriptor vertexBufferDesc{};
        vertexBufferDesc.nextInChain = nullptr;
        vertexBufferDesc.size = texcoordData.size() * sizeof(float);
        vertexBufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex;
        vertexBufferDesc.mappedAtCreation = false;
        WGPUBuffer vertexBuffer = wgpuDeviceCreateBuffer(device, &vertexBufferDesc);
        buffers.vertexBuffers.push_back(vertexBuffer);

        WGPUBufferDescriptor indexBufferDesc{};
        indexBufferDesc.nextInChain = nullptr;
        indexBufferDesc.size = index.size() * sizeof(uint32_t);
        indexBufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index;
        indexBufferDesc.mappedAtCreation = false;
        WGPUBuffer indexBuffer = wgpuDeviceCreateBuffer(device, &indexBufferDesc);
        buffers.indexBuffers.push_back(indexBuffer);
    }

    for (size_t i = 0; i < buffers.vertexBuffers.size(); ++i) {
        wgpuQueueWriteBuffer(wgpuDeviceGetQueue(device), buffers.vertexBuffers[i], 0, buffers.texcoordDatas[i].data(), buffers.texcoordDatas[i].size() * sizeof(float));
        wgpuQueueWriteBuffer(wgpuDeviceGetQueue(device), buffers.indexBuffers[i], 0, buffers.indices[i].data(), buffers.indices[i].size() * sizeof(uint32_t));
    }
}

void Application::InitializeBindGroup(){
    WGPUComputePipeline pipeline_Face;  // Initialize this with the appropriate pipeline
    WGPUComputePipeline pipeline_Edge;  // Initialize this with the appropriate pipeline
    WGPUComputePipeline pipeline_Vertex;  // Initialize this with the appropriate pipeline
    WGPUBuffer Base_Vertex_Buffer;  // Initialize this with the appropriate buffer

    // Assume buffers is already initialized properly
    BufferData buffers = createBufferData(/* appropriate parameters */);

    // Create bind groups for face, edge, and vertex data
    WGPUBindGroupEntry entries_Face[] = {
            {0, {buffers.vertex_Buffer_F}},
            {1, {buffers.offset_Buffer_F}},
            {2, {buffers.valance_Buffer_F}},
            {3, {buffers.pointIdx_Buffer_F}},
            {4, {Base_Vertex_Buffer}}
    };
    WGPUBindGroupDescriptor bindGroupDesc_Face = {};
    bindGroupDesc_Face.layout = wgpuComputePipelineGetBindGroupLayout(pipeline_Face, 0);
    bindGroupDesc_Face.entryCount = sizeof(entries_Face) / sizeof(entries_Face[0]);
    bindGroupDesc_Face.entries = entries_Face;
    WGPUBindGroup bindGroup_Face = wgpuDeviceCreateBindGroup(device, &bindGroupDesc_Face);

    WGPUBindGroupEntry entries_Edge[] = {
            {0, {buffers.vertex_Buffer_E}},
            {1, {buffers.pointIdx_Buffer_E}},
            {2, {Base_Vertex_Buffer}}
    };
    WGPUBindGroupDescriptor bindGroupDesc_Edge = {};
    bindGroupDesc_Edge.layout = wgpuComputePipelineGetBindGroupLayout(pipeline_Edge, 0);
    bindGroupDesc_Edge.entryCount = sizeof(entries_Edge) / sizeof(entries_Edge[0]);
    bindGroupDesc_Edge.entries = entries_Edge;
    WGPUBindGroup bindGroup_Edge = wgpuDeviceCreateBindGroup(device, &bindGroupDesc_Edge);

    WGPUBindGroupEntry entries_Vertex[] = {
            {0, {buffers.vertex_Buffer_V}},
            {1, {buffers.offset_Buffer_V}},
            {2, {buffers.valance_Buffer_V}},
            {3, {buffers.index_Buffer_V}},
            {4, {buffers.pointIdx_Buffer_V}},
            {5, {Base_Vertex_Buffer}}
    };
    WGPUBindGroupDescriptor bindGroupDesc_Vertex = {};
    bindGroupDesc_Vertex.layout = wgpuComputePipelineGetBindGroupLayout(pipeline_Vertex, 0);
    bindGroupDesc_Vertex.entryCount = sizeof(entries_Vertex) / sizeof(entries_Vertex[0]);
    bindGroupDesc_Vertex.entries = entries_Vertex;
    WGPUBindGroup bindGroup_Vertex = wgpuDeviceCreateBindGroup(device, &bindGroupDesc_Vertex);
}
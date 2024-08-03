#ifndef APPLICATION_H
#define APPLICATION_H

#include <webgpu/webgpu.h>
#include <GLFW/glfw3.h>

class Application {
public:
    bool Initialize();
    void Terminate();
    void MainLoop();
    bool IsRunning();

private:
    WGPUTextureView GetNextSurfaceTextureView();
    void InitializePipeline();
    WGPURequiredLimits GetRequiredLimits(WGPUAdapter adapter) const;
    void InitializeBuffers();

    GLFWwindow *window;
    WGPUDevice device;
    WGPUQueue queue;
    WGPUSurface surface;
    WGPUTextureFormat surfaceFormat = WGPUTextureFormat_Undefined;
    WGPURenderPipeline pipeline;
    WGPUBuffer pointBuffer;
    WGPUBuffer indexBuffer;
    uint32_t indexCount;
};

#endif // APPLICATION_H

#ifndef APPLICATION_H
#define APPLICATION_H

#include <dawn/webgpu.h>
#include <GLFW/glfw3.h>

class Application {
public:
    bool Initialize();
    void Terminate();
    void MainLoop();
    bool IsRunning();

private:
    void InitializePipeline();
    void InitializeBuffers();
    WGPUTextureView GetNextSurfaceTextureView();
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    WGPURequiredLimits GetRequiredLimits(WGPUAdapter adapter) const;

    GLFWwindow* window;
    WGPUSurface surface;
    WGPUDevice device;
    WGPUQueue queue;
    WGPURenderPipeline pipeline;
    WGPUBuffer pointBuffer;
    WGPUBuffer indexBuffer;
    WGPUTextureFormat surfaceFormat;
    uint32_t indexCount;
};

#endif // APPLICATION_H
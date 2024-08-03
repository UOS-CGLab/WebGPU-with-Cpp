#ifndef APPLICATION_H
#define APPLICATION_H

#include <dawn/webgpu.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

struct Buffers {
    std::vector<std::vector<float>> texcoordDatas;
    std::vector<std::vector<uint32_t>> indices;
    std::vector<WGPUBuffer> indexBuffers;
    std::vector<WGPUBuffer> vertexBuffers;
    std::vector<WGPUBuffer> connectivity;
    WGPUBuffer base_index;
    WGPUBuffer uniformBuffer;
    WGPUBuffer resolveBuffer;
    WGPUBuffer resultBuffer;
    std::vector<WGPUBuffer> colorStorageBuffer;
};

struct BindGroups {
    WGPUBindGroup bindGroup_Face;
    WGPUBindGroup bindGroup_Edge;
    WGPUBindGroup bindGroup_Vertex;
    WGPUBindGroup fixedBindGroups[3];
    WGPUBindGroup OrdinaryPointfixedBindGroup;
    WGPUBindGroup animeBindGroup;
    std::vector<WGPUBindGroup> changedBindGroups;
};

class Application {
public:
    bool Initialize();
    void Terminate();
    void MainLoop();
    bool IsRunning();

private:
    void InitializeBindGroup();
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

    int depth;
    std::string asset_name;

    Buffers buffers;
    void makeBuffer(int depth, int patchLevel, Buffers& buffers);

    std::vector<std::vector<uint32_t>> connectivitys;
    std::vector<std::vector<uint32_t>> OrdinaryPointData;

    BindGroups bindGroups;

};

#endif // APPLICATION_H
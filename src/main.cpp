#include <iostream>
#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Dear ImGui
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "Shader.h"
#include "Camera.h"
#include "TerrainManager.h"
#include "TerrainChunk.h"
#include "TextureManager.h"
#include "GameSkybox.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Real-Time Terrain", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Preload textures
    TextureManager& texManager = TextureManager::getInstance();

    // Load all PBR texture sets
    texManager.loadPBRTextureSet("sand",
        "Assets/Textures/Ground093C_2K-JPG");

    texManager.loadPBRTextureSet("grass",
        "Assets/Textures/Grass001_2K-JPG");

    texManager.loadPBRTextureSet("rock",
        "Assets/Textures/Rock020_2K-JPG");

    texManager.loadPBRTextureSet("snow",
        "Assets/Textures/Snow004_2K-JPG");



    // Create camera
    Camera camera(SCR_WIDTH, SCR_HEIGHT, -90.0f, -20.0f, true, 0.1f, 50.0f, window);
    GameSkybox skybox;

    // test_rot
    //cubemap_8192x4096_V2 >> Final Version using blender_map.py
    std::vector<std::string> faces = {
        "Assets/Textures/Skybox2/cubemap_8192x4096_V2/right.jpg",
        "Assets/Textures/Skybox2/cubemap_8192x4096_V2/left.jpg",
        "Assets/Textures/Skybox2/cubemap_8192x4096_V2/top.jpg",
        "Assets/Textures/Skybox2/cubemap_8192x4096_V2/bottom.jpg",
        "Assets/Textures/Skybox2/cubemap_8192x4096_V2/front.jpg",
        "Assets/Textures/Skybox2/cubemap_8192x4096_V2/back.jpg"
    };

    if (!skybox.loadCubemap(faces)) {
        std::cout << "Using default gradient skybox" << std::endl;
    }
    // Load terrain manager
    TerrainManager terrainManager;

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glfwSwapInterval(1);

    // Initialize Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    const char* glsl_version = "#version 330";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Timing variables
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process input
        camera.processInput(window, deltaTime);
        camera.updateViewMatrix();

        // Clear buffers
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Simple ImGui window
        ImGui::Begin("Camera Info");
        ImGui::Text("Press WASD to move");
        ImGui::Text("Press SPACE/SHIFT to move up/down");
        ImGui::Text("Press P to toggle wireframe");
        ImGui::Text("Frame time: %.3f ms", deltaTime * 1000.0f);
        ImGui::Text("Camera Pos: (%.1f, %.1f, %.1f)",
            camera.getCameraPos().x,
            camera.getCameraPos().y,
            camera.getCameraPos().z);
        ImGui::End();
        glClear(GL_DEPTH_BUFFER_BIT);  // Clear depth only
        skybox.draw(camera.getViewMatrix(), camera.getProjectionMatrix());

        // Render terrain
        terrainManager.update(camera);

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

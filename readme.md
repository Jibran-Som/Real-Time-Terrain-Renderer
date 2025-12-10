# Real-Time Terrain Renderer

A basic real-time terrain rendering project using OpenGL, GLFW, GLAD, and Dear ImGui.

## Features
- Procedural terrain rendering
- Camera movement and controls
- Basic lighting and skybox
- GUI controls with Dear ImGui

## Build Instructions
1. Clone the repository.
2. Open `Real-Time Terrain Renderer.sln` in Visual Studio 2022.
3. Ensure your configuration is set to `Release | x64`.
4. Build the project.
5. Run the generated `.exe` in `x64/Release`.

## Project Structure

/Assets → textures, shaders, models

/src → source files (.cpp)

/headers → header files (.h)

/screenshots → Screenshots of the active running program


## Dependencies
- OpenGL 3.3+
- GLFW
- GLAD
- Dear ImGui

## Notes
- All required source files for third-party libraries are included (`glad.c`, `imgui_*.cpp`), so no extra setup is needed.
- You can optionally unzip `Real-Time Terrain Renderer.zip` for easy use. However after unzipping add the Assets folder into the folder the `.exe` is in to run the program properly.
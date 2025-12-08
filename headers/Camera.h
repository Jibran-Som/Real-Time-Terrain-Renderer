#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Camera{
    public:
        Camera(unsigned int src_width, unsigned int src_height, float yaw, float pitch, bool firstMouse, float mouseSensitivity, float speed, GLFWwindow* window);
        ~Camera();

        void processInput(GLFWwindow* window, float deltaTime);
        void mouseCallback(double xpos, double ypos);
        static void staticMouseCallback(GLFWwindow* window,double xpos, double ypos);
        void updateViewMatrix();

        glm::mat4 getViewMatrix() const { return view; }
        glm::mat4 getProjectionMatrix() const { return projection; }
        bool getWireframe() const { return wireframe; }
        glm::vec3 getCameraPos() const { return cameraPos; }


    private:
        float yaw;
        float pitch;
        float lastX;
        float lastY;
        bool firstMouse;
        bool wireframe;
        float mouseSensitivity;
        float speed;
        glm::vec3 cameraPos;
        glm::vec3 cameraFront;
        glm::vec3 cameraUp;
        glm::mat4 view;
        glm::mat4 projection;

        static Camera* instance;

};


#endif // CAMERA_H


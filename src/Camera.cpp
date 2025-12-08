#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>

Camera* Camera::instance = nullptr;

Camera::Camera(unsigned int src_width, unsigned int src_height, float yaw, float pitch, bool firstMouse, float mouseSensitivity, float speed, GLFWwindow* window)
{

    Camera::instance = this;

    this->yaw = yaw;
    this->pitch = pitch;
    this->firstMouse = firstMouse;
    this->mouseSensitivity = mouseSensitivity;
    this->speed = speed;
    glEnable(GL_DEPTH_TEST);
    lastX = src_width / 2.0f;
    lastY = src_height / 2.0f;
    wireframe = false;


    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    projection = glm::perspective(
        glm::radians(45.0f),
        (float)src_width / src_height,
        0.1f,
        1000.0f
    );


    view = glm::lookAt(
        glm::vec3(150, 150, 150),  // camera POSITION
        glm::vec3(0, 0, 0),        // look AT center
        glm::vec3(0, 1, 0)         // UP vector
    );


    cameraPos = glm::vec3(100.0f, 50.0f, 100.0f);
    cameraFront = glm::vec3(0.0f, -0.3f, -1.0f);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    updateViewMatrix();

    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, Camera::staticMouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}


void Camera::processInput(GLFWwindow* window, float deltaTime) { // 0.016f for ~60FPS
    if (deltaTime <= 0.0f) return;  // Guard against invalid deltaTime

    float cameraSpeed = speed * deltaTime; // frame independent

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos.y += cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraPos.y -= cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    static bool pKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !pKeyPressed) {
        wireframe = !wireframe;
        if (wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        pKeyPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) {
        pKeyPressed = false;
    }
}


void Camera::staticMouseCallback(GLFWwindow* window, double xpos, double ypos) {
    // Get camera instance from window user pointer
    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    if (camera) {
        camera->mouseCallback(xpos, ypos);
    }
}


void Camera::mouseCallback(double xpos, double ypos){
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;


    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;


    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void Camera::updateViewMatrix() {
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

Camera::~Camera() {
    // Destructor
}

#include "GameSkybox.h"
#include "Shader.h"
#include <iostream>
#include <vector>
#include "stb_image.h"
#include <glm/gtc/type_ptr.hpp>

GameSkybox::GameSkybox() : brightness(1.0f), skyboxShader(nullptr) {
    // Create shader first
    skyboxShader = new Shader("Assets/Shaders/skybox.vert", "Assets/Shaders/skybox.frag");
    setupSkybox();
    createDefaultCubemap(); // Create a simple blue sky as fallback
}

GameSkybox::~GameSkybox() {
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteTextures(1, &cubemapTexture);
    if (skyboxShader) {
        delete skyboxShader;
    }
}

void GameSkybox::setupSkybox() {
    // Skybox cube vertices (in NDC coordinates)
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint GameSkybox::loadCubemapTextures(const std::vector<std::string>& faces) {
    if (faces.size() != 6) {
        std::cout << "ERROR: Need exactly 6 faces for cubemap. Got " << faces.size() << std::endl;
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;

    // IMPORTANT: Cubemaps should NOT be flipped vertically
    stbi_set_flip_vertically_on_load(false);

    for (unsigned int i = 0; i < 6; i++) {
        std::cout << "Loading cubemap face: " << faces[i] << std::endl;

        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = GL_RGB;
            if (nrChannels == 1) {
                format = GL_RED;
            }
            else if (nrChannels == 3) {
                format = GL_RGB;
            }
            else if (nrChannels == 4) {
                format = GL_RGBA;
            }

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format,
                width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);

            std::cout << "  Success! " << width << "x" << height << " channels: " << nrChannels << std::endl;
        }
        else {
            std::cout << "  FAILED to load cubemap face: " << faces[i] << std::endl;
            stbi_image_free(data);

            // Load a colorful error texture
            unsigned char errorColor[3] = { 255, 0, 255 }; // Magenta
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
                1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, errorColor);
        }
    }

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Restore flip setting
    stbi_set_flip_vertically_on_load(true);

    return textureID;
}

void GameSkybox::createDefaultCubemap() {
    // Create a simple gradient blue sky as fallback
    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    unsigned char skyColors[6][3] = {
        {100, 150, 255}, // right
        {100, 150, 255}, // left
        {50, 100, 200},  // top (darker)
        {150, 200, 255}, // bottom (lighter)
        {100, 150, 255}, // front
        {100, 150, 255}  // back
    };

    for (unsigned int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
            1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, skyColors[i]);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    std::cout << "Created default gradient skybox" << std::endl;
}

bool GameSkybox::loadCubemap(const std::vector<std::string>& faces) {
    GLuint newTexture = loadCubemapTextures(faces);

    if (newTexture != 0) {
        // Delete old texture if it exists
        if (cubemapTexture != 0) {
            glDeleteTextures(1, &cubemapTexture);
        }
        cubemapTexture = newTexture;
        std::cout << "Skybox loaded successfully!" << std::endl;
        return true;
    }

    std::cout << "Failed to load skybox, using default" << std::endl;
    return false;
}

void GameSkybox::draw(const glm::mat4& view, const glm::mat4& projection) {
    if (!skyboxShader) return;

    // Change depth function so skybox passes depth test at far plane
    glDepthFunc(GL_LEQUAL);

    skyboxShader->use();

    // Remove translation from view matrix (skybox follows camera but doesn't move)
    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));

    skyboxShader->setMat4("view", glm::value_ptr(viewNoTranslation));
    skyboxShader->setMat4("projection", glm::value_ptr(projection));
    skyboxShader->setInt("skybox", 0);
    skyboxShader->setFloat("brightness", brightness);

    // Draw skybox
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Restore default depth function
    glDepthFunc(GL_LESS);
}

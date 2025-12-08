#pragma once
#ifndef GAMESKYBOX_H
#define GAMESKYBOX_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

class Shader;

class GameSkybox {
public:
    GameSkybox();
    ~GameSkybox();

    bool loadCubemap(const std::vector<std::string>& faces);
    void draw(const glm::mat4& view, const glm::mat4& projection);

    void setBrightness(float brightness) { this->brightness = brightness; }
    float getBrightness() const { return brightness; }

private:
    GLuint skyboxVAO, skyboxVBO;
    GLuint cubemapTexture;
    Shader* skyboxShader;
    float brightness;

    void setupSkybox();
    GLuint loadCubemapTextures(const std::vector<std::string>& faces);
    void createDefaultCubemap();
};

#endif // GAMESKYBOX_H

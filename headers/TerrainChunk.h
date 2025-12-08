#pragma once
#include <vector>
#include <glad/glad.h>
#include "FastNoiseLite.h"
#include <glm/glm.hpp>
#include "Shader.h"
#include "Camera.h"

#ifndef TERRAINCHUNK_H
#define TERRAINCHUNK_H

class TerrainChunk {
public:
    TerrainChunk(int chunkX, int chunkZ, int size, float noiseFreq, float noiseAmp);
    ~TerrainChunk();

    void generateHeightmap();
    void setupMesh();
    void draw(Camera camera);
    glm::mat4 getModelMatrix() const { return model; }
    Shader& getShader() { return shader; }

private:
    int chunkX, chunkZ;
    int size;
    float noiseFreq;
    float noiseAmp;

    unsigned int textureID;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    unsigned int VAO, VBO, EBO;
    FastNoiseLite noise;

    glm::mat4 model;

    Shader shader;

    std::vector<float> heights;

    void loadTexture();


    float sandHeight = -20.0f;
    float grassHeight = 10.0f;
    float rockHeight = 30.0f;
    float snowHeight = 45.0f;

    
};

#endif // TERRAINCHUNK_H

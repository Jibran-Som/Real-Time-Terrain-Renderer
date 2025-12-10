#include "TerrainChunk.h"
#include "TextureManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include <iostream>

TerrainChunk::TerrainChunk(int chunkX, int chunkZ, int size, float noiseFreq, float noiseAmp)
    : chunkX(chunkX), chunkZ(chunkZ), size(size), noiseFreq(noiseFreq), noiseAmp(noiseAmp),
    shader("Assets/Shaders/terrain.vert", "Assets/Shaders/terrain.frag")
{
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFractalType(FastNoiseLite::FractalType_None);  // Add fractal for more detail


    model = glm::translate(glm::mat4(1.0f), glm::vec3(chunkX * size, 0, chunkZ * size));

    generateHeightmap();
    setupMesh();
}

void TerrainChunk::generateHeightmap() {
    vertices.clear();
    heights.clear();
    indices.clear();

    float texScale = 1.0f / size;

    for (int z = 0; z <= size; z++) {
        for (int x = 0; x <= size; x++) {
            float worldX = (chunkX * size) + x;
            float worldZ = (chunkZ * size) + z;

            // Get noise value
            float height = noise.GetNoise(worldX * noiseFreq, worldZ * noiseFreq) * noiseAmp;

            height = height * noiseAmp;  // This gives range [-noiseAmp, noiseAmp]

            // Store height for this vertex
            heights.push_back(height);

            // Vertex position
            vertices.push_back(static_cast<float>(x));
            vertices.push_back(height);
            vertices.push_back(static_cast<float>(z));

            // Texture coordinates (scale for more repetition)
            vertices.push_back(static_cast<float>(x) * texScale * 2.0f);
            vertices.push_back(static_cast<float>(z) * texScale * 2.0f);
        }
    }

    // Indices
    for (int z = 0; z < size; z++) {
        for (int x = 0; x < size; x++) {
            int topLeft = z * (size + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (size + 1) + x;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
}

void TerrainChunk::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW);

    // Vertex attribute - position (x y z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Vertex attribute - texture coordinates (u v)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
        5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void TerrainChunk::draw(Camera camera) {
    shader.use();

    TextureManager& texManager = TextureManager::getInstance();

    bool hasSandPBR = texManager.hasPBRTextureSet("sand");
    bool hasGrassPBR = texManager.hasPBRTextureSet("grass");
    bool hasRockPBR = texManager.hasPBRTextureSet("rock");
    bool hasSnowPBR = texManager.hasPBRTextureSet("snow");

    if (hasSandPBR && hasGrassPBR && hasRockPBR && hasSnowPBR) {
        texManager.bindPBRTextures("sand", 0);
        texManager.bindPBRTextures("grass", 4);
        texManager.bindPBRTextures("rock", 8);
        texManager.bindPBRTextures("snow", 12);

        shader.setInt("sandAlbedo", 0);
        shader.setInt("sandNormal", 1);
        shader.setInt("sandRoughness", 2);
        shader.setInt("sandAO", 3);

        shader.setInt("grassAlbedo", 4);
        shader.setInt("grassNormal", 5);
        shader.setInt("grassRoughness", 6);
        shader.setInt("grassAO", 7);

        shader.setInt("rockAlbedo", 8);
        shader.setInt("rockNormal", 9);
        shader.setInt("rockRoughness", 10);
        shader.setInt("rockAO", 11);

        shader.setInt("snowAlbedo", 12);
        shader.setInt("snowNormal", 13);
        shader.setInt("snowRoughness", 14);
        shader.setInt("snowAO", 15);
    }
    else {
        std::cout << "WARNING: Not all PBR textures loaded, using fallback" << std::endl;
        texManager.bindTexture("fallback", 0);
        shader.setInt("fallbackTexture", 0);
    }

    shader.setFloat("sandHeight", sandHeight);
    shader.setFloat("grassHeight", grassHeight);
    shader.setFloat("rockHeight", rockHeight);
    shader.setFloat("snowHeight", snowHeight);

 
    shader.setVec3("lightPos", glm::vec3(500.0f, 1000.0f, 500.0f));
    shader.setVec3("lightColor", glm::vec3(1.2f, 1.1f, 0.95f));
    shader.setVec3("viewPos", camera.getCameraPos());


    shader.setMat4("projection", glm::value_ptr(camera.getProjectionMatrix()));
    shader.setMat4("view", glm::value_ptr(camera.getViewMatrix()));
    shader.setMat4("model", glm::value_ptr(this->model));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}



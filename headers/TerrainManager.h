
#pragma once
#ifndef TERRAINMANAGER_H
#define TERRAINMANAGER_H

#include <unordered_map>
#include <glm/glm.hpp>
#include "TerrainChunk.h"
#include "Shader.h"
#include "Camera.h"

class TerrainManager {
public:
    std::unordered_map<long long, TerrainChunk*> chunks;

    int chunkSize = 32;
    int renderDistance = 6; // number of chunks
    // TerrainManager.h
// In TerrainManager.h
    float noiseFreq = 0.7f; // Slightly higher freq to ensure we see features
    float noiseAmp = 8.0f;   // Total range approx -60 to +60 due to the 1.2x bias

  
    void update(Camera camera);

    long long hash(int x, int z);


};

#endif // TERRAINMANAGER_H

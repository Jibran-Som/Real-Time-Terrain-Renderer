#include "TerrainManager.h"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Camera.h"



long long TerrainManager::hash(int x, int z) {
    return (((long long)x) << 32) | (unsigned int)z;
}


void TerrainManager::update(Camera camera) {
    glm::vec3 cameraPos = camera.getCameraPos();
    glm::mat4 projection = camera.getProjectionMatrix();
    glm::mat4 view = camera.getViewMatrix();
    int camChunkX = floor(cameraPos.x / chunkSize);
    int camChunkZ = floor(cameraPos.z / chunkSize);

    for (int dz = -renderDistance; dz <= renderDistance; dz++) {
        for (int dx = -renderDistance; dx <= renderDistance; dx++) {
            int cx = camChunkX + dx;
            int cz = camChunkZ + dz;
            long long key = hash(cx, cz);

            if (chunks.find(key) == chunks.end()) {
                chunks[key] = new TerrainChunk(cx, cz, chunkSize, noiseFreq, noiseAmp);
            }

            chunks[key]->draw(camera);  // Pass matrices
        }
    }
}

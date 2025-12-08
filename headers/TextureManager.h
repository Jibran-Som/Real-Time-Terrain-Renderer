#pragma once
#include <glad/glad.h>
#include <string>
#include <unordered_map>

class TextureManager {
public:
    static TextureManager& getInstance();

    unsigned int loadTexture(const std::string& name, const std::string& path);
    unsigned int getTexture(const std::string& name);
    void bindTexture(const std::string& name, GLuint textureUnit = 0);

    // PBR texture set loading
    void loadPBRTextureSet(const std::string& name, const std::string& basePath);
    bool hasPBRTextureSet(const std::string& name);
    void bindPBRTextures(const std::string& name, GLuint startUnit = 0);

private:
    TextureManager() = default;

    // Simple textures
    std::unordered_map<std::string, unsigned int> textures;

    // PBR texture sets: name -> {type -> textureID}
    std::unordered_map<std::string, std::unordered_map<std::string, unsigned int>> pbrTextures;

    unsigned int loadTextureFromFile(const std::string& path);



};

#include "TextureManager.h"
#include <iostream>
#include <vector>
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


#ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif

#ifndef GL_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#endif

TextureManager& TextureManager::getInstance() {
    static TextureManager instance;
    return instance;
}

unsigned int TextureManager::loadTextureFromFile(const std::string& path) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    unsigned int textureID = 0;

    if (data) {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Enable anisotropic filtering if available
        if (glfwExtensionSupported("GL_EXT_texture_filter_anisotropic")) {
            float maxAnisotropy = 0.0f;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
        }

        GLenum format = GL_RGB;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
        std::cout << "Loaded texture: " << path << " (" << width << "x" << height << ")" << std::endl;
    }
    else {
        std::cout << "Failed to load texture: " << path << std::endl;
    }

    return textureID;
}

unsigned int TextureManager::loadTexture(const std::string& name, const std::string& path) {
    unsigned int textureID = loadTextureFromFile(path);
    if (textureID != 0) {
        textures[name] = textureID;
    }
    return textureID;
}

unsigned int TextureManager::getTexture(const std::string& name) {
    auto it = textures.find(name);
    if (it != textures.end()) {
        return it->second;
    }
    return 0;
}

void TextureManager::bindTexture(const std::string& name, GLuint textureUnit) {
    auto it = textures.find(name);
    if (it != textures.end()) {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, it->second);
    }
}

void TextureManager::loadPBRTextureSet(const std::string& name, const std::string& basePath) {
    std::unordered_map<std::string, unsigned int> textureSet;

    std::vector<std::pair<std::string, std::string>> textureTypes;

    if (name == "grass") {
        textureTypes = {
            {"albedo", "Grass001_2K-JPG_Color.jpg"},
            {"normal", "Grass001_2K-JPG_NormalGL.jpg"},
            {"roughness", "Grass001_2K-JPG_Roughness.jpg"},
            {"ao", "Grass001_2K-JPG_AmbientOcclusion.jpg"},
            {"displacement", "Grass001_2K-JPG_Displacement.jpg"}
        };
    }
    else if (name == "rock") {
        textureTypes = {
            {"albedo", "Rock020_2K-JPG_Color.jpg"},
            {"normal", "Rock020_2K-JPG_NormalGL.jpg"},
            {"roughness", "Rock020_2K-JPG_Roughness.jpg"},
            {"ao", "Rock020_2K-JPG_AmbientOcclusion.jpg"},
            {"displacement", "Rock020_2K-JPG_Displacement.jpg"}
        };
    }

    else if (name == "snow") {
        textureTypes = {
            {"albedo", "Snow004_2K-JPG_Color.jpg"},
            {"normal", "Snow004_2K-JPG_NormalGL.jpg"},
            {"roughness", "Snow004_2K-JPG_Roughness.jpg"},
            {"ao", "Snow004_2K-JPG_AmbientOcclusion.jpg"},
            {"displacement", "Snow004_2K-JPG_Displacement.jpg"}
        };
    }
    else if (name == "sand") {
        textureTypes = {
            {"albedo", "Ground093C_2K-JPG_Color.jpg"},
            {"normal", "Ground093C_2K-JPG_NormalGL.jpg"},
            {"roughness", "Ground093C_2K-JPG_Roughness.jpg"},
            {"ao", "Ground093C_2K-JPG_AmbientOcclusion.jpg"},
            { "displacement", "Ground093C_2K-JPG_Displacement.jpg" }

        };
    }

    for (const auto& pair : textureTypes) {
        const std::string& type = pair.first;
        const std::string& filename = pair.second;
        std::string fullPath = basePath + "/" + filename;
        unsigned int texID = loadTextureFromFile(fullPath);
        if (texID != 0) {
            textureSet[type] = texID;
            std::cout << "  Loaded " << type << " map: " << filename << std::endl;
        }
    }

    if (!textureSet.empty()) {
        pbrTextures[name] = textureSet;
        std::cout << "Successfully loaded PBR texture set: " << name << std::endl;
    }
    else {
        std::cout << "Failed to load any textures for PBR set: " << name << std::endl;
    }
}

bool TextureManager::hasPBRTextureSet(const std::string& name) {
    return pbrTextures.find(name) != pbrTextures.end();
}

void TextureManager::bindPBRTextures(const std::string& name, GLuint startUnit) {
    auto it = pbrTextures.find(name);
    if (it != pbrTextures.end()) {
        const auto& textureSet = it->second;

        int unit = startUnit;
        if (textureSet.find("albedo") != textureSet.end()) {
            glActiveTexture(GL_TEXTURE0 + unit++);
            glBindTexture(GL_TEXTURE_2D, textureSet.at("albedo"));
        }
        if (textureSet.find("normal") != textureSet.end()) {
            glActiveTexture(GL_TEXTURE0 + unit++);
            glBindTexture(GL_TEXTURE_2D, textureSet.at("normal"));
        }
        if (textureSet.find("roughness") != textureSet.end()) {
            glActiveTexture(GL_TEXTURE0 + unit++);
            glBindTexture(GL_TEXTURE_2D, textureSet.at("roughness"));
        }
        if (textureSet.find("ao") != textureSet.end()) {
            glActiveTexture(GL_TEXTURE0 + unit++);
            glBindTexture(GL_TEXTURE_2D, textureSet.at("ao"));
        }
    }
}


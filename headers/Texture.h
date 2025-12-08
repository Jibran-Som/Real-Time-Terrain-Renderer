#pragma once
#include <glad/glad.h>
#include <string>

class Texture {
public:
    unsigned int ID;
    int width, height, nrChannels;

    Texture(const std::string& path, bool flipVertically = true);
    void bind(int textureUnit = 0) const;

private:
    void loadTexture(const std::string& path, bool flipVertically);
};

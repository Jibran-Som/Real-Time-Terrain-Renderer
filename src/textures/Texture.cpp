#include "Texture.h"
#include <iostream>
#include "stb_image.h"

Texture::Texture(const std::string& path, bool flipVertically) {
    stbi_set_flip_vertically_on_load(flipVertically);
    loadTexture(path, flipVertically);
}

void Texture::loadTexture(const std::string& path, bool flipVertically) {
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
    }
    else {
        std::cout << "Failed to load texture: " << path << std::endl;
    }
}

void Texture::bind(int textureUnit) const {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, ID);
}

#include "Texture.hpp"
#include <iostream>

Texture::Texture(Type type)
    : textureID(0)
    , type(type)
    , width(0)
    , height(0)
    , format(Format::RGBA)
{
    glGenTextures(1, &textureID);
}

Texture::~Texture() {
    cleanup();
}

bool Texture::loadFromFile(const std::string& path) {
    // Note: In a real engine, we'd use a library like stb_image here
    // For now, this is a placeholder for the implementation
    std::cout << "Texture loading from file would be implemented here: " << path << std::endl;
    return false;
}

bool Texture::loadFromData(unsigned char* data, int w, int h, Format fmt) {
    if (!data) return false;

    width = w;
    height = h;
    format = fmt;

    GLenum glFormat = GL_RGBA;
    switch (format) {
        case Format::RGB: glFormat = GL_RGB; break;
        case Format::RGBA: glFormat = GL_RGBA; break;
        case Format::Depth: glFormat = GL_DEPTH_COMPONENT; break;
    }

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, glFormat, width, height, 0, glFormat, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set default filtering
    setFilterMode(FilterMode::Linear);
    setWrapMode(WrapMode::Repeat);

    return true;
}

void Texture::bind(unsigned int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setFilterMode(FilterMode mode) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    switch (mode) {
        case FilterMode::Nearest:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        case FilterMode::Linear:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        case FilterMode::Trilinear:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
    }
}

void Texture::setWrapMode(WrapMode mode) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLint glMode;
    switch (mode) {
        case WrapMode::Repeat: glMode = GL_REPEAT; break;
        case WrapMode::MirroredRepeat: glMode = GL_MIRRORED_REPEAT; break;
        case WrapMode::ClampToEdge: glMode = GL_CLAMP_TO_EDGE; break;
        case WrapMode::ClampToBorder: glMode = GL_CLAMP_TO_BORDER; break;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glMode);
}

void Texture::cleanup() {
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }
}

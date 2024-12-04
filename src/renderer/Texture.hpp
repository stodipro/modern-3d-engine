#pragma once
#include <string>
#include <GL/glew.h>

class Texture {
public:
    enum class Type {
        Texture2D,
        CubeMap
    };

    enum class Format {
        RGB,
        RGBA,
        Depth
    };

    enum class FilterMode {
        Nearest,
        Linear,
        Trilinear
    };

    enum class WrapMode {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder
    };

    Texture(Type type = Type::Texture2D);
    ~Texture();

    bool loadFromFile(const std::string& path);
    bool loadFromData(unsigned char* data, int width, int height, Format format);
    void bind(unsigned int unit = 0) const;
    void unbind() const;

    void setFilterMode(FilterMode mode);
    void setWrapMode(WrapMode mode);

    unsigned int getID() const { return textureID; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    unsigned int textureID;
    Type type;
    int width;
    int height;
    Format format;

    void cleanup();
};

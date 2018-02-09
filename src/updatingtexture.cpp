#include "updatingtexture.h"
#include <glad/glad.h>
#include "stb_image.h"

UpdatingTexture::UpdatingTexture()
    : _pixels(nullptr)
{
}

unsigned int UpdatingTexture::textureId() const
{
    return _textureId;
}

void UpdatingTexture::loadTexture(std::string const &filename)
{
    int x, y;
    _pixels = stbi_load(filename.c_str(), &x, &y, &_comp, 3);
    if (_pixels == nullptr)
    {
        return;
    }

    _textureSize = glm::vec2(x, y);

    glGenTextures(1, &_textureId);

    glBindTexture(GL_TEXTURE_2D, _textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, _comp == 4 ? GL_RGBA : GL_RGB, _textureSize.x, _textureSize.y, 0, _comp == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, _pixels);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void UpdatingTexture::setPlaneSize(glm::vec2 const &planeSize)
{
    _planeSize = planeSize;
}

void UpdatingTexture::paintPixel(glm::vec2 const &at, std::vector<unsigned char> const &color)
{
    if (at.x < 0 || at.x > _textureSize.x)
    {
        return;
    }
    if (at.y < 0 || at.y > _textureSize.y)
    {
        return;
    }

    auto pixelOffset = int((at.y * _textureSize.x) + at.x) * _comp;

    for (int i = 0; i < color.size(); i++)
    {
        _pixels[pixelOffset + i] = color[i];
    }
}

void UpdatingTexture::paintLine(glm::vec2 const &from, glm::vec2 const &to, std::vector<unsigned char> const &color)
{
    float x1 = from.x;
    float y1 = from.y;
    float x2 = to.x;
    float y2 = to.y;
    {
        // Bresenham's line algorithm
        const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
        if (steep)
        {
            std::swap(x1, y1);
            std::swap(x2, y2);
        }

        if (x1 > x2)
        {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }

        const float dx = x2 - x1;
        const float dy = fabs(y2 - y1);

        float error = dx / 2.0f;
        const int ystep = (y1 < y2) ? 1 : -1;
        int y = (int)y1;

        const int maxX = (int)x2;

        for (int x = (int)x1; x < maxX; x++)
        {
            if (steep)
            {
                paintPixel(glm::vec2(y, x), color);
            }
            else
            {
                paintPixel(glm::vec2(x, y), color);
            }

            error -= dy;
            if (error < 0)
            {
                y += ystep;
                error += dx;
            }
        }
    }
}

void UpdatingTexture::paintOn(glm::mat4 const &modelMatrix)
{
    if (_pixels == nullptr)
    {
        return;
    }

    // Calculate the position in texture-space
    glm::vec2 pos(
        int((modelMatrix[3].x + (_planeSize.x / 2.0f)) * (_textureSize.x / _planeSize.x)),
        int((modelMatrix[3].y + (_planeSize.y / 2.0f)) * (_textureSize.y / _planeSize.y)));

    glm::vec2 dir = glm::vec2(modelMatrix[1].x, modelMatrix[1].y);
    glm::vec2 right = glm::vec2(modelMatrix[0].x, modelMatrix[0].y);

    // three lines to make sure the most pixels are painted over even when the car is moving fast or making  sharp turn
    auto localPos = pos + (dir * 10.0f);
    paintLine(localPos + (right * 10.0f), localPos + (right * -10.0f), std::vector<unsigned char>({0, 255, 0, 0}));
    localPos = pos + (dir * 9.0f);
    paintLine(localPos + (right * 10.0f), localPos + (right * -10.0f), std::vector<unsigned char>({0, 255, 0, 0}));
    localPos = pos + (dir * 8.0f);
    paintLine(localPos + (right * 10.0f), localPos + (right * -10.0f), std::vector<unsigned char>({0, 255, 0, 0}));

    glTexImage2D(GL_TEXTURE_2D, 0, _comp == 4 ? GL_RGBA : GL_RGB, _textureSize.x, _textureSize.y, 0, _comp == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, _pixels);
}

std::vector<glm::vec2> UpdatingTexture::listBluePixels()
{
    std::vector<glm::vec2> result;
    for (int y = 0; y < _textureSize.y; ++y)
    {
        for (int x = 0; x < _textureSize.x; ++x)
        {
            auto pixelOffset = int((y * _textureSize.x) + x) * _comp;
            if (_pixels[pixelOffset + 2] > 1)
            {
                result.push_back(glm::vec2(
                                     (-_planeSize.x / 2.0f) + (x / _textureSize.x) * _planeSize.x,
                                     (-_planeSize.y / 2.0f) + (y / _textureSize.y) * _planeSize.y));
            }
        }
    }

    return result;
}

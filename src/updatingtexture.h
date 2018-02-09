#ifndef UPDATINGTEXTURE_H
#define UPDATINGTEXTURE_H

#include <glm/glm.hpp>
#include <vector>
#include <string>

class UpdatingTexture
{
    unsigned int _textureId;
    int _comp;
    glm::vec2 _planeSize;
    unsigned char *_pixels;

    void paintPixel(glm::vec2 const &at, std::vector<unsigned char> const &color);
    void paintLine(glm::vec2 const &from, glm::vec2 const &to, std::vector<unsigned char> const &color);
public:
    UpdatingTexture();

    glm::vec2 _textureSize;
    unsigned int textureId() const;

    void loadTexture(std::string const &filename);
    void setPlaneSize(glm::vec2 const &planeSize);

    void paintOn(glm::mat4 const &modelMatrix);

    std::vector<glm::vec2> listBluePixels();
};

#endif // UPDATINGTEXTURE_H

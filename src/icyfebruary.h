#ifndef ICYFEBRUARY_H
#define ICYFEBRUARY_H

#include "tiny_obj_loader.h"

#include "game.h"
#include "gl-color-normal-position-vertex.h"
#include "gl-masked-textures.h"
#include "physics.h"
#include "updatingtexture.h"

#include <string>

enum class MenuModes
{
    NoMenu,
    MainMenu,
    KeyMappingMenu,
};

class IcyFebruary : public Game
{
    glm::mat4 _proj, _view;
    glm::vec3 _pos;

    std::string _settingsDir;
    MenuModes _menuMode;

    MaskedTexturesBuffer::ShaderType _floorShader;
    MaskedTexturesBuffer::BufferType _floor;
    ShaderType _boxShader;
    BufferType _car;
    BufferType _truck;
    BufferType _wheelLeft;
    BufferType _wheelRight;
    BufferType _tree;
    float _camOffset[3];

    unsigned int _snowTexture;
    unsigned int _grassTexture;
    unsigned int _asphaltTexture;
    UpdatingTexture _maskTexture;

    PhysicsManager _physics;
    PhysicsObject *_floorObject;
    CarObject *_carObject;
    std::vector<PhysicsObject *> _treeObjects;
    std::vector<glm::vec2> _treeLocations;

    unsigned int uploadTexture(std::string const &filename);

public:
    IcyFebruary(int argc, char *argv[]);

    virtual bool Setup();
    virtual void Resize(int width, int height);
    virtual void Update(int dt);
    virtual void RenderUi();
    virtual void Render();
    virtual void Destroy();
};

#endif // ICYFEBRUARY_H

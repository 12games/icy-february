#ifndef ICYFEBRUARY_H
#define ICYFEBRUARY_H

#include <tiny_gltf_loader.h>
#include <tiny_obj_loader.h>

#include "game.h"
#include "gl-color-normal-position-vertex.h"
#include <gl-color-position-vertex.h>
#include "physics.h"

#include <string>

enum class MenuModes
{
    NoMenu,
    MainMenu,
    KeyMappingMenu,
};

class CreationObject
{
public:
    glm::vec3 _pos;
    glm::vec3 _size;

    void rebuildBuffer();

    static ColorPosition::ShaderType _shader;
    ColorPosition::BufferType _buffer;
};

class IcyFebruary : public Game
{
    bool _showPhysicsDebug;
    glm::mat4 _proj, _view;
    glm::vec3 _pos;

    std::string _settingsDir;
    MenuModes _menuMode;

    BufferType _floor;
    ShaderType _boxShader;
    float _camOffset[3];

    PhysicsManager _physics;
    PhysicsObject *_floorObject;
    BufferType _character;
    CharacterObject *_characterObject;
    BufferType _fridge;
    CreationObject *_create;

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

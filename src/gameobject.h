#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glm/glm.hpp>

class GameObject
{
public:
    GameObject();

    class BufferType* _buffer;
    class ShaderType* _shader;
    class PhysicsObject* _physics;

};

#endif // GAMEOBJECT_H

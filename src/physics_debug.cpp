#include "physics.h"
#include <gl-color-position-vertex.h>
#include <LinearMath/btIDebugDraw.h>
#include <iostream>

class DebugDrawer : public btIDebugDraw
{
    int _debugMode;
    ColorPosition::ShaderType _shader;
    ColorPosition::BufferType _buffer;

public:
    DebugDrawer();

    void init();
    void render(glm::mat4 const &proj, glm::mat4 const &view);

    virtual void clearLines();

    virtual void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color);
    virtual void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color);

    virtual void reportErrorWarning(const char *warningString) {}
    virtual void draw3dText(const btVector3 &location, const char *textString) {}
    virtual void setDebugMode(int debugMode) { _debugMode = debugMode; }
    virtual int getDebugMode() const { return _debugMode; }
};

DebugDrawer::DebugDrawer()
    : _debugMode(btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_DrawConstraints + btIDebugDraw::DBG_DrawNormals)
{
    _buffer.setDrawMode(GL_LINES);
}

void DebugDrawer::clearLines()
{
    _buffer.cleanup();
}

void DebugDrawer::init()
{
    _shader.compileDefaultShader();
}

void DebugDrawer::render(glm::mat4 const &proj, glm::mat4 const &view)
{
    _buffer.setup(&_shader);

    _shader.use();
    _shader.setupMatrices(proj, view, glm::mat4(1.0f));
    _buffer.render();
}

void DebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
    _buffer.color(glm::vec4(color.x(), color.y(), color.z(), 1.0f));
    _buffer.vertex(glm::vec3(from.x(), from.y(), from.z()));
    _buffer.vertex(glm::vec3(to.x(), to.y(), to.z()));
}

void DebugDrawer::drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color)
{
    drawLine(PointOnB, PointOnB + normalOnB * distance, color);
    btVector3 ncolor(0, 0, 0);
    drawLine(PointOnB, PointOnB + normalOnB * 0.01, ncolor);
}

void PhysicsManager::InitDebugDraw()
{
    if (_drawer == nullptr)
    {
        _drawer = new DebugDrawer();
        _drawer->init();
    }
    _dynamicsWorld->setDebugDrawer(_drawer);
}

void PhysicsManager::DebugDraw(glm::mat4 const &proj, glm::mat4 const &view)
{
    _drawer->clearLines();
    _dynamicsWorld->debugDrawWorld();

    _drawer->render(proj, view);
}

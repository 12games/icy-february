/* 
 * File:   Physics.h
 * Author: Administrator
 *
 * Created on 28 oktober 2010, 2:08
 */

#ifndef PHYSICS_H
#define PHYSICS_H

#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <LinearMath/btIDebugDraw.h>
#include <LinearMath/btMotionState.h>
#include <btBulletDynamicsCommon.h>

#include "physicsobject.h"

class PhysicsManager
{
    friend class PhysicsObjectBuilder;
private:
    btBroadphaseInterface *_broadphase;
    btDefaultCollisionConfiguration *_collisionConfiguration;
    btCollisionDispatcher *_dispatcher;
    btSequentialImpulseConstraintSolver *_solver;
    btDiscreteDynamicsWorld *_dynamicsWorld;

    static struct Config
    {
        float _gravity;

    } _config;

    class DebugDrawer *_drawer;

public:
    PhysicsManager();
    virtual ~PhysicsManager();

    void InitDebugDraw();
    void DebugDraw(glm::mat4 const &proj, glm::mat4 const &view);

    void Step(float gameTime);

    void AddObject(PhysicsObject *obj, short group = btBroadphaseProxy::DefaultFilter, short mask = btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::StaticFilter | btBroadphaseProxy::CharacterFilter);
    void RemoveObject(PhysicsObject *obj);
};

#endif /* PHYSICS_H */

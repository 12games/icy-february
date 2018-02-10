#include "physics.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

using namespace std;

PhysicsManager::Config PhysicsManager::_config = {9.81f};

PhysicsManager::PhysicsManager()
    : _drawer(nullptr)
{
    this->_broadphase = new btDbvtBroadphase();

    this->_collisionConfiguration = new btDefaultCollisionConfiguration();
    this->_dispatcher = new btCollisionDispatcher(this->_collisionConfiguration);

    this->_solver = new btSequentialImpulseConstraintSolver();

    this->_dynamicsWorld = new btDiscreteDynamicsWorld(this->_dispatcher, this->_broadphase, this->_solver, this->_collisionConfiguration);
    this->_dynamicsWorld->setGravity(btVector3(0, 0, -PhysicsManager::_config._gravity));
}

PhysicsManager::~PhysicsManager()
{
    if (this->_dynamicsWorld != 0)
        delete this->_dynamicsWorld;
    this->_dynamicsWorld = 0;

    if (this->_solver != 0)
        delete this->_solver;
    this->_solver = 0;

    if (this->_dispatcher != 0)
        delete this->_dispatcher;
    this->_dispatcher = 0;

    if (this->_collisionConfiguration != 0)
        delete this->_collisionConfiguration;
    this->_collisionConfiguration = 0;

    if (this->_broadphase != 0)
        delete this->_broadphase;
    this->_broadphase = 0;
}

void PhysicsManager::Step(float gameTime)
{
    this->_dynamicsWorld->stepSimulation(1.0f/60.0f, 1);
    int numManifolds = this->_dynamicsWorld->getDispatcher()->getNumManifolds();

    for (int i = 0; i < numManifolds; i++)
    {
        btPersistentManifold *contactManifold = this->_dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

        if (contactManifold->getNumContacts() <= 0) continue;

        if (contactManifold->getBody0()->getUserPointer() == nullptr) continue;
        if (contactManifold->getBody1()->getUserPointer() == nullptr) continue;

        //        auto entityA = static_cast<GameObject *>(contactManifold->getBody0()->getUserPointer());
        //        if (entityA == nullptr) continue;

        //        auto entityB = static_cast<GameObject *>(contactManifold->getBody1()->getUserPointer());
        //        if (entityB == nullptr) continue;

        //        for (auto handler : this->_collisionHandlers)
        //            handler->handleCollision(entityA, entityB);
        //        this->_collisionHandlers->handleCollision(entityA, entityB);
        //        contactManifold->clearManifold();
    }
}

void PhysicsManager::AddObject(PhysicsObject *obj, short group, short mask)
{
    if (obj == nullptr)
    {
        return;
    }

    this->_dynamicsWorld->addRigidBody(obj->getRigidBody(), group, mask);
}

void PhysicsManager::RemoveObject(PhysicsObject *obj)
{
    if (obj == nullptr)
    {
        return;
    }

    this->_dynamicsWorld->removeCollisionObject(obj->getRigidBody());
}

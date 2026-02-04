#pragma once
#include <ecs/ecs.hpp>
#include <vector>
#include "rigidbody2d.hpp"
#include "collider2d.hpp"

class PhysicsWorld
{
public:
    PhysicsWorld() {}
    ~PhysicsWorld() { Clean(); }

    void setGravity(vec2 g) { gravity = g; }
    void addRigidBody(Rigidbody2D *rigid)
    {
        if (std::find(bodies.begin(), bodies.end(), rigid) == bodies.end())
            bodies.push_back(rigid);
    }

    void removeRigidBody(Rigidbody2D *rigid)
    {
        auto it = std::find(bodies.begin(), bodies.end(), rigid);
        if (it != bodies.end())
        {
            bodies.erase(it); // Erase the found element
            delete rigid;
        }
        else
        {
            throw std::runtime_error("This body is not found in bodies!");
        }
    }

    void step(const float &deltaTime)
    {
    }

    void Clean()
    {
        for (auto body : bodies)
            delete body;
    }

private:
    vec2 gravity = {0.0f, -9.81f};
    std::vector<Rigidbody2D *> bodies;
};
#pragma once
#include <ecs/ecs.hpp>
#include <ecs/system.hpp>
#include <physics/world.hpp>

#include <components/physics/rigidbody2d.hpp>
#include <components/physics/box.hpp>

class PhysicsSystem : public System
{
public:
    PhysicsSystem() : System("PhyiscsSystem")
    {
        world = new PhysicsWorld();
    }

    ~PhysicsSystem()
    {
        if (world)
            delete world;

        for (auto shape : ownedShape)
            delete shape;
    }

    void OnUpdate(World &entities, float deltaTime) override
    {
        accumulator += deltaTime;

        while (accumulator >= fixedDeltaTime)
        {
            RegisterNewBodies(entities);
            world->step(fixedDeltaTime);
            accumulator -= fixedDeltaTime;
        }
    }

private:
    PhysicsWorld *world = nullptr;
    std::vector<physics::PhysicsShape *> ownedShape;

    float fixedDeltaTime = 1.0f / 60.0f;
    float accumulator = 0.0f;

    void RegisterNewBodies(World &entities)
    {
        for (const auto &[id, entity] : entities.GetEntities())
        {
            auto phys = entity->GetComponent<PhysicsComponent>();
            if (phys->body)
                continue;

            auto collider = entity->GetComponent<Collider>();
            if (!collider)
                continue;

            auto rigidbody = entity->GetComponent<Rigidbody2D>();
            CreateRigidBody(entity.get(), phys, collider, rigidbody);
        }
    }

    void CreateRigidBody(Entity *entity, PhysicsComponent *phyis, Collider *collider, Rigidbody2D *rigid)
    {
    }
};
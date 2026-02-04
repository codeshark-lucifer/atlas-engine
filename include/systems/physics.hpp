#pragma once
#include <iostream>
#include <ecs/ecs.hpp>
#include <ecs/system.hpp>
#include <physics/world.hpp>
#include <physics/rigidbody2d.hpp>
#include <physics/collider2d.hpp>

#include <components/physics/rigidbody2d.hpp>
#include <components/physics/collider2d.hpp>
#include <components/physics/box.hpp>
#include <components/physics/circle.hpp>

class PhysicsSystem : public System
{
public:
    PhysicsSystem() : System("PhysicsSystem")
    {
        world = new physics::PhysicsWorld();
        world->setGravity(vec2(0.0f, -9.81f));
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
            SyncTransforms(entities);
            accumulator -= fixedDeltaTime;
        }
    }

private:
    physics::PhysicsWorld *world = nullptr;
    std::vector<physics::PhysicsShape *> ownedShape;

    float fixedDeltaTime = 1.0f / 60.0f;
    float accumulator = 0.0f;

    void SyncTransforms(World &entities)
    {
        for (const auto &[id, entity] : entities.GetEntities())
        {
            auto body = entity->GetComponent<::Rigidbody2D>();
            if (!body || !body->body)
                continue;

            auto motion = body->body->GetMotion();
            auto *trans = entity->GetComponent<Transform>();
            if (!trans)
                continue;

            trans->position = vec3(motion.position.x, motion.position.y, 0);
            trans->rotation = motion.quaternion;
        }
    }

    void RegisterNewBodies(World &entities)
    {
        for (const auto &[id, entity] : entities.GetEntities())
        {
            // Look for any collider (BoxCollider2D or CircleCollider2D)
            auto boxCollider = entity->GetComponent<BoxCollider2D>();
            auto circleCollider = entity->GetComponent<CircleCollider2D>();
            Collider2D *collider = boxCollider ? (Collider2D *)boxCollider : (circleCollider ? (Collider2D *)circleCollider : nullptr);

            auto rigidbody = entity->GetComponent<::Rigidbody2D>();

            // Need both collider and rigidbody to create physics
            if (!collider || !rigidbody)
                continue;

            // Skip if already registered
            if (rigidbody->body)
                continue;

            CreateRigidBody(entity.get(), collider, rigidbody);
        }
    }

    void CreateRigidBody(Entity *entity, Collider2D *collider, ::Rigidbody2D *rigid)
    {
        float mass = rigid ? rigid->mass : 0.0f;

        auto *shape = collider->CreateShape();
        ownedShape.push_back(shape);

        Transform trans = *entity->GetComponent<Transform>();
        float inertia = 0.0f;
        if (mass > 0.0f && shape)
            inertia = shape->CalculateLocalInertia(mass);

        auto motion = physics::MotionState::create(
            {trans.position.x, trans.position.y},
            trans.rotation);

        auto *body = new physics::Rigidbody2D(
            mass,
            motion,
            shape);

        if (rigid)
        {
            if (rigid->isKinematic)
            {
                body->flag = CollistionObject::Kinematic;
            }
            if (!rigid->useGravity)
                body->gravity = {0.0f, 0.0f};
        }

        world->addRigidBody(body);

        rigid->body = body;
        collider->body = body;

        rigid->shape = shape;
        collider->shape = shape;
    }
};
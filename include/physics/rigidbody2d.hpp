#pragma once
#include <ecs/ecs.hpp>
#include "collider2d.hpp"
#include "comp.h"

namespace physics{
    struct Rigidbody2D 
    {
        Rigidbody2D(float mass, MotionState motion, PhysicsShape shape, vec2 intertia)
        {
            this->mass = mass;
            this->position = motion.position;
            this->quaternion = motion.quaternion;
            this->mass = mass;
            this->intertia = intertia;
        }
    
        vec2 position;
        quat quaternion;
    
        float mass;
        vec2 intertia;
    
        void CalulateLocalIntertia(float m, vec2 inter)
        {
        }
    };
};
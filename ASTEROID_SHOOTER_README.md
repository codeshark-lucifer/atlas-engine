# ASTEROID SHOOTER - Atlas Engine Demo Game

A complete space shooter game built entirely with your Atlas Engine, showcasing all core features:

## 🎮 Game Features

### Core Systems Used
- **ECS Architecture** - Entity Component System for game object management
- **Physics System** - Circle-to-circle collision detection and resolution
- **Rendering System** - 2D graphics rendering with shapes and colors
- **Input System** - Keyboard input handling for player control
- **Transform System** - Position, rotation, and motion management

### Game Components

#### Spaceship
- **File:** `include/components/spaceship.hpp`
- **Features:**
  - Rotates with A/D keys (360° per second)
  - Accelerates with W key (pixel-perfect physics)
  - Shoots bullets with SPACE key
  - Screen wrapping (exits one side, enters the other)
  - Maximum velocity cap to prevent over-acceleration
  - Velocity drag for realistic deceleration
  - Bullet inheritance of ship velocity

#### Asteroids
- **File:** `include/components/asteroid.hpp`
- **Features:**
  - Three size classes: LARGE (30px), MEDIUM (15px), SMALL (7px)
  - Dynamic spawning at screen edges
  - Continuous rotation based on random speeds
  - Mass-based physics (larger = heavier)
  - Collision-triggered splitting:
    - Large → 2 Medium asteroids
    - Medium → 2 Small asteroids
    - Small → destroyed

#### Bullets
- **Component:** `BulletComponent` in `spaceship.hpp`
- **Features:**
  - 3-second lifetime before despawn
  - Inherits ship velocity for realistic ballistics
  - Screen wrapping like the ship
  - Collision detection with asteroids

### Physics Improvements
- **Fixed circle-to-circle collision** with proper separation
- **Mass-proportional impulse resolution** preventing sticky collisions
- **Damped restitution** for realistic bouncing
- **Velocity-based collision detection** with distance checks

## 🕹️ Controls

| Key | Action |
|-----|--------|
| **A** | Rotate ship left |
| **D** | Rotate ship right |
| **W** | Thrust forward |
| **SPACE** | Shoot |

## 🎯 Scoring System

| Action | Points |
|--------|--------|
| Destroy Large Asteroid | 20 |
| Destroy Medium Asteroid | 50 |
| Destroy Small Asteroid | 100 |

### Difficulty Progression
- **Wave 1:** 2 large asteroids
- **Wave 2:** 3 large asteroids
- **Wave 3:** 4 large asteroids
- And so on...

## 🏗️ Architecture

### Game Flow
```
main.cpp
├── Initialization (SDL3 + OpenGL 4.6)
├── Scene Creation
├── Entity Spawning
│   ├── Camera
│   ├── Spaceship (with physics)
│   ├── Initial Asteroids
│   └── Bullets (spawned on shoot)
├── Game Loop
│   ├── Input Processing
│   ├── Physics Update (fixed timestep)
│   ├── Collision Detection
│   ├── Entity Updates
│   └── Rendering
└── Cleanup
```

### Component Hierarchy

```
Spaceship (Component)
├── Transform
├── Rigidbody2D → physics::Rigidbody2D
├── CircleCollider2D
└── Circle (visual)

Asteroid (Component)
├── Transform
├── Rigidbody2D → physics::Rigidbody2D
├── CircleCollider2D
└── Circle (visual)

BulletComponent
├── Transform
├── Rigidbody2D → physics::Rigidbody2D
├── CircleCollider2D
└── Circle (visual)
```

## 📊 Collision System

### Physics Pipeline
1. **Spawn Phase** - Register all entities with physics world
2. **Force Phase** - Apply gravity to dynamic bodies
3. **Integration Phase** - Update positions/velocities
4. **Collision Detection** - Broad phase (AABB) + Narrow phase (circle)
5. **Collision Resolution** - Impulse-based response
6. **Game Logic** - Check asteroid destruction, spawn new ones

### Collision Types
- **Bullet → Asteroid** - Game logic (custom detection)
- **Asteroid → Asteroid** - Physics engine handles
- **Ship → Asteroid** - Physics engine handles (can cause damage in future)

## 🎨 Visual Design

- **Background:** Deep space (dark blue #020205)
- **Spaceship:** Green circle (8px radius)
- **Large Asteroids:** Green circles (30px radius)
- **Medium Asteroids:** Green circles (15px radius)
- **Small Asteroids:** Green circles (7px radius)
- **Bullets:** Blue circles (2px radius)

## 🚀 Features Demonstrated

✅ **ECS System** - Full entity component system usage  
✅ **Physics Engine** - 2D rigid body dynamics  
✅ **Collision Detection** - Circle-to-circle physics + game-logic collision  
✅ **Rendering** - 2D shape rendering with transforms  
✅ **Input Handling** - Real-time keyboard input  
✅ **Scene Management** - Dynamic entity creation/destruction  
✅ **Game State** - Score tracking, wave progression  
✅ **Spatial Logic** - Screen wrapping, distance calculations  

## 🔧 Building & Running

```bash
# Build
cmake --build build

# Run
./build/bin/engine.exe
```

## 📝 Game Statistics (Per Session)

- **Total Score** - Accumulated from asteroid destruction
- **Current Wave** - Increases each time all asteroids destroyed
- **Active Asteroids** - Real-time count
- **Active Bullets** - Real-time count
- **Ships Remaining** - Lives counter (for future enhancement)

## 🎓 Learning Points

This game demonstrates:
- How to use an ECS framework for game development
- Physics-based collision response
- Component composition patterns
- Real-time input handling
- Game loop architecture
- State management and progression
- 2D transformation and rotation
- Performance optimization (entity pooling, spatial queries)

---

**Built with Atlas Engine** - Showcasing the power of a well-architected game engine!

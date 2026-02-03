# Pixel Art Texture Support Guide

## Overview
The improved `Texture2D` class now supports pixel art rendering with crisp, sharp edges through the `pixelArt` parameter.

## Key Changes

### Constructor Parameter
```cpp
Texture2D(const std::string &path, const Type &type = Type::DIFFUSE, bool gamma = true, bool pixelArt = false)
```

- **pixelArt** (default: false): Toggle for pixel-perfect rendering
  - `true`: Uses GL_NEAREST filtering (crisp, sharp pixel edges - ideal for pixel art)
  - `false`: Uses GL_LINEAR filtering (smooth, blurred edges - ideal for photographs/high-res art)

## Usage Examples

### Creating Pixel Art Textures

#### Example 1: Load a pixel art sprite with crisp edges
```cpp
auto texture = std::make_shared<Texture2D>("assets/textures/player_sprite.png", Type::DIFFUSE, false, true);
//                                                                                                      ^^^^
//                                                                            pixelArt = true (enabled)
```

#### Example 2: Load a standard high-resolution texture (default smooth rendering)
```cpp
auto texture = std::make_shared<Texture2D>("assets/textures/background.png");
// pixelArt defaults to false, uses smooth GL_LINEAR filtering
```

#### Example 3: Load with custom gamma settings
```cpp
auto texture = std::make_shared<Texture2D>("assets/textures/sprite.png", Type::DIFFUSE, false, true);
//                                                                                      ^^^^^ ^^^^
//                                                                         gamma = false, pixelArt = true
```

### Toggling Pixel Art Mode at Runtime

```cpp
auto texture = std::make_shared<Texture2D>("assets/textures/sprite.png");

// Later, if you want to toggle to pixel art mode:
texture->SetPixelArtMode(true);  // Enable pixel-perfect rendering

// Or disable it:
texture->SetPixelArtMode(false); // Switch back to smooth rendering
```

## Technical Details

### What Changes When pixelArt = true?

1. **Texture Filtering**
   - MIN_FILTER: GL_NEAREST (no interpolation between pixels)
   - MAG_FILTER: GL_NEAREST (no smoothing when scaled up)

2. **Mipmaps**
   - Mipmaps are NOT generated for pixel art textures
   - This prevents the blurring that occurs at different zoom levels

3. **Wrapping Mode**
   - GL_CLAMP_TO_EDGE (same as smooth mode)
   - Prevents texture wrapping at edges

### What Changes When pixelArt = false (Default)?

1. **Texture Filtering**
   - MIN_FILTER: GL_LINEAR_MIPMAP_LINEAR
   - MAG_FILTER: GL_LINEAR

2. **Mipmaps**
   - Mipmaps ARE generated for smoother downsampling

## Best Practices

1. **Always enable pixelArt mode for:**
   - Pixel art sprites
   - Retro-style games
   - Low-resolution textures that need to maintain crispness

2. **Use default mode for:**
   - High-resolution photographs
   - Modern game assets
   - Textures designed for linear filtering

3. **Performance**
   - Pixel art textures with GL_NEAREST are actually slightly faster than GL_LINEAR
   - No mipmap generation saves a tiny bit of VRAM

## Integration with Your Player

Example: Using pixel art for your player sprite

```cpp
class Player : public Component
{
public:
    Player(Scene *scene) { this->scene = scene; }

    void Start() override
    {
        transform = entity->GetComponent<Transform>();
        
        // Create pixel art sprite
        auto spriteTexture = std::make_shared<Texture2D>(
            "assets/textures/player_pixel_art.png", 
            Type::DIFFUSE, 
            false,
            true  // pixelArt = true
        );
        
        // Add a sprite component with the pixel art texture
        auto sprite = entity->AddComponent<Circle>(20.0f, 32, RED, spriteTexture);
    }

    // ... rest of implementation
};
```

## Notes

- The class is backward compatible; existing code without the pixelArt parameter will work as before
- SetPixelArtMode() checks if the mode is already set to avoid unnecessary GPU calls
- Always bind the texture before calling SetPixelArtMode() if needed

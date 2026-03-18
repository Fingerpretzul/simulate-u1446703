# Architecture

## Project Structure

```
simulate-u1446703/
├── CMakeLists.txt          # Build configuration
├── include/
│   ├── physics.h           # Core physics types: Vec2, Ball, Wall, PhysicsWorld
│   └── renderer.h          # SDL3 renderer wrapper
├── src/
│   ├── physics.cpp         # Physics engine implementation
│   ├── renderer.cpp        # SDL3 rendering (circles, lines)
│   └── main.cpp            # Entry point, scene setup, main loop
├── tests/
│   └── test_physics.cpp    # 20 unit tests for physics engine
├── docs/
│   ├── ARCHITECTURE.md     # This file
│   └── BUILD.md            # Build instructions
└── build/                  # CMake build directory (gitignored)
```

## Design Decisions

### Physics Engine (physics.h / physics.cpp)

- **Substep integration**: Each frame is divided into N substeps (default 8) to prevent tunneling.
- **Iterative constraint solving**: Each substep runs 4 iterations of collision resolution.
- **Position-based correction + impulse**: Overlapping objects are first separated positionally, then velocity impulses are applied. This prevents the "jitter→explode" failure mode.
- **O(n²) collision**: Acceptable for ~1000 balls (~500K checks). A spatial hash grid would be needed for larger counts.
- **Sleep threshold**: Balls below a velocity threshold are zeroed out to help convergence.

### Renderer (renderer.h / renderer.cpp)

- Circles drawn as triangle fans via `SDL_RenderGeometry` (16 segments each).
- Balls colored by speed: blue (slow) → green (medium) → red (fast).
- Walls drawn as white lines.

### Scene Setup (main.cpp)

- Rectangular container with two angled shelves for visual interest.
- 1000 balls placed in a grid with slight random offsets and velocities.
- Restitution configurable via command-line argument: `./simulator [restitution]`

## Key Classes

| Class | File | Purpose |
|-------|------|---------|
| `Vec2` | physics.h | 2D vector math (add, sub, dot, normalize, etc.) |
| `Ball` | physics.h | Circular body with position, velocity, radius, mass |
| `Wall` | physics.h | Immovable line segment with outward normal |
| `PhysicsConfig` | physics.h | Simulation parameters (gravity, restitution, substeps, etc.) |
| `PhysicsWorld` | physics.h/cpp | Owns balls+walls, runs simulation step |
| `Renderer` | renderer.h/cpp | SDL3 window, drawing, event handling |

## Collision Resolution Algorithm

### Ball-Wall
1. Find closest point on wall segment to ball center
2. If distance < radius: push ball out along normal, reflect velocity with restitution

### Ball-Ball
1. Check center-to-center distance vs sum of radii
2. If overlapping: push apart proportional to inverse mass
3. Apply impulse along collision normal with restitution
4. Apply tangential friction (Coulomb model)

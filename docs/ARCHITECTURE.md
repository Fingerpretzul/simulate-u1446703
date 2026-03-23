# Architecture

## Project Structure

```
simulate-u1446703/
├── CMakeLists.txt          # Build configuration
├── include/
│   ├── physics.h           # Core physics types: Vec2, Ball, Wall, PhysicsWorld, SpatialGrid
│   └── renderer.h          # SDL3 renderer wrapper
├── src/
│   ├── physics.cpp         # Physics engine implementation (CCD, spatial grid, solvers)
│   ├── renderer.cpp        # SDL3 rendering (circles, lines, HUD, screenshots)
│   └── main.cpp            # Entry point, scene setup, main loop, headless mode
├── tests/
│   └── test_physics.cpp    # 35 unit tests for physics engine
├── screenshots/            # BMP screenshots from headless runs (gitignored)
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
- **CCD (continuous collision detection)**: After each position integration, a swept-circle-vs-line test checks whether any ball crossed a wall during the substep. If so, the ball is clipped back to the wall surface and its velocity is reflected. This catches extreme-speed tunneling that substeps alone might miss.
- **Correct ball-ball restitution response**: The pair solver computes relative velocity using the standard normal direction (A→B) so approaching balls receive the intended restitution impulse.
- **Endpoint-aware wall contacts**: Exact wall-endpoint overlaps distinguish point contacts from segment interiors, allowing correct reflection at corners.
- **Spatial hash grid with generation counter**: Ball-ball collision uses `SpatialGrid` that buckets balls into uniform cells. Only pairs sharing a cell are tested, reducing average cost from O(n²) to ~O(n). Cell size is auto-tuned to 2× the max ball radius. The grid uses a generation counter for O(1) `clear()` — cells with stale generation are treated as empty on access. Duplicate pairs from overlapping cells are handled by idempotency.
- **Sleep threshold**: Balls below a velocity threshold are zeroed out to help convergence.
- **Settling invariant coverage**: Tests verify that restitution affects decay time but not the final packed footprint — at 50, 120, 500, and 1000 ball scales.
- **Full-scale 1000-ball tests**: No-overlap and settling-invariance tests at the actual production ball count (1000 balls with shelves).

### Renderer (renderer.h / renderer.cpp)

- Circles drawn as triangle fans via `SDL_RenderGeometry` (16 segments each).
- Precomputed trig tables and static vertex buffer eliminate per-ball heap allocation.
- Balls colored by speed: blue (slow) → green (medium) → red (fast).
- Walls drawn as white lines.
- FPS + ball count HUD overlay via `SDL_RenderDebugText` (scaled 2×).
- `saveScreenshot()` captures the framebuffer to BMP via `SDL_RenderReadPixels` + `SDL_SaveBMP`.

### Scene Setup (main.cpp)

- Rectangular container with two angled shelves for visual interest.
- 1000 balls placed in a grid with slight random offsets and velocities.
- Restitution configurable via command-line argument: `./simulator [restitution]`
- **Headless mode**: `./simulator --headless [restitution] [frames] [prefix]` runs for a fixed number of frames using the offscreen video driver, saving BMP screenshots at key moments (initial, bouncing, settling, settled).

## Key Classes

| Class | File | Purpose |
|-------|------|---------|
| `Vec2` | physics.h | 2D vector math (add, sub, dot, normalize, etc.) |
| `Ball` | physics.h | Circular body with position, velocity, radius, mass |
| `Wall` | physics.h | Immovable line segment with outward normal |
| `PhysicsConfig` | physics.h | Simulation parameters (gravity, restitution, substeps, etc.) |
| `SpatialGrid` | physics.h/cpp | Spatial hash grid with generation counter for broadphase |
| `CellKey` / `CellKeyHash` | physics.h | Grid cell coordinate + hash for unordered_map |
| `CellData` | physics.h | Per-cell ball indices + generation stamp |
| `PhysicsWorld` | physics.h/cpp | Owns balls+walls, runs simulation step with CCD |
| `Renderer` | renderer.h/cpp | SDL3 window, drawing, event handling, FPS HUD, screenshots |

## Collision Resolution Algorithm

### Ball-Wall (with CCD)
1. **CCD pass** (in `integratePositions`): For each ball, swept-circle-vs-line test against all walls. If the ball's trajectory this substep crosses a wall, clip position back to the contact point and reflect velocity.
2. **Overlap resolution** (in `solveBallWallCollisions`): Find closest point on wall segment to ball center. Distinguish segment interiors from clamped endpoints. If distance < radius: push ball out along normal, reflect velocity with restitution.

### Ball-Ball
1. Populate spatial hash grid with all ball bounding boxes
2. For each unique pair sharing a grid cell, check center-to-center distance vs sum of radii
3. If overlapping: push apart proportional to inverse mass
4. Compute B-relative-to-A velocity along the A→B collision normal and apply the restitution impulse only while the pair is closing
5. Apply tangential friction (Coulomb model)

## Performance

| Metric | Value |
|--------|-------|
| 1000-ball physics step | ~0.8–0.9 ms/frame |
| Target frametime (30 FPS) | 33 ms |
| Headroom | ~30× |
| Spatial grid clear | O(1) via generation counter |
| Ball-ball broadphase | O(n) average via spatial hash |

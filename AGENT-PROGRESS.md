# Agent Progress Log

## Iteration 1 — 2026-03-18 (Claude Opus 4.6)

### What was done
Built the entire 2D physics simulator from scratch:

1. **Dependencies**: Installed SDL3 3.4.2 and cmake 4.2.3 via homebrew
2. **Project structure**: CMakeLists.txt with three targets (physics_lib, simulator, tests)
3. **Physics engine** (`include/physics.h`, `src/physics.cpp`):
   - `Vec2` math helper
   - `Ball` (circular body with pos, vel, radius, mass)
   - `Wall` (immovable line segment)
   - `PhysicsWorld` with substep integration + iterative constraint solving
   - Ball-wall collision: closest-point-on-segment + push-out + restitution impulse
   - Ball-ball collision: position correction (inverse mass weighted) + impulse + friction
   - Sleep threshold to zero out near-stopped balls
4. **Renderer** (`include/renderer.h`, `src/renderer.cpp`):
   - SDL3 window + renderer
   - Balls as triangle-fan circles, colored by speed
   - Walls as white lines
5. **Scene** (`src/main.cpp`):
   - 1000 balls in a rectangular container with two angled shelves
   - Restitution configurable via CLI argument
6. **Tests** (`tests/test_physics.cpp`): 20 tests, all passing:
   - Vec2 math (6 tests)
   - Gravity (2 tests)
   - Ball-wall collision (3 tests)
   - Ball-ball collision (3 tests)
   - Restitution behavior (2 tests)
   - Energy dissipation (1 test)
   - Settling/stacking (1 test)
   - Wall normals (2 tests)
7. **Visual verification**: Launched simulator with restitution 0.0, 0.3, 0.9 — all ran without crashes

### Current state
- Code builds cleanly, 20/20 tests pass
- Simulator runs with ~1000 balls, configurable restitution
- All source files are well-commented for agent handoff

### What the next iteration should focus on
- Run the simulator and visually confirm balls don't overlap/phase through walls
- Performance testing — is 1000 balls smooth at 60fps?
- Consider spatial hash grid if performance is an issue
- Add more edge-case tests (e.g., balls at exact wall corners)
- Add FPS counter or performance overlay
- See TASKS.md for full list of future work

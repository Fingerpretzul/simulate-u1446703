# Tasks

## Completed — Iteration 1 (2026-03-18)

- [x] Install SDL3 and cmake via homebrew
- [x] Set up CMake project structure (src/, include/, tests/)
- [x] Implement core physics engine (Ball, Wall, PhysicsWorld)
- [x] Implement SDL3 renderer and scene setup (~1000 balls)
- [x] Write 20 unit tests for physics engine — all passing
- [x] Build and verify: simulator runs, tests pass
- [x] Visual verification: simulator launched with restitution 0.0, 0.3, 0.9
- [x] Documentation: ARCHITECTURE.md, BUILD.md

## Known Issues / Future Work

- [ ] **Spatial partitioning**: O(n²) ball-ball collision is fine for 1000 balls but could use a spatial hash grid for larger counts
- [ ] **Performance profiling**: With 1000 balls, 8 substeps, and 4 solver iterations, the sim does 32 passes of collision per frame. May need optimization if targeting 60fps consistently
- [ ] **Wall thickness**: Very fast balls could still tunnel if substeps are too low. Consider CCD (continuous collision detection) for extreme cases
- [ ] **Visual polish**: Could add ball outlines, fps counter, restitution slider UI
- [ ] **Settling verification**: Add automated test that different restitution values produce same final "settled" volume/height
- [ ] **Screen recording**: Take a screenshot/recording to document visual behavior

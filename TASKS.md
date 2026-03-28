# Tasks

## Completed — Iteration 11 (2026-03-28)

- [x] Implement two-phase sleep system to fix gravity-vs-sleep issue
  - Phase 1 (never-active): counter-based delay gives gravity `sleepDelay` substeps to build velocity
  - Phase 2 (previously-active): instant sleep kills solver micro-vibrations
  - Added `sleepCounter` and `hasBeenActive` fields to `Ball` struct
  - Added `sleepDelay` config parameter (default 8, matching substeps)
- [x] Add 3 new tests (43→46): gravity_wakes_zero_velocity_balls, sleep_counter_resets_on_fast_motion, settling_with_zero_initial_velocity
- [x] Increase shelf scene settling test frames (4000→5000) for counter-based sleep convergence
- [x] Verify all 46/46 tests pass
- [x] Run headless simulations at restitution 0.0, 0.3, 0.9 — all produce valid results
- [x] Update documentation (ARCHITECTURE.md, BUILD.md, TASKS.md, AGENT-PROGRESS.md)

## Completed — Iteration 10 (2026-03-25)

- [x] Diagnose 5 failing physics settling tests (38/43 passing at start of iteration)
- [x] Root cause: iteration 9 moved `applySleepThreshold()` from per-substep to per-frame, allowing energy accumulation in dense packings via constraint solver residuals
- [x] Fix: restore `applySleepThreshold()` to run inside the substep loop (per-substep), while also keeping it at frame-end
- [x] Verify all 43/43 tests pass after fix
- [x] Verify headless simulation works (KE decreases from ~850M peak to ~37M after 200 frames)
- [x] Update documentation (ARCHITECTURE.md, TASKS.md, AGENT-PROGRESS.md)

## Completed — Iteration 9 (2026-03-23)

- [x] Add `BallColor` struct and `color` field to `Ball` for persistent ball coloring
- [x] Update renderer to use ball's assigned color when `hasColor` is true
- [x] Implement `csv_io.h`/`csv_io.cpp`: `loadSceneFromCSV()`, `saveSceneToCSV()`, `splitCSVLine()`
- [x] CSV format supports balls (type,x,y,radius,r,g,b) and walls (type,x1,y1,x2,y2), comments, headers
- [x] Add `--load-csv` and `--save-csv` CLI options to simulator (both headless and interactive)
- [x] Create `color_assign` tool: loads CSV + BMP, simulates, samples image at final positions, writes colored CSV
- [x] Add `csv_io_lib` static library and `color_assign` executable to CMakeLists.txt
- [x] Add 8 new tests (35→43): CSV split, load/save roundtrip, comments, missing file, ball color default
- [x] Build, verify 43/43 tests pass, headless CSV workflow verified end-to-end
- [x] Update documentation (ARCHITECTURE.md, BUILD.md, AGENTS.md, AGENT-PROGRESS.md, TASKS.md)

## Completed — Iteration 8 (2026-03-23)

- [x] Rebuild SDL3 with video support enabled (`SDL_VIDEO=ON`)
- [x] Add `--headless` mode with BMP screenshot capture at key simulation moments
- [x] Add `saveScreenshot()` to Renderer using `SDL_RenderReadPixels` + `SDL_SaveBMP`
- [x] Implement CCD (continuous collision detection) — swept-circle-vs-line in `integratePositions()`
- [x] Add 2 CCD tests: `ccd_prevents_fast_ball_tunneling`, `ccd_works_with_angled_walls`
- [x] Add 1000-ball full-scale tests: `full_scale_1000_balls_no_overlap_after_settling`, `full_scale_1000_balls_restitution_invariance`
- [x] Optimize spatial grid: O(1) `clear()` via generation counter (replaces per-cell iteration)
- [x] Capture screenshots at restitution 0.0, 0.3, 0.9 — verified rendering and settling behavior
- [x] Build, verify 35/35 tests pass, performance still 0.8–0.9 ms/frame
- [x] Update documentation (ARCHITECTURE.md, BUILD.md, AGENT-PROGRESS.md, TASKS.md)

## Completed — Iteration 7 (2026-03-23)

- [x] Build and verify 29/29 baseline tests pass
- [x] Remove pair-dedup `unordered_set` — rely on idempotent resolution instead (0.8 ms/frame, down from 2.4 ms)
- [x] Optimize renderer: precomputed trig tables + static vertex buffer (no per-ball heap allocation)
- [x] Add `large_scale_no_overlap_after_settling` test (500 balls, no overlaps, all contained)
- [x] Add `large_scale_restitution_preserves_packed_size` test (500 balls, settling invariance)
- [x] Build, verify 31/31 tests pass, confirm simulator startup (headless)
- [x] Update documentation (ARCHITECTURE.md, BUILD.md, AGENT-PROGRESS.md, TASKS.md)

## Completed — Iteration 6 (2026-03-23)

- [x] Build and verify 25/25 baseline tests pass
- [x] Implement spatial hash grid for O(n) ball-ball collision detection
- [x] Add FPS counter + ball count HUD overlay to renderer
- [x] Add 4 new tests: glancing endpoint impact, dense column stack, spatial grid correctness, 1000-ball performance benchmark
- [x] Build, verify 29/29 tests pass, confirm simulator startup
- [x] Update documentation (ARCHITECTURE.md, BUILD.md, AGENT-PROGRESS.md, TASKS.md)

## Completed — Iteration 5 (2026-03-22)

- [x] Rebuild and rerun the simulator test suite to re-establish a clean baseline before extending settling coverage
- [x] Convert the shelf-geometry settling probe into an automated mixed-radius regression
- [x] Rebuild, rerun tests, and relaunch the simulator in this environment for verification
- [x] Update documentation and append the handoff details to `AGENT-PROGRESS.md`

## Completed — Iteration 4 (2026-03-22)

- [x] Rebuild and rerun the simulator test suite to establish a clean baseline before touching wall contacts
- [x] Add explicit regression coverage for wall-endpoint overlap resolution and corner-joint containment
- [x] Fix the wall solver so exact endpoint contacts use a point-contact normal instead of only the segment normal
- [x] Rebuild, rerun tests, and relaunch the simulator in this environment for verification
- [x] Update documentation and append the handoff details to `AGENT-PROGRESS.md`

## Completed — Iteration 3 (2026-03-22)

- [x] Rebuild and rerun the test suite to re-establish a clean baseline before physics changes
- [x] Investigate ball-ball collision response to confirm whether restitution impulses are being applied correctly
- [x] Fix the ball-ball impulse math so approaching balls exchange momentum according to restitution
- [x] Strengthen collision regression coverage to assert post-impact velocities, not just separation
- [x] Rebuild, rerun tests, and relaunch the simulator in this environment for verification
- [x] Update documentation and append the handoff details to `AGENT-PROGRESS.md`

## Completed — Iteration 2 (2026-03-22)

- [x] Rebuild the project and rerun the existing test suite to establish a clean baseline
- [x] Probe settling behavior across multiple restitution values to verify whether packed resting height already matches
- [x] Add an automated regression test that proves different restitution values converge to the same settled packing footprint
- [x] Update project documentation to describe the new settling-invariance verification
- [x] Launch the simulator again for manual verification after the test/documentation changes
- [x] Append this iteration's handoff details to `AGENT-PROGRESS.md`

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

- [x] ~~**Spatial partitioning**: O(n²) ball-ball collision~~ — Implemented spatial hash grid in iteration 6
- [x] ~~**Performance profiling**~~ — 1000-ball step measured at ~0.8 ms/frame avg; idempotent spatial grid, no hash-set overhead
- [x] ~~**FPS counter**~~ — Added FPS + ball count HUD overlay
- [x] ~~**Collision edge cases (glancing, dense stacks)**~~ — Added glancing endpoint, dense column, and spatial grid correctness tests
- [x] ~~**Wall thickness / CCD**~~ — Implemented swept-circle CCD in iteration 8; fast balls no longer tunnel through walls
- [x] ~~**Settling verification**~~ — Full 1000-ball no-overlap and settling-invariance tests added in iteration 8
- [x] ~~**Pair dedup optimization**~~ — Removed hash-set; idempotent resolution handles duplicates (iteration 7)
- [x] ~~**SDL3 video support**~~ — Rebuilt SDL3 with `SDL_VIDEO=ON` in iteration 8; offscreen rendering + BMP screenshots now work
- [x] ~~**Screen recording**~~ — Headless screenshot capture implemented in iteration 8; screenshots at 3 restitution values saved
- [x] ~~**Spatial grid clear optimization**~~ — Generation counter replaces per-cell iteration (iteration 8)
- [x] ~~**CSV scene I/O**~~ — Load/save balls+walls from CSV; supports colors, comments, headers (iteration 9)
- [x] ~~**Color assignment tool**~~ — `color_assign` maps final ball positions to BMP image colors (iteration 9)
- [x] ~~**Gravity-vs-sleep fix**~~ — Two-phase sleep system: counter-based delay for rest-start balls, instant sleep for active balls (iteration 11)
- [ ] **Visual polish**: Ball outlines, restitution slider UI, color scheme options
- [ ] **SIMD vectorization**: Consider SIMD for the physics step inner loops
- [ ] **Interactive display**: Need an environment with a real display server (X11/Wayland) for interactive mode
- [ ] **CSV scene generator**: Tool to procedurally generate initial scene CSVs with various layouts
- [ ] **PNG image support**: `color_assign` currently requires BMP; could add PNG via SDL_image
- [ ] **High-restitution residual bouncing**: With restitution ≥0.9, some balls can maintain stable orbits (gravity input = bounce loss). Consider adaptive damping or geometric orbit detection.

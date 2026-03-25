# Build Instructions

## Prerequisites

- C++17 compiler (GCC, Clang, or Apple Clang)
- CMake 3.16+
- SDL3 (built with `SDL_VIDEO=ON` for rendering support)

## Install Dependencies

**macOS (Homebrew)**:
```bash
brew install cmake sdl3
```

**Linux (from source)**:
```bash
# Build SDL3 with video support
git clone https://github.com/libsdl-org/SDL.git --branch release-3.2.4
cd SDL && mkdir build && cd build
cmake .. -DSDL_VIDEO=ON -DSDL_OFFSCREEN=ON
cmake --build . -j$(nproc)
sudo cmake --install .
```

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run Tests

```bash
./build/tests
```

Expected: **43/43 tests pass**. Includes:
- Vec2 math (6), gravity (2), ball-wall (5), ball-ball (4)
- Restitution behavior (4), energy/settling (3), wall normals (2)
- Collision edge cases (3), CCD (2), performance benchmark (1)
- Large-scale 500-ball (2), full-scale 1000-ball (2)
- CSV I/O (6), ball color (2)

## Run Simulator

### Interactive mode
```bash
./build/simulator [restitution]
```

### Headless mode (offscreen rendering + screenshots)
```bash
./build/simulator --headless [restitution] [frames] [screenshot_prefix]
```

- `restitution`: float 0.0–1.0 (default 0.3)
- `frames`: number of frames to simulate (default 600)
- `screenshot_prefix`: path/prefix for BMP files (default "screenshot")

### CSV scene loading and saving
```bash
# Load scene from CSV
./build/simulator --load-csv scene.csv [restitution]

# Save final positions to CSV
./build/simulator --headless --save-csv output.csv [restitution] [frames]

# Load from CSV, simulate, save result
./build/simulator --headless --load-csv input.csv --save-csv output.csv 0.3 600
```

### Color assignment tool
```bash
./build/color_assign <input.csv> <image.bmp> <output.csv> [restitution] [frames]
```

Runs the simulation on the input scene, then assigns each ball a color based on the pixel in the BMP image at the ball's final position. Writes the original scene with new colors to the output CSV.

### Controls (interactive mode)
- **ESC** or **Q** to quit
- FPS and ball count displayed in top-left corner

## Examples

```bash
./build/simulator              # Interactive, restitution 0.3
./build/simulator 0.0          # Interactive, perfectly inelastic
./build/simulator 0.9          # Interactive, very bouncy

# Headless: 800 frames, save screenshots to ./screenshots/
mkdir -p screenshots
./build/simulator --headless 0.3 800 screenshots/sim_r03
# Produces: sim_r03_initial.bmp, sim_r03_bouncing.bmp,
#           sim_r03_settling.bmp, sim_r03_settled.bmp

# CSV workflow: generate scene → simulate → color from image
./build/simulator --headless --save-csv initial.csv 0.3 600 screenshots/sim
./build/color_assign initial.csv screenshots/sim_settled.bmp colored.csv 0.3 600
./build/simulator --headless --load-csv colored.csv 0.3 10 screenshots/colored
```

## Performance

- 1000-ball physics step: ~0.8–0.9 ms/frame
- 30 FPS budget: 33 ms → ~30× headroom for physics alone
- Spatial hash grid: O(n) broadphase with O(1) clear via generation counter
- CCD: negligible overhead (one dot product per ball-wall pair per substep)

# Build Instructions

## Prerequisites

- macOS (tested on macOS 15 / Apple Silicon)
- Homebrew
- C++17 compiler (Apple Clang 17+)

## Install Dependencies

```bash
brew install cmake sdl3
```

## Build

```bash
mkdir -p build && cd build
cmake ..
cmake --build .
```

## Run Tests

```bash
cd build
./tests
```

Expected: 20/20 tests pass.

## Run Simulator

```bash
cd build
./simulator [restitution]
```

- `restitution`: float 0.0–1.0 (default 0.3)
  - `0.0` = perfectly inelastic, balls stop quickly
  - `0.3` = default, nice settling behavior
  - `0.9` = very bouncy, takes longer to settle
- Press **ESC** or **Q** to quit

## Examples

```bash
./simulator          # Default restitution 0.3
./simulator 0.0      # Balls stop almost immediately
./simulator 0.9      # Very bouncy
```

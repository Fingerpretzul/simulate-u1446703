cmake_minimum_required(VERSION 3.20)
project(PhysicsSim VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find SDL3
find_package(SDL3 REQUIRED)

# ── Physics library (no SDL dependency) ──────────────────────────────
add_library(physics_lib STATIC
    src/physics.cpp
)
target_include_directories(physics_lib PUBLIC include)

# ── Main simulator executable ────────────────────────────────────────
add_executable(simulator
    src/main.cpp
    src/renderer.cpp
)
target_include_directories(simulator PRIVATE include)
target_link_libraries(simulator PRIVATE physics_lib SDL3::SDL3)

# ── Unit tests (no SDL needed) ───────────────────────────────────────
add_executable(tests
    tests/test_physics.cpp
)
target_include_directories(tests PRIVATE include)
target_link_libraries(tests PRIVATE physics_lib)

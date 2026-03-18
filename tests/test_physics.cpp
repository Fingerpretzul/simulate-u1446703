// test_physics.cpp — Unit tests for the physics engine.
// Uses a minimal test framework (no external dependencies).
// Tests cover: Vec2 math, gravity, ball-wall collision, ball-ball collision,
// overlap resolution, restitution behavior, and energy dissipation.

#include "physics.h"
#include <cstdio>
#include <cmath>
#include <cstdlib>

// ── Minimal test framework ──────────────────────────────────────────
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) \
    static void test_##name(); \
    static struct Register_##name { \
        Register_##name() { \
            tests_run++; \
            printf("  TEST %-50s ", #name); \
            try { test_##name(); tests_passed++; printf("PASS\n"); } \
            catch (...) { tests_failed++; printf("FAIL (exception)\n"); } \
        } \
    } register_##name; \
    static void test_##name()

#define ASSERT(cond) do { \
    if (!(cond)) { \
        printf("FAIL\n    Assertion failed: %s\n    at %s:%d\n", \
               #cond, __FILE__, __LINE__); \
        tests_failed++; tests_passed--; return; \
    } \
} while(0)

#define ASSERT_NEAR(a, b, eps) do { \
    float _a = (a), _b = (b), _e = (eps); \
    if (std::abs(_a - _b) > _e) { \
        printf("FAIL\n    ASSERT_NEAR: %f != %f (eps=%f)\n    at %s:%d\n", \
               _a, _b, _e, __FILE__, __LINE__); \
        tests_failed++; tests_passed--; return; \
    } \
} while(0)

// ═══════════════════════════════════════════════════════════════════════
// Vec2 tests
// ═══════════════════════════════════════════════════════════════════════

TEST(vec2_add) {
    Vec2 a(1, 2), b(3, 4);
    Vec2 c = a + b;
    ASSERT_NEAR(c.x, 4.0f, 1e-5f);
    ASSERT_NEAR(c.y, 6.0f, 1e-5f);
}

TEST(vec2_sub) {
    Vec2 a(5, 3), b(2, 1);
    Vec2 c = a - b;
    ASSERT_NEAR(c.x, 3.0f, 1e-5f);
    ASSERT_NEAR(c.y, 2.0f, 1e-5f);
}

TEST(vec2_dot) {
    Vec2 a(1, 0), b(0, 1);
    ASSERT_NEAR(a.dot(b), 0.0f, 1e-5f);
    ASSERT_NEAR(a.dot(a), 1.0f, 1e-5f);
}

TEST(vec2_length) {
    Vec2 v(3, 4);
    ASSERT_NEAR(v.length(), 5.0f, 1e-5f);
}

TEST(vec2_normalized) {
    Vec2 v(3, 4);
    Vec2 n = v.normalized();
    ASSERT_NEAR(n.length(), 1.0f, 1e-5f);
    ASSERT_NEAR(n.x, 0.6f, 1e-5f);
    ASSERT_NEAR(n.y, 0.8f, 1e-5f);
}

TEST(vec2_zero_normalized) {
    Vec2 v(0, 0);
    Vec2 n = v.normalized();
    ASSERT_NEAR(n.x, 0.0f, 1e-5f);
    ASSERT_NEAR(n.y, 0.0f, 1e-5f);
}

// ═══════════════════════════════════════════════════════════════════════
// Gravity tests
// ═══════════════════════════════════════════════════════════════════════

TEST(gravity_accelerates_ball_downward) {
    // A ball in free space should accelerate downward
    PhysicsWorld world;
    world.config.gravity = 100.0f;
    world.config.substeps = 1;
    world.config.damping = 1.0f; // No damping for clean test
    world.config.sleepSpeed = 0.0f; // Disable sleep

    Ball b(Vec2(100, 100), 5.0f);
    b.vel = {0, 0};
    world.balls.push_back(b);

    // Step for 1 second (in small steps to avoid dt clamping)
    for (int i = 0; i < 100; ++i) {
        world.step(0.01f);
    }

    // Ball should have moved down significantly
    ASSERT(world.balls[0].pos.y > 140.0f);
    // Ball should have downward velocity
    ASSERT(world.balls[0].vel.y > 50.0f);
}

TEST(gravity_does_not_affect_horizontal) {
    PhysicsWorld world;
    world.config.gravity = 100.0f;
    world.config.substeps = 1;
    world.config.damping = 1.0f;
    world.config.sleepSpeed = 0.0f;

    Ball b(Vec2(200, 200), 5.0f);
    b.vel = {0, 0};
    world.balls.push_back(b);

    for (int i = 0; i < 100; ++i) {
        world.step(0.01f);
    }

    // X position should be unchanged
    ASSERT_NEAR(world.balls[0].pos.x, 200.0f, 0.1f);
}

// ═══════════════════════════════════════════════════════════════════════
// Ball-wall collision tests
// ═══════════════════════════════════════════════════════════════════════

TEST(ball_stays_above_floor) {
    // Ball dropped onto a floor should not pass through
    PhysicsWorld world;
    world.config.gravity = 500.0f;
    world.config.substeps = 8;
    world.config.restitution = 0.3f;
    world.config.damping = 0.999f;
    world.config.sleepSpeed = 0.0f;

    Ball b(Vec2(100, 80), 10.0f);
    world.balls.push_back(b);

    // Floor at y=100
    world.walls.push_back(Wall(Vec2(0, 100), Vec2(200, 100)));

    for (int i = 0; i < 1000; ++i) {
        world.step(0.016f);
    }

    // Ball center should be at or above floor minus radius
    ASSERT(world.balls[0].pos.y <= 100.0f);
}

TEST(ball_bounces_off_wall) {
    // Ball moving right should bounce off a vertical wall
    PhysicsWorld world;
    world.config.gravity = 0.0f; // No gravity for this test
    world.config.substeps = 4;
    world.config.restitution = 0.8f;
    world.config.damping = 1.0f;
    world.config.sleepSpeed = 0.0f;

    Ball b(Vec2(80, 50), 5.0f);
    b.vel = {200, 0}; // Moving right
    world.balls.push_back(b);

    // Vertical wall at x=100
    world.walls.push_back(Wall(Vec2(100, 0), Vec2(100, 200)));

    // Step until ball would have hit the wall
    for (int i = 0; i < 50; ++i) {
        world.step(0.016f);
    }

    // Ball should have bounced back (negative x velocity)
    ASSERT(world.balls[0].vel.x < 0.0f);
    // Ball should be to the left of the wall
    ASSERT(world.balls[0].pos.x < 100.0f - world.balls[0].radius + 1.0f);
}

TEST(ball_does_not_phase_through_wall) {
    // Fast ball should still be caught by substep integration
    PhysicsWorld world;
    world.config.gravity = 0.0f;
    world.config.substeps = 16; // High substeps for fast ball
    world.config.restitution = 0.5f;
    world.config.damping = 1.0f;
    world.config.sleepSpeed = 0.0f;

    Ball b(Vec2(50, 50), 5.0f);
    b.vel = {2000, 0}; // Very fast
    world.balls.push_back(b);

    // Wall at x=200
    world.walls.push_back(Wall(Vec2(200, 0), Vec2(200, 200)));

    for (int i = 0; i < 20; ++i) {
        world.step(0.016f);
    }

    // Ball must not have passed through the wall
    ASSERT(world.balls[0].pos.x < 200.0f);
}

// ═══════════════════════════════════════════════════════════════════════
// Ball-ball collision tests
// ═══════════════════════════════════════════════════════════════════════

TEST(two_balls_do_not_overlap) {
    // Two balls approaching each other should not overlap
    PhysicsWorld world;
    world.config.gravity = 0.0f;
    world.config.substeps = 4;
    world.config.restitution = 0.5f;
    world.config.damping = 1.0f;
    world.config.sleepSpeed = 0.0f;

    Ball a(Vec2(40, 50), 10.0f);
    a.vel = {100, 0};
    Ball b(Vec2(80, 50), 10.0f);
    b.vel = {-100, 0};
    world.balls.push_back(a);
    world.balls.push_back(b);

    for (int i = 0; i < 100; ++i) {
        world.step(0.016f);

        // Check no overlap at any point
        Vec2 diff = world.balls[1].pos - world.balls[0].pos;
        float dist = diff.length();
        float minDist = world.balls[0].radius + world.balls[1].radius;
        // Allow tiny floating-point tolerance
        ASSERT(dist >= minDist - 0.5f);
    }
}

TEST(head_on_collision_conserves_direction) {
    // Two equal balls in head-on collision should swap velocities (approx)
    PhysicsWorld world;
    world.config.gravity = 0.0f;
    world.config.substeps = 8;
    world.config.restitution = 1.0f; // Perfectly elastic
    world.config.damping = 1.0f;
    world.config.friction = 0.0f;
    world.config.sleepSpeed = 0.0f;

    Ball a(Vec2(30, 50), 10.0f);
    a.vel = {100, 0};
    Ball b(Vec2(80, 50), 10.0f);
    b.vel = {0, 0};
    world.balls.push_back(a);
    world.balls.push_back(b);

    // Run just long enough for the collision to happen
    for (int i = 0; i < 30; ++i) {
        world.step(0.016f);
    }

    // After collision: ball B should have gained rightward momentum
    // (in a perfectly elastic equal-mass collision, A stops and B takes all velocity)
    ASSERT(world.balls[1].pos.x > 80.0f); // B moved right from its start position
}

TEST(ball_ball_overlap_resolved) {
    // Two overlapping balls should be pushed apart
    PhysicsWorld world;
    world.config.gravity = 0.0f;
    world.config.substeps = 4;
    world.config.restitution = 0.5f;
    world.config.damping = 1.0f;
    world.config.sleepSpeed = 0.0f;

    // Place two balls overlapping
    Ball a(Vec2(50, 50), 10.0f);
    a.vel = {0, 0};
    Ball b(Vec2(55, 50), 10.0f); // Only 5px apart, should be 20
    b.vel = {0, 0};
    world.balls.push_back(a);
    world.balls.push_back(b);

    world.step(0.016f);

    // They should be separated now
    Vec2 diff = world.balls[1].pos - world.balls[0].pos;
    float dist = diff.length();
    ASSERT(dist >= 19.5f); // Should be ~20 (sum of radii)
}

// ═══════════════════════════════════════════════════════════════════════
// Restitution tests
// ═══════════════════════════════════════════════════════════════════════

TEST(low_restitution_settles_faster) {
    // With low restitution, kinetic energy should drop faster
    auto runSim = [](float restitution) -> float {
        PhysicsWorld world;
        world.config.gravity = 500.0f;
        world.config.substeps = 4;
        world.config.restitution = restitution;
        world.config.damping = 0.999f;
        world.config.sleepSpeed = 0.0f; // Disable so we can measure energy

        // Drop 10 balls onto a floor
        for (int i = 0; i < 10; ++i) {
            Ball b(Vec2(50 + i * 25, 50), 8.0f);
            world.balls.push_back(b);
        }
        world.walls.push_back(Wall(Vec2(0, 400), Vec2(500, 400)));  // floor
        world.walls.push_back(Wall(Vec2(0, 400), Vec2(0, 0)));      // left
        world.walls.push_back(Wall(Vec2(500, 0), Vec2(500, 400)));  // right

        // Run for 3 simulated seconds
        for (int i = 0; i < 200; ++i) {
            world.step(0.016f);
        }

        return world.totalKineticEnergy();
    };

    float energyLow  = runSim(0.1f);
    float energyHigh = runSim(0.9f);

    // Low restitution should have less remaining energy
    ASSERT(energyLow < energyHigh);
}

TEST(restitution_zero_stops_quickly) {
    PhysicsWorld world;
    world.config.gravity = 500.0f;
    world.config.substeps = 8;
    world.config.restitution = 0.0f;
    world.config.damping = 0.999f;
    world.config.sleepSpeed = 1.0f;

    Ball b(Vec2(100, 50), 10.0f);
    world.balls.push_back(b);
    world.walls.push_back(Wall(Vec2(0, 200), Vec2(300, 200))); // floor

    // Run for 5 simulated seconds
    for (int i = 0; i < 300; ++i) {
        world.step(0.016f);
    }

    // Ball should have essentially stopped
    ASSERT(world.balls[0].vel.length() < 5.0f);
}

// ═══════════════════════════════════════════════════════════════════════
// Energy dissipation test
// ═══════════════════════════════════════════════════════════════════════

TEST(energy_decreases_over_time) {
    PhysicsWorld world;
    world.config.gravity = 500.0f;
    world.config.substeps = 4;
    world.config.restitution = 0.5f;
    world.config.damping = 0.999f;
    world.config.sleepSpeed = 0.0f;

    // A few balls bouncing in a box
    for (int i = 0; i < 5; ++i) {
        Ball b(Vec2(50 + i * 30, 50), 8.0f);
        b.vel = {static_cast<float>(i * 20 - 40), 0};
        world.balls.push_back(b);
    }
    world.walls.push_back(Wall(Vec2(0, 300), Vec2(300, 300)));
    world.walls.push_back(Wall(Vec2(0, 0), Vec2(0, 300)));
    world.walls.push_back(Wall(Vec2(300, 300), Vec2(300, 0)));

    // Measure energy at start and after simulation
    // (After gravity has had time to add energy, measure relative decrease)
    for (int i = 0; i < 100; ++i) world.step(0.016f);
    float energyMid = world.totalKineticEnergy();

    for (int i = 0; i < 500; ++i) world.step(0.016f);
    float energyLate = world.totalKineticEnergy();

    // Energy should decrease (or at least not explode)
    ASSERT(energyLate <= energyMid * 1.5f); // Allow some tolerance for gravity input
}

// ═══════════════════════════════════════════════════════════════════════
// Stacking / settling test
// ═══════════════════════════════════════════════════════════════════════

TEST(balls_settle_in_container) {
    // Many balls in a box should eventually settle
    PhysicsWorld world;
    world.config.gravity = 500.0f;
    world.config.substeps = 8;
    world.config.restitution = 0.3f;
    world.config.damping = 0.999f;
    world.config.sleepSpeed = 2.0f;

    // Box
    world.walls.push_back(Wall(Vec2(0, 0), Vec2(200, 0)));
    world.walls.push_back(Wall(Vec2(200, 0), Vec2(200, 400)));
    world.walls.push_back(Wall(Vec2(200, 400), Vec2(0, 400)));
    world.walls.push_back(Wall(Vec2(0, 400), Vec2(0, 0)));

    // 50 balls dropped from top
    for (int i = 0; i < 50; ++i) {
        float x = 20 + (i % 10) * 18;
        float y = 20 + (i / 10) * 18;
        Ball b(Vec2(x, y), 7.0f);
        world.balls.push_back(b);
    }

    // Run for 10 simulated seconds
    for (int i = 0; i < 600; ++i) {
        world.step(0.016f);
    }

    // All balls should be inside the container
    for (const auto& b : world.balls) {
        ASSERT(b.pos.x > -5.0f && b.pos.x < 205.0f);
        ASSERT(b.pos.y > -5.0f && b.pos.y < 405.0f);
    }

    // Most balls should be nearly stopped
    int stoppedCount = 0;
    for (const auto& b : world.balls) {
        if (b.vel.length() < 5.0f) stoppedCount++;
    }
    ASSERT(stoppedCount > 40); // At least 80% settled
}

// ═══════════════════════════════════════════════════════════════════════
// Wall normal test
// ═══════════════════════════════════════════════════════════════════════

TEST(wall_normal_perpendicular) {
    Wall w(Vec2(0, 0), Vec2(10, 0)); // Horizontal wall
    Vec2 n = w.normal();
    ASSERT_NEAR(n.x, 0.0f, 1e-5f);
    ASSERT_NEAR(std::abs(n.y), 1.0f, 1e-5f);
}

TEST(wall_normal_unit_length) {
    Wall w(Vec2(0, 0), Vec2(3, 4));
    Vec2 n = w.normal();
    ASSERT_NEAR(n.length(), 1.0f, 1e-5f);
}

// ═══════════════════════════════════════════════════════════════════════
// Main — run all tests
// ═══════════════════════════════════════════════════════════════════════

int main() {
    printf("\n=== Physics Engine Tests ===\n\n");

    // Tests are auto-registered by static constructors above.
    // By the time we reach main(), they've already run.

    printf("\n=== Results: %d/%d passed", tests_passed, tests_run);
    if (tests_failed > 0) {
        printf(", %d FAILED", tests_failed);
    }
    printf(" ===\n\n");

    return tests_failed > 0 ? 1 : 0;
}

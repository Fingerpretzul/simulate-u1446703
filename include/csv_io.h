#pragma once
// csv_io.h — CSV scene file I/O for the physics simulator.
//
// CSV format for balls:
//   type,x,y,radius,r,g,b
//   ball,100.0,200.0,5.0,255,0,0
//
// CSV format for walls:
//   type,x1,y1,x2,y2
//   wall,50.0,50.0,1150.0,50.0
//
// The type column distinguishes balls from walls in a single file.
// Ball rows have 7 columns: type,x,y,radius,r,g,b
// Wall rows have 5 columns: type,x1,y1,x2,y2
// Lines starting with '#' are comments and are ignored.
// The first non-comment line should be a header row (also ignored).

#include "physics.h"
#include <string>
#include <vector>

// ── CSV loading ────────────────────────────────────────────────────
// Load a scene from a CSV file into a PhysicsWorld. Clears existing
// balls and walls before loading. Returns true on success.
// If the file contains balls without color (r,g,b all 0), hasColor
// remains false so the renderer uses default speed-based coloring.
bool loadSceneFromCSV(const std::string& filename, PhysicsWorld& world);

// ── CSV saving ─────────────────────────────────────────────────────
// Save the current scene (balls + walls) to a CSV file. Ball
// positions reflect their current (possibly settled) state.
// Returns true on success.
bool saveSceneToCSV(const std::string& filename, const PhysicsWorld& world);

// ── CSV parsing helpers (exposed for testing) ──────────────────────
// Parse a single CSV line into tokens, splitting on commas.
// Trims whitespace from each token.
std::vector<std::string> splitCSVLine(const std::string& line);

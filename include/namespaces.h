#pragma once
#include "ctpl_stl.h"
#include "raylib.h"

namespace screen
{
extern const int WIDTH;
extern const int HEIGHT;
extern const bool FULL_SCREEN;
} // namespace screen

namespace game
{
extern int SOLDIER_COUNT;
extern float DELTA;
extern const int TILE_SIZE;
extern const int WIDTH;
extern const int HEIGHT;
extern const int GRAPH_RESOLUTION;
extern const int numThreads;
extern ctpl::thread_pool pool;
} // namespace game

namespace shoshone
{
extern const Color yellow;
extern const Color orange;
extern const Color red;
extern const Color pink;
extern const Color maroon;
} // namespace shoshone
